// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "Human/HumanCharacter.h"
#include "Item/Weapon/BaseWeapon.h"
#include "Pickup/BasePickup.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
	primaryWeapon = nullptr;
	secondaryWeapon = nullptr;
	currentWeapon = nullptr;

	AmmoSlots.SetNum(MaxAmmoSlots); // 초기 8개 nullptr
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

bool UInventoryComponent::PickUpPrimaryWeapon(ABaseWeapon* NewWeapon)
{
	if (!NewWeapon) return false;

	primaryWeapon = NewWeapon;
	
	PickUpPrimaryWeapon(primaryWeapon);
	return true;
}

bool UInventoryComponent::PickUpSecondaryWeapon(ABaseWeapon* NewWeapon)
{
	if (!NewWeapon) return false;

	secondaryWeapon = NewWeapon;
	EquipSecondaryWeapon();
	return true;
}

bool UInventoryComponent::PickUpAmmo(ABaseItem* NewAmmo)
{
	if (!NewAmmo) return false;

	for (int32 i = 0; i < MaxAmmoSlots; ++i)
	{
		if (!AmmoSlots[i])
		{
			AmmoSlots[i] = NewAmmo;
			return true;
		}
	}

	return false; // 빈 슬롯 없음
}

void UInventoryComponent::EquipPrimaryWeapon()
{
	if (primaryWeapon)
	{
		currentWeapon = primaryWeapon;
	}
}

void UInventoryComponent::EquipSecondaryWeapon()
{
	if (secondaryWeapon)
	{
		currentWeapon = secondaryWeapon;
	}
}

void UInventoryComponent::UnequipCurrentWeapon()
{
	if (!IsValid(currentWeapon) || !OwnerCharacter) 
		return;

	// 어떤 슬롯인지 판단
	if (currentWeapon == primaryWeapon)
	{
		SpawnPickup(primaryWeapon);
		primaryWeapon = nullptr;
	}
	else if (currentWeapon == secondaryWeapon)
	{
		SpawnPickup(secondaryWeapon);
		secondaryWeapon = nullptr;
	}

	currentWeapon = nullptr;  
}

void UInventoryComponent::SpawnPickup(class ABaseWeapon* Weapon)
{
	if (!IsValid(Weapon) || !OwnerCharacter)
		return;

	TSubclassOf<ABasePickup> PickupClass = Weapon->pickupClass;
	if (!PickupClass)
		return;

	FRotator ControlRot = OwnerCharacter->GetControlRotation();
	FVector Forward = ControlRot.Vector();
	FVector SpawnLoc = OwnerCharacter->GetActorLocation() + Forward * 50.f;

	Weapon->Destroy();

	FActorSpawnParameters Params;
	Params.Owner = OwnerCharacter;
	Params.Instigator = OwnerCharacter;

	ABasePickup* Pickup = OwnerCharacter->GetWorld()->SpawnActor<ABasePickup>(
		PickupClass,
		SpawnLoc,
		ControlRot,
		Params
	);

	if (Pickup)
	{
		if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Pickup->GetRootComponent()))
		{
			if (Root->IsSimulatingPhysics())
				Root->AddImpulse(Forward * 600.f);
		}
	}
}

void UInventoryComponent::SpawnWeapon(TSubclassOf<ABaseWeapon> WeaponToSpawn)
{
	if (!OwnerCharacter || !WeaponToSpawn)
		return;
	
	// 1) 현재 무기 드롭
	if (currentWeapon)
	{
		UnequipCurrentWeapon();  // 내부적으로 SpawnPickup 실행
	}
	// 2) 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 3) 스폰 위치/회전
	FVector SpawnLocation = OwnerCharacter->GetActorLocation();
	FRotator SpawnRotation = OwnerCharacter->GetActorRotation();

	// 4) 무기 스폰
	ABaseWeapon* NewWeapon = OwnerCharacter->GetWorld()->SpawnActor<ABaseWeapon>(
		WeaponToSpawn,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	
	// 5) 새 무기 장착
	currentWeapon = NewWeapon;

	// 6) Character Mesh에 Attach
	NewWeapon->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		NewWeapon->weaponDetails.socketName
	);

	// 7) 하이라이트된 Pickup 제거(플레이어가 줍는 상황)
	if (OwnerCharacter->HighlightedPickup)
	{
		OwnerCharacter->HighlightedPickup->Destroy();
		OwnerCharacter->HighlightedPickup = nullptr;
	}
}

