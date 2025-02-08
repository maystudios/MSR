#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Kismet/KismetMathLibrary.h"
#include "MayRecoilData.generated.h"

USTRUCT(BlueprintType)
struct FStaticPatternData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D Point = FVector2D::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float MinRecoilVerticalStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float MaxRecoilVerticalStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float MinRecoilHorizontalStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float MaxRecoilHorizontalStrength = 0.0f;
};

/**
 * DataAsset für das Recoil-System
 */
UCLASS(BlueprintType)
class MAYSIMPLERECOIL_API UMayRecoilData : public UDataAsset
{
	GENERATED_BODY()

public:

	// ============================== Recoil Strength ==============================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Random")
	float MinRecoilVerticalStrength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Random")
	float MaxRecoilVerticalStrength = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Random")
	bool ForceMinMaxVerticalStrength = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Random")
	float MinRecoilHorizontalStrength = -0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Random")
	float MaxRecoilHorizontalStrength = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Random")
	bool ForceMinMaxHorizontalStrength = false;

	// ============================== Recoil Pattern ==============================
	// TODO: Implement Recoil Pattern
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Static")
	// bool UseStaticPattern = false;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Pattern|Static")
	// TArray<FStaticPatternData> StaticPattern;
	
	// ============================== Recoil Scale ==============================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Scale")
	bool OverrideScaleSettings = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Scale")
	float RecoilScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Scale")
	float RecoilScaleSprint = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Scale")
	float RecoilScaleCrouch = 0.25f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Scale")
	float RecoilScaleJump = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Scale")
	float RecoilScaleADS = 1.0f;

	// ============================== Recoil Animation ==============================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Animation")
	bool UseThisRecoilAnimation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Animation")
	float RecoilSpeed = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Animation")
	TEnumAsByte<EEasingFunc::Type> RecoilInterpolation = EEasingFunc::Linear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Animation")
	float RecoilInterpolationEaseExp = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Animation")
	int32 RecoilInterpolationSteps = 2;

	// ============================== Recoil Reset ==============================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	bool UseThisResetAnimation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	bool RecoilResetRecoil = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	float RecoilResetDelay = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	float RecoilResetSpeed = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	TEnumAsByte<EEasingFunc::Type> RecoilResetInterpolation = EEasingFunc::Linear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	float RecoilResetInterpolationEaseExp = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Reset")
	int32 RecoilResetInterpolationSteps = 2;

	// ============================== Recoil Preview ==============================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Preview")
	int32 NumShots = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Preview")
	int32 Iterations = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaySimpleRecoil|Preview")
	int32 BulletRadius = 2;
};