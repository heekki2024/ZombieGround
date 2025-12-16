// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Instance/Weapon/BaseWeaponInstance.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"


void UBaseWeaponInstance::InitInstance(class UBaseDataAsset* itemDataAsset)
{
	if (UWeaponDataAsset* weaponDataAsset = Cast<UWeaponDataAsset>(itemDataAsset))
	{
		this->defaultItemData = weaponDataAsset;
		maxAmmo = weaponDataAsset->weaponStats.maxAmmo;
		currentAmmo = weaponDataAsset->weaponStats.maxAmmo;
		pickupMesh = weaponDataAsset->pickupMesh;
		actorMesh = weaponDataAsset->actorMesh;
		reloadDuration = weaponDataAsset->weaponStats.reloadDuration;
	}
}
