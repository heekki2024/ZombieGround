// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Instance/BaseInstance.h"
#include "WeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UWeaponInstance : public UBaseInstance
{
	GENERATED_BODY()
	
public:
	//원본 데이터 에셋
	UPROPERTY()
	class UWeaponDataAsset* weaponDA;
	
	UPROPERTY()
	int32 maxAmmo; // 최대 장전 가능 총알수
	
    //현재 상태 (변하는 값)
	UPROPERTY()
	int32 currentAmmo; // 현재 장전된 총알

	
public:
	virtual void InitInstance(class UBaseItemDataAsset* itemDataAsset) override;
	
	//최대 탄창 용량 계산
	UFUNCTION()
	int32 GetMaxAmmo();
	
	// UFUNCTION()
	// int32 UpgradeMaxAmmo(int upgradeAmount);
};
