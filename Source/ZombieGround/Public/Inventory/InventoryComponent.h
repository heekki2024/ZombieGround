// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


enum class EWeaponType : uint8;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY()
	class UBaseInstance* itemInstance = nullptr;

	UPROPERTY()
	int32 currentQuantity = 0;   // 슬롯 안에 들어있는 실제 수량
	
	// 아이템 들어온 시간
	UPROPERTY()
	uint64 timeStamp = 0; 

	bool IsEmpty() const { return itemInstance == nullptr; }
	void Clear()
	{
		itemInstance = nullptr;
		currentQuantity = 0;
		timeStamp = 0;
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
	
	UPROPERTY()
	class AHumanCharacter* OwnerCharacter;

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
	TArray<class UBaseInstance*> itemSlots;

	/** 현재 장착 무기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseWeaponActor* currentWeaponActor;
	
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
	bool AddItemToSlot(ABasePickup* pickup);
	
	UFUNCTION()
	void DropWeaponFromSlot(class UWeaponInstance* weaponInstance);
	
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
	int32 GetItemQuantity(class UBaseItemDataAsset* TargetItemData);	
public:
	const int32 MaxItemSlots = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> inventorySlots;

};
