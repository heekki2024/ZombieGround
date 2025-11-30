// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseItemDataAsset.generated.h"

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

UCLASS()
class ZOMBIEGROUND_API UBaseItemDataAsset : public UPrimaryDataAsset
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
	
	// 이 아이템을 월드에 버릴 때 생성될 Pickup 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<class ABasePickup> pickupClass;
	

};
