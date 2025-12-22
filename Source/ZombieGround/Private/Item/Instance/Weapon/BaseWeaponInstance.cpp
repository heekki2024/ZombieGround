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

void UBaseWeaponInstance::AddSpread()
{
	UWeaponDataAsset* Data = GetItemData<UWeaponDataAsset>();
	if (!Data) return;

	// [수정 후] 커브를 이용한 동적 더하기
	float HeatToAdd = 1.0f; // 커브가 없을 때를 대비한 기본값

	if (Data->weaponStats.HeatToHeatPerShotCurve)
	{
		// "지금 내 열기(CurrentHeat)가 이정도인데, 몇(Y)을 더해야 하지?"
		HeatToAdd = Data->weaponStats.HeatToHeatPerShotCurve->GetFloatValue(CurrentHeat);
	}
    
	CurrentHeat += HeatToAdd;
    
	// 2. 탄퍼짐 계산 (커브가 있으면 커브값, 없으면 열기 그대로)
	if (Data->weaponStats.HeatToSpreadCurve)
	{
		CurrentSpreadAngle = Data->weaponStats.HeatToSpreadCurve->GetFloatValue(CurrentHeat);
	}
	else
	{
		CurrentSpreadAngle = CurrentHeat; // 커브 없으면 1:1 적용 (임시)
	}
    
	// 3. 마지막 발사 시간 갱신
	if (UWorld* World = GetWorld())
	{
		LastFireTime = World->GetTimeSeconds();
	}
}

void UBaseWeaponInstance::UpdateSpread(float DeltaTime)
{
	UWeaponDataAsset* Data = GetItemData<UWeaponDataAsset>();
	if (!Data) return;

	// 1. 쿨다운 딜레이 체크
	double TimeSinceFired = 0.0;
	if (UWorld* World = GetWorld())
	{
		TimeSinceFired = World->GetTimeSeconds() - LastFireTime;
	}

	if (TimeSinceFired > Data->weaponStats.CoolingDelay)
	{
		// 2. 열기 식히기 속도 계산
		float CooldownRate = 10.0f; // 기본 속도
		if (Data->weaponStats.HeatToCooldownCurve)
		{
			CooldownRate = Data->weaponStats.HeatToCooldownCurve->GetFloatValue(CurrentHeat);
		}
        
		// 3. 열기 감소 (0 이하로 안 내려가게)
		CurrentHeat = FMath::Max(0.0f, CurrentHeat - (CooldownRate * DeltaTime));
        
		// 4. 탄퍼짐 갱신
		if (Data->weaponStats.HeatToSpreadCurve)
		{
			CurrentSpreadAngle = Data->weaponStats.HeatToSpreadCurve->GetFloatValue(CurrentHeat);
		}
		else
		{
			CurrentSpreadAngle = CurrentHeat;
		}
	}
}

float UBaseWeaponInstance::GetCurrentSpread()
{
	return CurrentSpreadAngle;
}
