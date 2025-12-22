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

public:
	// [추가] 총을 꺼내는 중인가? (이게 true면 발사 불가)
	bool bIsEquipping = false;

	// [추가] 총을 넣는 중인가?
	bool bIsUnequipping = false;
	
//총기반동
public:
	// 현재 열기 (0 ~ 100 등)
	float CurrentHeat = 0.0f;
	// 현재 탄퍼짐 각도 (도 Degree)
	float CurrentSpreadAngle = 0.0f;
	// 마지막 사격 시간
	double LastFireTime = 0.0;

	// Lyra의 로직을 본딴 함수들
	void AddSpread();           // 발사 시 열기 추가
	void UpdateSpread(float DeltaTime); // 틱마다 열기 식힘
	float GetCurrentSpread();   // 현재 탄퍼짐 각도 반환
};
