// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/BaseUserWidget.h"
#include "AmmoCounter.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UAmmoCounter : public UBaseUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* currentAmmo;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InventoryAmmo;
	
	UFUNCTION()
	void UpdateCurrentAmmoText(int32 inCurrentAmmo);
	
	UFUNCTION()
	void UpdateInventoryAmmoText(int32 inInventoryAmmo);

};
