// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


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

	/** 총알 슬롯 8개 */
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
	void DropWeaponFromSlot(class UWeaponInstance* weaponInstance);
	
	UFUNCTION()
	void EquipPrimaryWeapon();
	UFUNCTION()
	void EquipSecondaryWeapon();
	UFUNCTION()
	void EquipMeleeWeapon();
	
	

	
private:
	const int32 MaxAmmoSlots = 8;
};
