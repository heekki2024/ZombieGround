// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Instance/BaseInstance.h"
#include "BaseWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UBaseWeaponInstance : public UBaseInstance
{
	GENERATED_BODY()
	
public:
	// //원본 데이터 에셋
	// UPROPERTY()
	// class UWeaponDataAsset* defaultWeaponData;
	
	UPROPERTY()
	int32 maxAmmo; // 최대 장전 가능 총알수
	
    //현재 상태 (변하는 값)
	UPROPERTY()
	int32 currentAmmo; // 현재 장전된 총알
	
	// [추가] 현재 장전 중인지 확인하는 플래그
	UPROPERTY()
	bool bIsReloading = false;

	// [추가] 인벤토리에 보이지 않는 내부 예비 탄약 (기본 지급 무기용)
	UPROPERTY()
	int32 InternalReserveAmmo = 0;
	
	UPROPERTY()
	float reloadDuration;
	
public:
	// 현재 켜져있는지 상태 확인용
	bool bIsLightOn = true;
public:
	virtual void InitInstance(class UBaseDataAsset* itemDataAsset) override;
	
	// virtual UBaseDataAsset* GetItemData() override;
};
