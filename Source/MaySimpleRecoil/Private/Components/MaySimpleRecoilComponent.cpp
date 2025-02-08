// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MaySimpleRecoilComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UMaySimpleRecoilComponent::UMaySimpleRecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMaySimpleRecoilComponent::TrySpawnRecoilWorkerInstance()
{
	if (!RecoilWorkerInstance)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = CharacterOwner;
		SpawnParams.Instigator = CharacterOwner->GetInstigator();

		RecoilWorkerInstance = GetWorld()->SpawnActor<AMayRecoilWorker>(RecoilWorker, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (RecoilWorkerInstance)
		{
			RecoilWorkerInstance->SetCurrentComponent(this);
		}
	}
}

void UMaySimpleRecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ACharacter>(GetOwner());

	TrySpawnRecoilWorkerInstance();
}

void UMaySimpleRecoilComponent::InitializeComponent()
{
	Super::InitializeComponent();
	PrimaryComponentTick.bCanEverTick = false;
	CharacterOwner = nullptr;
}

void UMaySimpleRecoilComponent::Recoil()
{
	TrySpawnRecoilWorkerInstance();
	
	if (RecoilWorkerInstance && RecoilData)
	{
		RecoilWorkerInstance->SetCurrentRecoilData(RecoilData);
		RecoilWorkerInstance->Recoil();
	}
}

void UMaySimpleRecoilComponent::OnYawAdded(float Yaw)
{
	if (RecoilWorkerInstance)
	{
		RecoilWorkerInstance->OnYawAdded(Yaw);
	}
}

void UMaySimpleRecoilComponent::OnPitchAdded(float Pitch)
{
	if (RecoilWorkerInstance)
	{
		RecoilWorkerInstance->OnPitchAdded(Pitch);
	}
}

void UMaySimpleRecoilComponent::UpdatePlayerYawAndPitch_Implementation(float Yaw, float Pitch)
{
	if (CharacterOwner)
	{
		CharacterOwner->AddControllerYawInput(Yaw);
		CharacterOwner->AddControllerPitchInput(Pitch);
	}
}

void UMaySimpleRecoilComponent::TrySetADS(bool bNewADS)
{
	if (CanSwitchToADS_Implementation())
	{
		ADS = bNewADS;
	}
}

bool UMaySimpleRecoilComponent::IsSprinting_Implementation() const
{
	if (CharacterOwner)
	{
		return CharacterOwner->GetVelocity().Size() > SprintSpeedThreshold;
	}
	return false;
}

bool UMaySimpleRecoilComponent::IsCrouching_Implementation() const
{
	return (CharacterOwner) ? CharacterOwner->bIsCrouched : false;
}

bool UMaySimpleRecoilComponent::IsJumping_Implementation() const
{
	if (CharacterOwner && CharacterOwner->GetCharacterMovement())
	{
		return CharacterOwner->GetCharacterMovement()->Velocity.Z > 0.0f && CharacterOwner->GetCharacterMovement()->IsFalling();
	}
	return false;
}

bool UMaySimpleRecoilComponent::IsFalling_Implementation() const
{
	if (CharacterOwner && CharacterOwner->GetCharacterMovement())
	{
		return CharacterOwner->GetCharacterMovement()->Velocity.Z < 0.0f && CharacterOwner->GetCharacterMovement()->IsFalling();
	}
	return false;
}

bool UMaySimpleRecoilComponent::CanSwitchToADS_Implementation() const
{
	return !this->IsJumping_Implementation() &&
		   !this->IsFalling_Implementation();
}

bool UMaySimpleRecoilComponent::IsADS_Implementation() const
{
	return ADS;
}

UMayRecoilData* UMaySimpleRecoilComponent::ProvideRecoilData_Implementation() const
{
	// Standardimplementierung: rekursive Suche ab dem Owner
	AActor* OwnerActor = GetOwner();
	UMayRecoilData* LocalRecoilData = nullptr;
	// if (OwnerActor)
	// {
	// 	LocalRecoilData = DefaultFindRecoilData(OwnerActor);
	// }
	if (!LocalRecoilData)
	{
		if (RecoilData)
		{
			LocalRecoilData = RecoilData;
		}
	}

	return LocalRecoilData;
}