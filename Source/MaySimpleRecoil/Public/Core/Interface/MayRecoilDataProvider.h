// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MayRecoilDataProvider.generated.h"

class UMayRecoilData;

UINTERFACE()
class UMayRecoilDataProvider : public UInterface
{
	GENERATED_BODY()
};

class MAYSIMPLERECOIL_API IMayRecoilDataProvider
{
	GENERATED_BODY()

public:
	/** 
	 * Liefert eine Instanz von UMayRecoilData. 
	 * Nutzer können diese Methode überschreiben, um die Suchlogik anzupassen.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil")
	UMayRecoilData* ProvideRecoilData() const;
};
