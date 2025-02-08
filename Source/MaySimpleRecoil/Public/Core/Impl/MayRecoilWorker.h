/******************************************************************************
 * Copyright (c) 2023 MayStudios (Sven Maibaum).
 * All Rights Reserved.
 *
 * This software and its accompanying documentation are the exclusive property
 * of MayStudios (Sven Maibaum). No part of this software may be reproduced,
 * distributed, modified, or transmitted in any form or by any means, including
 * without limitation electronic, mechanical, or otherwise, without the prior
 * written permission of the owner.
 *
 * This software is licensed for sale exclusively on fab. Unauthorized use,
 * copying, or distribution is strictly prohibited.
 *
 * For licensing inquiries or further information, please contact:
 * [Insert your contact information or website URL here].
 *
 * Author: Sven Maibaum
 * Project: MayStudios
*****************************************************************************/


#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "MayRecoilWorker.generated.h"

class UMaySimpleRecoilComponent;
class UMayRecoilData;

/**
 * @brief Actor that handles the recoil effect.
 *
 * This actor uses two timelines:
 * - One timeline (AddRecoilTimeline) to apply the recoil effect.
 * - Another timeline (ResetRecoilTimeline) to reset the recoil effect.
 *
 * It works in conjunction with a recoil component (UMaySimpleRecoilComponent) and recoil data (UMayRecoilData)
 * to calculate and update the player's yaw and pitch values accordingly.
 */
UCLASS(Blueprintable, ShowCategories = (MayRecoil),
	HideCategories = (Rendering, Replication, Input, Actor, Cooking, Collision, HLOD, Mobile, Tags, Physics,
		Level_Instance, World_Partition, Data_Layers, Events, Actor_Tick, Advanced), HideFunctions = (PointDamage))
class MAYSIMPLERECOIL_API AMayRecoilWorker : public AActor
{
	GENERATED_BODY()

public:
	// ================================================================
	// Constructor & Lifecycle Functions
	// ================================================================

	/**
	 * @brief Constructor.
	 */
	AMayRecoilWorker();

protected:
	/**
	 * @brief Called when the game starts or when spawned.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * @brief Called every frame.
	 * @param DeltaTime The time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaTime) override;

	// ================================================================
	// Setup: Component and Data Configuration
	// ================================================================

	/**
	 * @brief Sets the current recoil component.
	 * @param NewComponent Pointer to the new recoil component.
	 */
	UFUNCTION(BlueprintCallable, Category = "MayRecoil")
	void SetCurrentComponent(UMaySimpleRecoilComponent* NewComponent);

	/**
	 * @brief Sets the current recoil data.
	 * @param RecoilData Pointer to the new recoil data.
	 */
	UFUNCTION(BlueprintCallable, Category = "MayRecoil")
	void SetCurrentRecoilData(UMayRecoilData* RecoilData);

	// ================================================================
	// Recoil Functionality
	// ================================================================

	/**
	 * @brief Triggers the recoil effect.
	 *
	 * This function initiates the timeline that applies the recoil effect to the player.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	void Recoil();
	virtual void Recoil_Implementation();

	/**
	 * @brief Resets the recoil effect.
	 *
	 * This function is called after a delay to reset the recoil to its original state.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	void ResetRecoil();
	virtual void ResetRecoil_Implementation();

	/**
	 * @brief Immediately resets the recoil state.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	void ResetRecoilState();
	virtual void ResetRecoilState_Implementation();

	// ================================================================
	// Recoil Scaling and Strength Calculation
	// ================================================================

	/**
	 * @brief Calculates the recoil scale factor based on the current component state.
	 * @return The calculated recoil scale.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	float GetRecoilScale();
	virtual float GetRecoilScale_Implementation();

	/**
	 * @brief Calculates the yaw and pitch strengths for the recoil.
	 * @param OutYaw Output parameter for the calculated yaw strength.
	 * @param OutPitch Output parameter for the calculated pitch strength.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	void GetRecoilYawAndPitchStrength(float& OutYaw, float& OutPitch);
	virtual void GetRecoilYawAndPitchStrength_Implementation(float& OutYaw, float& OutPitch);

	/**
	 * @brief Called when additional yaw is added.
	 * @param Yaw The yaw value to add.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	void OnYawAdded(float Yaw);
	virtual void OnYawAdded_Implementation(float Yaw);

	/**
	 * @brief Called when additional pitch is added.
	 * @param Pitch The pitch value to add.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MayRecoil")
	void OnPitchAdded(float Pitch);
	virtual void OnPitchAdded_Implementation(float Pitch);

	// ================================================================
	// Timeline Callback Functions
	// ================================================================

	// ---- Add-Recoil Timeline ----

	/**
	 * @brief Callback called during the AddRecoil timeline.
	 * @param Value The interpolation value from the timeline.
	 */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void AddRecoilTimelineFloatReturn(float Value);

	/**
	 * @brief Called when the AddRecoil timeline finishes playing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnAddRecoilTimelineFinished();

	/**
	 * @brief Delayed callback after the AddRecoil timeline has finished.
	 */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void AfterAddRecoilTimelineDelay();

	// ---- Reset-Recoil Timeline ----

	/**
	 * @brief Callback called during the ResetRecoil timeline.
	 * @param Value The interpolation value from the timeline.
	 */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void ResetRecoilTimelineFloatReturn(float Value);

	/**
	 * @brief Called when the ResetRecoil timeline finishes playing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnResetRecoilTimelineFinished();

	// ================================================================
	// Properties
	// ================================================================

	/** Pointer to the current recoil component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MayRecoil")
	UMaySimpleRecoilComponent* CurrentComponent;

	/** Pointer to the current recoil data. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MayRecoil")
	UMayRecoilData* CurrentRecoilData;

	/** Timeline for applying recoil. */
	FTimeline AddRecoilTimeline;

	/** Curve used for interpolation in the AddRecoil timeline. */
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* AddRecoilCurve;

	/** Timeline for resetting recoil. */
	FTimeline ResetRecoilTimeline;

	/** Curve used for interpolation in the ResetRecoil timeline. */
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* ResetRecoilCurve;

	// ================================================================
	// Temporary Runtime Variables
	// ================================================================

	/** Temporary pitch offset used during recoil reset. */
	UPROPERTY(EditAnywhere, Category = "Temp Runtime")
	float TempRecoilResetPitchOffset = 0.0f;

	/** Temporary storage for the added yaw value. */
	UPROPERTY(EditAnywhere, Category = "Temp Runtime")
	float TempAddedYaw = 0.0f;

	/** Temporary storage for the added pitch value. */
	UPROPERTY(EditAnywhere, Category = "Temp Runtime")
	float TempAddedPitch = 0.0f;

private:
	/**
	 * @brief Initializes the recoil timelines.
	 *
	 * Checks if the required curves are set and binds the corresponding callbacks
	 * and finish events for both timelines.
	 */
	void initialize_recoil_timelines();

	/** Internal variable: current calculated yaw value for recoil. */
	float CurrentOutYaw = 0.0f;

	/** Internal variable: current calculated pitch value for recoil. */
	float CurrentOutPitch = 0.0f;

	/** Internal variable: accumulated values of added pitch and yaw. */
	FVector2D AddedPitchAndYaw = FVector2D::ZeroVector;

	/** Internal variable: temporary storage for pitch and yaw during reset. */
	FVector2D TempAddedPitchAndYaw = FVector2D::ZeroVector;
};
