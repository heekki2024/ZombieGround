// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Instance/Weapon/WeaponInstance.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"


void UWeaponInstance::InitWeaponInstance(class UWeaponDataAsset* weaponDataAsset)
{
	defaultWeaponData = weaponDataAsset;
	maxAmmo = defaultWeaponData->weaponStats.maxAmmo;
	currentAmmo = defaultWeaponData->weaponStats.maxAmmo;
	pickupMesh = defaultWeaponData->pickupMesh;
	actorMesh = defaultWeaponData->actorMesh;
}

UBaseItemDataAsset* UWeaponInstance::GetItemData()
{
	return defaultWeaponData;
}





