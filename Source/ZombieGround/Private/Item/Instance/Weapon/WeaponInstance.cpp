// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Instance/Weapon/WeaponInstance.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"

void UWeaponInstance::InitInstance(class UBaseItemDataAsset* itemDataAsset)
{
	Super::InitInstance(itemDataAsset);
	
	weaponDA = Cast<UWeaponDataAsset>(itemDataAsset);
	
	maxAmmo = GetMaxAmmo();
}

int32 UWeaponInstance::GetMaxAmmo()
{
	return weaponDA->weaponStats.maxAmmo;
}


