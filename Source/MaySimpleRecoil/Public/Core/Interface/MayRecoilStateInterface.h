#pragma once

#include "UObject/Interface.h"
#include "MayRecoilStateInterface.generated.h"

/**
 * Interface for dynamic state queries (Sprinting, Crouching, etc.)
 */
UINTERFACE(Blueprintable)
class MAYSIMPLERECOIL_API UMayRecoilStateInterface : public UInterface
{
	GENERATED_BODY()
};

class MAYSIMPLERECOIL_API IMayRecoilStateInterface
{
	GENERATED_BODY()

public:
	/** Returns whether the character is sprinting */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil|State")
	bool IsSprinting() const;

	/** Returns whether the character is crouching */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil|State")
	bool IsCrouching() const;

	/** Returns whether the character is jumping */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil|State")
	bool IsJumping() const;

	/** Returns whether the character is falling */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil|State")
	bool IsFalling() const;

	/** Returns whether the character can switch to ADS (Aiming Down Sights) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil|State")
	bool CanSwitchToADS() const;

	/** Returns whether the character has ADS (Aiming Down Sights) activated */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil|State")
	bool IsADS() const;
};
