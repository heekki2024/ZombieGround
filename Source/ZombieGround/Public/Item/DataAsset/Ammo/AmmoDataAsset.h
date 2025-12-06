// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "AmmoDataAsset.generated.h"

/**
 * 
 */
// UENUM(BlueprintType)
// enum class EAmmoType : uint8
// {
// 	AssaultRifle,
// 	LMG,
// 	SMG, // 볼트액션, 펌프액션 등
// 	Shotgun,
// 	SniperRifle,
// 	MAX             UMETA(Hidden)
// };



UCLASS()
class ZOMBIEGROUND_API UAmmoDataAsset : public UBaseDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EWeaponType ammoType;
	
};
