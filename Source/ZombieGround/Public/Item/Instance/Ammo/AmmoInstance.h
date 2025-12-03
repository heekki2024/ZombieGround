// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Instance/BaseInstance.h"
#include "AmmoInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UAmmoInstance : public UBaseInstance
{
	GENERATED_BODY()
	
public:
	// enum class EAmmoType ammoType;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// int32 maxAmmo = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAmmoDataAsset* defaultAmmoData; 
	
public:
	UFUNCTION()
	virtual void InitAmmoInstance(class UAmmoDataAsset* ammoDataAsset);

	virtual UBaseItemDataAsset* GetItemData() override;

};
