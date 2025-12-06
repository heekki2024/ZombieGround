// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/Equippable/BaseEquippable.h"
#include "Item/Pickup/BasePickup.h"
#include "BaseDataAsset.generated.h"

/**
 * 
 */
// 아이템 유형 (필요 시 확장)
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Ammo,   // 총알, 재료
	Weapon,     // 무기
	Gadget      // 키트, 수류탄
};

UENUM(BlueprintType)
enum class ESortPriority : uint8
{
	AssaultRifleAmmo = 0, // 소총
	LMGAmmo = 1,          // 기관총
	SMGAmmo = 2,			//기관단총
	SniperRifleAmmo = 3,   // 스나이퍼
	ShotgunAmmo = 4,  //샷건
	PistolAmmo = 5  //샷건
};

UCLASS(BlueprintType)
class ZOMBIEGROUND_API UBaseDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText itemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemType itemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	class UStaticMesh* pickupMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	class USkeletalMesh* actorMesh;
	
	UPROPERTY(EditDefaultsOnly)
	ESortPriority sortPriority;
	
	UPROPERTY(EditDefaultsOnly)
	int32 defaultSpawnQuantity;
	
	UPROPERTY(EditDefaultsOnly)
	int32 maxQuantity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<class ABasePickup> pickupClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<class ABaseEquippable> actorClass;
	
	
public:
	UFUNCTION(BlueprintCallable)
	int32 GetSortPriority() const;
	
	// 반환 타입을 TSubclassOf<T>로 감싸야 합니다.
	template<typename T>
	TSubclassOf<T> GetPickupClass() const 
	{
		// pickupClass가 T의 자식인지 확인하고 반환
		if (pickupClass && pickupClass->IsChildOf(T::StaticClass()))
		{
			return *pickupClass;
		}
		return nullptr;
	}

	
	// 반환 타입을 TSubclassOf<T>로 감싸야 합니다.
	template<typename T>
	TSubclassOf<T> GetActorClass() const 
	{
		// pickupClass가 T의 자식인지 확인하고 반환
		if (actorClass && actorClass->IsChildOf(T::StaticClass()))
		{
			return *actorClass;
		}
		return nullptr;
	}
};
