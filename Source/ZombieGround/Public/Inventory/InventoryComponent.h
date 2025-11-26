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
	class ABaseWeapon* primaryWeapon;

	/** 보조무기 슬롯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseWeapon* secondaryWeapon;

	/** 총알 슬롯 8개 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class ABaseItem*> AmmoSlots;

	/** 현재 장착 무기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseWeapon* currentWeapon;

	/** 주무기 습득 */
	bool PickUpPrimaryWeapon(ABaseWeapon* NewWeapon);

	/** 보조무기 습득 */
	bool PickUpSecondaryWeapon(ABaseWeapon* NewWeapon);

	/** 총알 습득 (빈 슬롯에만 추가) */
	bool PickUpAmmo(ABaseItem* NewAmmo);

	/** 무기 장착 */
	UFUNCTION()
	void EquipPrimaryWeapon();
	UFUNCTION()
	void EquipSecondaryWeapon();

	UFUNCTION()
	void UnequipCurrentWeapon();
	
	UFUNCTION()
	void SpawnPickup(class ABaseWeapon* Weapon);

	UFUNCTION()
	void SpawnWeapon(TSubclassOf<ABaseWeapon> WeaponToSpawn);
	
private:
	const int32 MaxAmmoSlots = 8;
};
