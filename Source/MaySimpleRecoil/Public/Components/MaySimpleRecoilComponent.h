// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Interface/MayRecoilDataProvider.h"
#include "Core/Interface/MayRecoilStateInterface.h"
#include "Core/Impl/MayRecoilWorker.h"
#include "MaySimpleRecoilComponent.generated.h"

class ACharacter;


UCLASS(ClassGroup=(MayRecoil), meta=(BlueprintSpawnableComponent), Blueprintable, HideCategories=(Object, LOD, Physics, Lighting, TextureStreaming, Collision, HLOD, Mobile, VirtualTexture, ComponentReplication))
class MAYSIMPLERECOIL_API UMaySimpleRecoilComponent : public UActorComponent, public IMayRecoilStateInterface, public IMayRecoilDataProvider
{
	GENERATED_BODY()

public:
	UMaySimpleRecoilComponent();
	void TrySpawnRecoilWorkerInstance();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
public:
	
	UFUNCTION(BlueprintCallable, Category = "MaySimpleRecoil")
	void Recoil();

	UFUNCTION(BlueprintCallable, Category = "MaySimpleRecoil")
	void OnYawAdded(float Yaw);

	UFUNCTION(BlueprintCallable, Category = "MaySimpleRecoil")
	void OnPitchAdded(float Pitch);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MaySimpleRecoil")
	void UpdatePlayerYawAndPitch(float Yaw, float Pitch);
	virtual void UpdatePlayerYawAndPitch_Implementation(float Yaw, float Pitch);

	// ============================== Recoil State ==============================
	
	UPROPERTY(BlueprintReadOnly, Category = "Recoil|State")
	bool ADS = false;

	UFUNCTION(BlueprintCallable, Category = "Recoil|State")
	void TrySetADS(bool bNewADS);
	
	// ============================== Recoil Strength ==============================

	UPROPERTY(EditAnywhere, Category = "MaySimpleRecoil")
	bool bEnableRecoil = true;

	// ============================== Recoil Animation ==============================
	
	/** Interner Zeiger auf den ACharacter, dessen Zustand abgefragt wird */
	UPROPERTY(BlueprintReadOnly, Category = "MaySimpleRecoil|State")
	ACharacter* CharacterOwner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MaySimpleRecoil|State")
	float SprintSpeedThreshold = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaySimpleRecoil")
	TSubclassOf<AMayRecoilWorker> RecoilWorker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaySimpleRecoil")
	UMayRecoilData* RecoilData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaySimpleRecoil")
	AMayRecoilWorker* RecoilWorkerInstance = nullptr;

	virtual bool IsSprinting_Implementation() const override;
	virtual bool IsCrouching_Implementation() const override;
	virtual bool IsJumping_Implementation() const override;
	virtual bool IsFalling_Implementation() const override;
	virtual bool CanSwitchToADS_Implementation() const override;
	virtual bool IsADS_Implementation() const override;
	
	virtual UMayRecoilData* ProvideRecoilData_Implementation() const override;
};