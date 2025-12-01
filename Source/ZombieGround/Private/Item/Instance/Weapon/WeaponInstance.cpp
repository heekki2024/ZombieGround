// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Instance/Weapon/WeaponInstance.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"


void UWeaponInstance::InitWeaponInstance(class UWeaponDataAsset* weaponDataAsset)
{
	defaultWeaponData = weaponDataAsset;
	currentAmmo = defaultWeaponData->weaponStats.maxAmmo;
	pickupMesh = defaultWeaponData->pickupMesh;
	actorMesh = defaultWeaponData->actorMesh;
}





