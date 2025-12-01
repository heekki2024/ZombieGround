// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "Character/Human/HumanCharacter.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
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
			weaponPickup->Destroy();
			EquipPrimaryWeapon();

		}else if (weaponPickup->weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Secondary)
		{
			AddSecondaryToSlot(weaponPickup);
			weaponPickup->Destroy();
			EquipSecondaryWeapon();

		}else if (weaponPickup->weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Melee)
		{
			AddMeleeToSlot(weaponPickup);
			weaponPickup->Destroy();
			EquipPrimaryWeapon();

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
	
	// [수정 포인트 1] 바라보는 방향(Aim Direction) 가져오기
	// GetActorForwardVector() 대신 GetControlRotation()을 사용합니다.
	// GetControlRotation()은 마우스/스틱으로 조종하는 카메라의 회전값(Pitch, Yaw)을 포함합니다.
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FVector AimDirection = ControlRotation.Vector(); // 회전값을 방향 벡터로 변환

	// [수정 포인트 2] 스폰 위치 계산
	// 바라보는 방향으로 100만큼 떨어진 곳에서 스폰
	FVector SpawnLocation = OwnerCharacter->GetActorLocation() + (AimDirection * 50.f); 
    
	// [옵션] 스폰 회전값도 시선과 일치시킬지, 아니면 랜덤하게 할지 결정
	// 무기가 날아가는 방향으로 머리를 돌리려면 ControlRotation을 넣으세요.
	FRotator SpawnRotation = ControlRotation; 
	
	ABaseWeaponPickup* newPickup = GetWorld()->SpawnActor<ABaseWeaponPickup>(
	weaponInstance->defaultWeaponData->pickupClass,
	SpawnLocation,
	SpawnRotation,
	SpawnParams
);
	
	newPickup->LoadWeaponInstance(weaponInstance);
	
	// 5. 물리 임펄스 적용
	if (newPickup) // Pickup이 잘 생성되었는지 확인
	{
		UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(newPickup->GetRootComponent());
		if (RootComp && RootComp->IsSimulatingPhysics())
		{
			// [수정 포인트 3] 바라보는 방향(AimDirection)으로 힘을 가함
			// 400.f는 좀 약할 수 있으니 테스트해보며 조절하세요 (예: 1000.f)
			FVector ThrowForce = AimDirection * 1000.f; 
			RootComp->AddImpulse(ThrowForce, NAME_None, true);
		}
	}
	
	if (weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Primary)
	{
		primaryWeaponSlot = nullptr;
	}else if (weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Secondary)
	{
		secondaryWeaponSlot = nullptr;
	}else if (weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Melee)
	{
		meleeWeaponSlot = nullptr;
	}
	
	
}

void UInventoryComponent::EquipPrimaryWeapon()
{
	if (!IsValid(primaryWeaponSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("DropWeapon: primaryWeapon is invalid"));
		return;
	}
	if (IsValid(currentWeaponActor) && currentWeaponActor->weaponInstance == primaryWeaponSlot) return;


	// 1️⃣ 기존 장착 무기 제거
	if (IsValid(currentWeaponActor))
	{
		currentWeaponActor->Destroy();
		currentWeaponActor = nullptr;
	}
	
	// 2. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 3. 스폰 위치/회전은 대충 캐릭터 위치 기준으로
	FVector SpawnLocation = OwnerCharacter->GetActorLocation();
	FRotator SpawnRotation = OwnerCharacter->GetActorRotation();

	// 4. 액터 스폰
	ABaseWeaponActor* newCurrentWeapon = GetWorld()->SpawnActor<ABaseWeaponActor>(
		primaryWeaponSlot->defaultWeaponData->actorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	
	newCurrentWeapon->LoadWeaponInstance(primaryWeaponSlot);
	
	// 6. 무기 저장
	currentWeaponActor = newCurrentWeapon;
	// currentWeaponNameEnum = currentWeapon->weaponDetails.WeaponName;
	
	// 7. Attach (부착)
	currentWeaponActor->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		primaryWeaponSlot->defaultWeaponData->rHandRifleSocketName
	);
}

void UInventoryComponent::EquipSecondaryWeapon()
{
	if (!IsValid(secondaryWeaponSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("DropWeapon: primaryWeapon is invalid"));
		return;
	}
	if (IsValid(currentWeaponActor) && currentWeaponActor->weaponInstance == secondaryWeaponSlot) return;


	// 1️⃣ 기존 장착 무기 제거
	if (IsValid(currentWeaponActor))
	{
		currentWeaponActor->Destroy();
		currentWeaponActor = nullptr;
	}
	
	// 2. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 3. 스폰 위치/회전은 대충 캐릭터 위치 기준으로
	FVector SpawnLocation = OwnerCharacter->GetActorLocation();
	FRotator SpawnRotation = OwnerCharacter->GetActorRotation();

	// 4. 액터 스폰
	ABaseWeaponActor* newCurrentWeapon = GetWorld()->SpawnActor<ABaseWeaponActor>(
		secondaryWeaponSlot->defaultWeaponData->actorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	
	newCurrentWeapon->LoadWeaponInstance(secondaryWeaponSlot);
	
	// 6. 무기 저장
	currentWeaponActor = newCurrentWeapon;
	// currentWeaponNameEnum = currentWeapon->weaponDetails.WeaponName;
	
	// 7. Attach (부착)
	currentWeaponActor->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		secondaryWeaponSlot->defaultWeaponData->rHandPistolSocketName
	);
}

void UInventoryComponent::EquipMeleeWeapon()
{
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


