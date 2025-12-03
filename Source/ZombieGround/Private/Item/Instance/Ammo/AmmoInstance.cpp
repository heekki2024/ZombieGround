// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Instance/Ammo/AmmoInstance.h"

#include "Item/DataAsset/Ammo/AmmoDataAsset.h"

void UAmmoInstance::InitAmmoInstance(class UAmmoDataAsset* ammoDataAsset)
{
	defaultAmmoData = ammoDataAsset;
	currentQuantity = defaultAmmoData->defaultSpawnQuantity;
}

UBaseItemDataAsset* UAmmoInstance::GetItemData()
{
	return defaultAmmoData;
}
