// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Instance/Ammo/AmmoInstance.h"
#include "Item/DataAsset/Ammo/AmmoDataAsset.h"


void UAmmoInstance::InitInstance(class UBaseDataAsset* itemDataAsset)
{
	if (UAmmoDataAsset* ammoData = Cast<UAmmoDataAsset>(itemDataAsset))
	{
		this->defaultItemData = itemDataAsset;
		currentQuantity = this->defaultItemData->defaultSpawnQuantity;
	}
}
