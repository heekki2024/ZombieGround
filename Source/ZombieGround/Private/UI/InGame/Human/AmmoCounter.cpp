// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Human/AmmoCounter.h"

#include "Components/TextBlock.h"


void UAmmoCounter::UpdateCurrentAmmoText(int32 inCurrentAmmo)
{
	if (currentAmmo)
	{
		currentAmmo->SetText(FText::AsNumber(inCurrentAmmo));
	}
    
	// if (InventoryAmmo)
	// {
	// 	InventoryAmmo->SetText(FText::AsNumber(inInventoryAmmo));
	// }
}

void UAmmoCounter::UpdateInventoryAmmoText(int32 inInventoryAmmo)
{
	if (InventoryAmmo)
	{
		InventoryAmmo->SetText(FText::AsNumber(inInventoryAmmo));
	}
}


