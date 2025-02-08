// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Impl/MayRecoilWorker.h"
#include "Components/MaySimpleRecoilComponent.h"
#include "Core/Data/MayRecoilData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h" // For ease functions

// ============================================================================
// Constructor and Initialization
// ============================================================================

/**
 * @brief Constructor.
 *
 * Initializes the curve pointers to nullptr and enables ticking.
 */
AMayRecoilWorker::AMayRecoilWorker()
	: AddRecoilCurve(nullptr)
	, ResetRecoilCurve(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

/**
 * @brief Initializes the timelines for applying and resetting recoil.
 *
 * Checks if the required curves are set and binds the appropriate interpolation
 * callbacks and finish events for both timelines.
 */
void AMayRecoilWorker::initialize_recoil_timelines()
{
	// --- Add-Recoil Timeline ---
	if (AddRecoilCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("AddRecoilTimelineFloatReturn"));
		
		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("OnAddRecoilTimelineFinished"));
		
		AddRecoilTimeline.AddInterpFloat(AddRecoilCurve, TimelineCallback);
		AddRecoilTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		AddRecoilTimeline.SetLooping(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AddRecoilCurve is not set!"));
	}

	// --- Reset-Recoil Timeline ---
	if (ResetRecoilCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("ResetRecoilTimelineFloatReturn"));
		
		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("OnResetRecoilTimelineFinished"));
		
		ResetRecoilTimeline.AddInterpFloat(ResetRecoilCurve, TimelineCallback);
		ResetRecoilTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		ResetRecoilTimeline.SetLooping(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ResetRecoilCurve is not set!"));
	}
}

// ============================================================================
// Actor Lifecycle Functions
// ============================================================================

/**
 * @brief Called when the game starts or when the actor is spawned.
 */
void AMayRecoilWorker::BeginPlay()
{
	Super::BeginPlay();
	initialize_recoil_timelines();
}

/**
 * @brief Called every frame.
 * @param DeltaTime Time elapsed since the last frame.
 */
void AMayRecoilWorker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update timelines
	AddRecoilTimeline.TickTimeline(DeltaTime);
	ResetRecoilTimeline.TickTimeline(DeltaTime);
	
	// Debug messages displaying current recoil state
	GEngine->AddOnScreenDebugMessage(200, 5.0f, FColor::Blue, FString::Printf(TEXT("TempAddedPitch: %f %f"), AddedPitchAndYaw.X, AddedPitchAndYaw.Y));
	GEngine->AddOnScreenDebugMessage(201, 5.0f, FColor::Blue, FString::Printf(TEXT("TempAddedYaw: %f"), TempAddedYaw));
	GEngine->AddOnScreenDebugMessage(202, 5.0f, FColor::Blue, FString::Printf(TEXT("TempAddedPitch: %f"), TempAddedPitch));
}

// ============================================================================
// Component and Data Setup Functions
// ============================================================================

/**
 * @brief Sets the current recoil component.
 * @param NewComponent Pointer to the new recoil component.
 */
void AMayRecoilWorker::SetCurrentComponent(UMaySimpleRecoilComponent* NewComponent)
{
	CurrentComponent = NewComponent;
}

/**
 * @brief Sets the current recoil data.
 * @param RecoilData Pointer to the new recoil data.
 */
void AMayRecoilWorker::SetCurrentRecoilData(UMayRecoilData* RecoilData)
{
	CurrentRecoilData = RecoilData;
}

// ============================================================================
// Recoil Functionality
// ============================================================================

/**
 * @brief Triggers the recoil effect.
 *
 * Calculates the current recoil strengths, stops any running timelines,
 * resets temporary variables, and starts the AddRecoil timeline.
 */
void AMayRecoilWorker::Recoil_Implementation()
{
	if (!CurrentRecoilData) return; // RecoilData must be valid
	
	// Calculate recoil strengths
	GetRecoilYawAndPitchStrength_Implementation(CurrentOutYaw, CurrentOutPitch);

	// Stop both timelines if they are playing
	AddRecoilTimeline.Stop();
	ResetRecoilTimeline.Stop();

	// Reset temporary values
	TempRecoilResetPitchOffset = 0.0f;
	TempAddedPitch = 0.0f;
	TempAddedYaw = 0.0f;
	
	// Set the play rate based on recoil data and play from the start
	AddRecoilTimeline.SetPlayRate(CurrentRecoilData->RecoilSpeed);
	AddRecoilTimeline.PlayFromStart();
}

/**
 * @brief Callback for the AddRecoil timeline, called during interpolation.
 * @param Value Interpolation value provided by the timeline.
 */
void AMayRecoilWorker::AddRecoilTimelineFloatReturn(float Value)
{
	GEngine->AddOnScreenDebugMessage(203, 5.0f, FColor::Magenta, FString::Printf(TEXT("Value: %f"), Value));

	// Calculate eased yaw and pitch values using the ease function
	float EaseYaw = UKismetMathLibrary::Ease(0.0f, CurrentOutYaw, Value, CurrentRecoilData->RecoilInterpolation, CurrentRecoilData->RecoilInterpolationEaseExp, CurrentRecoilData->RecoilInterpolationSteps);
	float EasePitch = UKismetMathLibrary::Ease(0.0f, CurrentOutPitch, Value, CurrentRecoilData->RecoilInterpolation, CurrentRecoilData->RecoilInterpolationEaseExp, CurrentRecoilData->RecoilInterpolationSteps);
	
	// Update the player's yaw and pitch using the calculated differences
	CurrentComponent->UpdatePlayerYawAndPitch(
		EaseYaw - TempAddedYaw,
		EasePitch - TempAddedPitch
	);

	// Accumulate the added values
	AddedPitchAndYaw = AddedPitchAndYaw + FVector2D(EaseYaw - TempAddedYaw, EasePitch - TempAddedPitch);

	TempAddedYaw = EaseYaw;
	TempAddedPitch = EasePitch;
}

/**
 * @brief Called when the AddRecoil timeline finishes playing.
 *
 * Triggers a delayed callback that will initiate the recoil reset process.
 */
void AMayRecoilWorker::OnAddRecoilTimelineFinished()
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("AfterAddRecoilTimelineDelay");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;
	
	UKismetSystemLibrary::RetriggerableDelay(this, CurrentRecoilData->RecoilResetDelay, LatentInfo);
}

/**
 * @brief Callback executed after the delay following the AddRecoil timeline.
 *
 * Initiates the recoil reset process.
 */
void AMayRecoilWorker::AfterAddRecoilTimelineDelay()
{
	ResetRecoil_Implementation();
}

/**
 * @brief Calculates the recoil scale factor based on the current component state and recoil data.
 * @return The calculated recoil scale.
 */
float AMayRecoilWorker::GetRecoilScale_Implementation()
{
	if (!CurrentComponent) return 1.0f; // Component must be valid
	if (!CurrentRecoilData) return 1.0f;  // RecoilData must be valid
	return CurrentRecoilData->RecoilScale *
		(CurrentComponent->IsCrouching_Implementation() ? CurrentRecoilData->RecoilScaleCrouch : 1) *
		(CurrentComponent->IsSprinting_Implementation() ? CurrentRecoilData->RecoilScaleSprint : 1) *
		(CurrentComponent->IsJumping_Implementation() ? CurrentRecoilData->RecoilScaleJump : 1) *
		(CurrentComponent->IsADS_Implementation() ? CurrentRecoilData->RecoilScaleADS : 1);
}

/**
 * @brief Calculates the yaw and pitch strengths for the recoil.
 * @param OutYaw Output parameter for the calculated yaw strength.
 * @param OutPitch Output parameter for the calculated pitch strength.
 */
void AMayRecoilWorker::GetRecoilYawAndPitchStrength_Implementation(float& OutYaw, float& OutPitch)
{
	if (!CurrentComponent) return; // Component must be valid
	if (!CurrentRecoilData) return;  // RecoilData must be valid

	float Scale = GetRecoilScale_Implementation();
	
	// Calculate vertical (pitch) recoil strength
	OutPitch = (CurrentRecoilData->ForceMinMaxVerticalStrength ? 
				(FMath::RandBool() ? CurrentRecoilData->MaxRecoilVerticalStrength : CurrentRecoilData->MinRecoilVerticalStrength) : 
				FMath::FRandRange(CurrentRecoilData->MinRecoilVerticalStrength, CurrentRecoilData->MaxRecoilVerticalStrength)
			   ) * Scale * -1;
	// Calculate horizontal (yaw) recoil strength
	OutYaw = (CurrentRecoilData->ForceMinMaxHorizontalStrength ? 
			  (FMath::RandBool() ? CurrentRecoilData->MaxRecoilHorizontalStrength : CurrentRecoilData->MinRecoilHorizontalStrength) : 
			  FMath::FRandRange(CurrentRecoilData->MinRecoilHorizontalStrength, CurrentRecoilData->MaxRecoilHorizontalStrength)
			 ) * Scale;
}

/**
 * @brief Initiates the recoil reset process.
 *
 * Checks if recoil reset is enabled and if the add recoil timeline has stopped,
 * then starts the reset recoil timeline.
 */
void AMayRecoilWorker::ResetRecoil_Implementation()
{
	if (!CurrentRecoilData->RecoilResetRecoil) return; // Recoil reset must be enabled
	if (AddRecoilTimeline.IsPlaying()) return; // AddRecoilTimeline must not be playing

	ResetRecoilTimeline.Stop();
	
	// Reset temporary values for the reset process
	TempAddedYaw = 0.0f;
	TempAddedPitch = 0.0f;
	TempAddedPitchAndYaw = AddedPitchAndYaw;

	// Set the play rate for the reset timeline and play from start
	ResetRecoilTimeline.SetPlayRate(CurrentRecoilData->RecoilResetSpeed);
	ResetRecoilTimeline.PlayFromStart();
}

/**
 * @brief Callback for the ResetRecoil timeline, called during interpolation.
 * @param Value Interpolation value provided by the timeline.
 */
void AMayRecoilWorker::ResetRecoilTimelineFloatReturn(float Value)
{
	GEngine->AddOnScreenDebugMessage(204, 5.0f, FColor::Magenta, FString::Printf(TEXT("Value: %f"), Value));
	if (!CurrentComponent) return; // Component must be valid
	if (!CurrentRecoilData) return;  // RecoilData must be valid
	
	// Calculate eased values for yaw and pitch during the reset process
	float EaseYaw = UKismetMathLibrary::Ease(0.0f, TempAddedPitchAndYaw.X, Value, CurrentRecoilData->RecoilResetInterpolation, CurrentRecoilData->RecoilResetInterpolationEaseExp, CurrentRecoilData->RecoilResetInterpolationSteps);
	float EasePitch = UKismetMathLibrary::Ease(0.0f, TempAddedPitchAndYaw.Y, Value, CurrentRecoilData->RecoilResetInterpolation, CurrentRecoilData->RecoilResetInterpolationEaseExp, CurrentRecoilData->RecoilResetInterpolationSteps);
	
	// Update the player's yaw and pitch to reverse the recoil
	CurrentComponent->UpdatePlayerYawAndPitch(
		(EaseYaw - TempAddedYaw) * -1.0f,
		(EasePitch - TempAddedPitch) * -1.0f
	);

	// Adjust the accumulated values
	AddedPitchAndYaw = AddedPitchAndYaw - FVector2D(EaseYaw - TempAddedYaw, EasePitch - TempAddedPitch);
	TempAddedYaw = EaseYaw;
	TempAddedPitch = EasePitch;
}

/**
 * @brief Called when the ResetRecoil timeline finishes playing.
 */
void AMayRecoilWorker::OnResetRecoilTimelineFinished()
{
	// No additional logic is needed here.
}

/**
 * @brief Immediately resets the recoil state.
 *
 * Stops both timelines and resets the accumulated recoil values.
 */
void AMayRecoilWorker::ResetRecoilState_Implementation()
{
	ResetRecoilTimeline.Stop();
	AddRecoilTimeline.Stop();

	AddedPitchAndYaw = FVector2D::ZeroVector;
	TempAddedPitchAndYaw = FVector2D::ZeroVector;
}

/**
 * @brief Handles the addition of yaw.
 *
 * Interpolates the accumulated yaw value back to zero.
 * @param Yaw The yaw value to add.
 */
void AMayRecoilWorker::OnYawAdded_Implementation(float Yaw)
{
	if (Yaw == 0.0f) return; // Validate input

	AddedPitchAndYaw = FVector2D(FMath::FInterpTo(AddedPitchAndYaw.X, 0, GetWorld()->GetDeltaSeconds(), 10), AddedPitchAndYaw.Y);
}

/**
 * @brief Handles the addition of pitch.
 *
 * Increases the accumulated pitch value and checks if a reset should be triggered during the reset process.
 * @param Pitch The pitch value to add.
 */
void AMayRecoilWorker::OnPitchAdded_Implementation(float Pitch)
{
	if (Pitch > 0.0f)
	{
		// Add pitch but do not exceed 0.0f (to prevent over-rotation)
		AddedPitchAndYaw = FVector2D(AddedPitchAndYaw.X, FMath::Min(AddedPitchAndYaw.Y + Pitch, 0.0f));
	}

	// If the reset timeline is playing and a negative pitch is applied,
	// check if the recoil reset should be triggered again.
	if (ResetRecoilTimeline.IsPlaying() && Pitch < 0.0f)
	{
		TempRecoilResetPitchOffset += FMath::Abs(Pitch);
		if (TempRecoilResetPitchOffset >= FMath::Abs(TempAddedPitchAndYaw.Y) * 1.1)
		{
			ResetRecoil_Implementation();
			TempRecoilResetPitchOffset = 0.0f;
		}
	}
}
