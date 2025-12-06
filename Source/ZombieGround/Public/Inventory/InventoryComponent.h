// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Instance/Ammo/AmmoInstance.h"
#include "InventoryComponent.generated.h"

// UI 갱신을 위한 멀티캐스트 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UENUM(BlueprintType)
enum class EInventoryPickupResult : uint8
{
	Success,        // 다 먹음
	Partial,        // 일부만 먹음 (인벤토리 꽉 참)
	Failed_Full     // 하나도 못 먹음 (인벤토리 꽉 참)
};

enum class EWeaponType : uint8;
// ConsumableInventory
USTRUCT(BlueprintType)
struct FConsumableSlotData
{
	GENERATED_BODY()

	UPROPERTY()
	class UBaseInstance* itemInstance = nullptr;

	// UPROPERTY()
	// int32 currentQuantity = 0;   // 슬롯 안에 들어있는 실제 수량
	//
	// 아이템 들어온 시간
	UPROPERTY()
	uint64 timeStamp = 0; 

	bool IsEmpty() const
	{
		if (UAmmoInstance* ammoInstance = Cast<UAmmoInstance>(itemInstance))
		{
			return false;
		}
		return itemInstance == nullptr;
	}
	void Clear()
	{
		itemInstance = nullptr;
		timeStamp = 0;
	}
	
	//템플릿 함수
	template <typename T>
	T* GetInstance() const
	{
		return Cast<T>(itemInstance);
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZOMBIEGROUND_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	
	
	FActorComponentTickFunction* ThisTickFunction) override;
	
	
public:
	const int32 MaxItemSlots = 10;
	
public:
	
	
	UPROPERTY()
	class AHumanCharacter* ownerCharacter;

	/** 주무기 슬롯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWeaponInstance* primaryWeaponSlot;

	/** 보조무기 슬롯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWeaponInstance* secondaryWeaponSlot;
	
	/** 근접무기 슬롯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWeaponInstance* meleeWeaponSlot;

	/** 아이템 슬롯 8개 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FConsumableSlotData> consumableSlot;
	
	
	/** 현재 장착 무기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseWeaponActor* currentWeaponActor;
	
	// UI에서 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	
public:
	UFUNCTION()
	void PickupItem(class ABasePickup* pickup);
	
	UFUNCTION()
	void AddPrimaryToSlot(class ABaseWeaponPickup* weaponPickup);	
	
	UFUNCTION()
	void AddSecondaryToSlot(class ABaseWeaponPickup* weaponPickup);
	
	UFUNCTION()
	void AddMeleeToSlot(class ABaseWeaponPickup* weaponPickup);
	
	UFUNCTION()
	bool AddConsumableToSlot(ABasePickup* pickup);
	
	UFUNCTION()
	void DropItemFromSlot(class UBaseInstance* itemInstance);
	
	UFUNCTION()
	void SortInventory();
	
	UFUNCTION()
	void EquipPrimaryWeapon();
	UFUNCTION()
	void EquipSecondaryWeapon();
	UFUNCTION()
	void EquipMeleeWeapon();
	

	
	// 탄약(TargetItemData)을 AmountToConsume만큼 찾아서 제거하고, 실제로 제거한 양을 반환
	int32 ConsumeItem(EWeaponType weaponType, int32 amountToConsume);
    
	// 현재 인벤토리에 해당 아이템이 총 몇 개 있는지 확인 (UI 표시용 등)
	UFUNCTION()
	int32 GetItemQuantity(class UBaseDataAsset* targetItemData);	
	
	// [추가] UI 슬롯에서 호출할 아이템 버리기 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropConsumableByIndex(int32 SlotIndex);

	// [추가] 슬롯 데이터 접근용 (UI에서 사용)
	const TArray<FConsumableSlotData>& GetConsumableSlots() const { return consumableSlot; }
	



};
