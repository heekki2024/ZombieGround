// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "Character/Human/HumanCharacter.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Instance/Weapon/WeaponInstance.h"
#include "Item/Pickup/BasePickup.h"
#include "Item/Pickup/Weapon/BaseWeaponPickup.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
	primaryWeaponSlot = nullptr;
	secondaryWeaponSlot = nullptr;
	meleeWeaponSlot = nullptr;
	currentWeaponActor = nullptr;

	
	itemSlots.SetNum(MaxAmmoSlots); // 초기 8개 nullptr
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AHumanCharacter>(GetOwner());

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::PickupItem(class ABasePickup* pickup)
{
	if (ABaseWeaponPickup* weaponPickup = Cast<ABaseWeaponPickup>(pickup))
	{
		if (weaponPickup->weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Primary)
		{
			AddPrimaryToSlot(weaponPickup);
		}else if (weaponPickup->weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Secondary)
		{
			AddSecondaryToSlot(weaponPickup);
		}else if (weaponPickup->weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Melee)
		{
			AddMeleeToSlot(weaponPickup);
		}
	}
	else
	{
		
	}
}

void UInventoryComponent::AddPrimaryToSlot(class ABaseWeaponPickup* weaponPickup)
{
	if (!IsValid(primaryWeaponSlot))
	{
		// 슬롯이 비어있거나 GC로 삭제됨
		primaryWeaponSlot = weaponPickup->weaponInstance;
	}
	else
	{
		DropWeaponFromSlot(primaryWeaponSlot);
		// 정상적으로 무기 존재
		primaryWeaponSlot = weaponPickup->weaponInstance;
	}
}

void UInventoryComponent::AddSecondaryToSlot(class ABaseWeaponPickup* weaponPickup)
{
	if (!IsValid(secondaryWeaponSlot))
	{
		// 슬롯이 비어있거나 GC로 삭제됨
		secondaryWeaponSlot = weaponPickup->weaponInstance;
	}
	else
	{
		DropWeaponFromSlot(secondaryWeaponSlot);
		// 정상적으로 무기 존재
		secondaryWeaponSlot = weaponPickup->weaponInstance;
	}
}

void UInventoryComponent::AddMeleeToSlot(class ABaseWeaponPickup* weaponPickup)
{
}

void UInventoryComponent::DropWeaponFromSlot(class UWeaponInstance* weaponInstance)
{
	// 2. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 3. 스폰 위치/회전은 대충 캐릭터 위치 기준으로
	FVector SpawnLocation = GetOwner()->GetActorLocation();
	FRotator SpawnRotation = GetOwner()->GetActorRotation();
	
	
	
	// 4. pickup 스폰
	ABaseWeaponPickup* newPickup = GetWorld()->SpawnActor<ABaseWeaponPickup>(
		weaponInstance->defaultWeaponData->weaponPickupClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	
	newPickup->LoadWeaponInstance(weaponInstance);
	
	primaryWeaponSlot = nullptr;
}


// bool UInventoryComponent::PickUpAmmo(ABaseItem* NewAmmo)
// {
// 	if (!NewAmmo) return false;
//
// 	for (int32 i = 0; i < MaxAmmoSlots; ++i)
// 	{
// 		if (!AmmoSlots[i])
// 		{
// 			AmmoSlots[i] = NewAmmo;
// 			return true;
// 		}
// 	}
//
// 	return false; // 빈 슬롯 없음
// }


