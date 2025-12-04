// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "Character/Human/HumanCharacter.h"
#include "Item/DataAsset/Ammo/AmmoDataAsset.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
#include "Item/Instance/Ammo/AmmoInstance.h"
#include "Item/Instance/Weapon/WeaponInstance.h"
#include "Item/Pickup/BasePickup.h"
#include "Item/Pickup/Ammo/AmmoPickup.h"
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

	consumableItemSlot.SetNum(MaxItemSlots); // 초기 8개 nullptr

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
		AddItemToSlot(pickup);
		pickup->Destroy();
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

bool UInventoryComponent::AddItemToSlot(ABasePickup* pickup)
{
	int32 maxQuantity;
	int32 remaining = 1;

	AAmmoPickup* ammoPickup = Cast<AAmmoPickup>(pickup);
	if (ammoPickup && ammoPickup->ammoInstance)
	{
		UAmmoInstance* ammoInstance = ammoPickup->ammoInstance;
		if (!ammoInstance) return false;
	
		maxQuantity = ammoInstance->defaultAmmoData->maxQuantity;
	
		//한 slot에 60발이 들어가는 소총탄이 80이 들어왔을경우 remaining변수에 넣고 빼나가며 계산
		remaining = ammoInstance->currentQuantity;
	
		// 동일 아이템이 있는 슬롯이 최대개수가 아닐경우 그 슬롯부터 채운다.
		for (FConsumableItemSlot& slot : consumableItemSlot)
		{
			if (slot.itemInstance == ammoInstance && slot.currentQuantity < maxQuantity)
			{
			
				//remaining이 10개, 60개가 들어가는 슬롯에서 currentQuantity가 28이면 space는 32,
				//space 가 32, remaining이 10개. ToAdd는 10개
				// 기존 슬롯에 10발을 더한다. remaining 에 10발을 뺀다. remaining이 0발 이하면 sorting
			
				//remaining이 40개, 60개가 들어가는 슬롯에서 currentQuantity가 28이면 space는 32,
				//space 가 32, remaining이 40개. ToAdd는 32개
				// 기존 슬롯에 32발을 더한다. remaining(40)에 ToAdd(32)발을 뺀다 8개. remaining이 1개 이상이면 
				//for문을 탈출하고 아래 for문에 가서 완전히 빈칸인 곳에 남은 총알을 채운다.
				const int32 Space = maxQuantity - slot.currentQuantity;
				const int32 ToAdd = FMath::Min(Space, remaining);
			
			
				slot.currentQuantity += ToAdd;
				remaining -= ToAdd;
				if (remaining <= 0)
				{
					SortInventory();
					
					if(OwnerCharacter)
					{
						OwnerCharacter->BroadcastInventoryAmmoUpdate();
					}
					
					return true;
				}
			}
		}
	
		// 2) 남은 수량을 빈 슬롯에 배치
		for (FConsumableItemSlot& slot : consumableItemSlot)
		{
			if (slot.IsEmpty())
			{
				slot.itemInstance = ammoInstance;

				const int32 ToAdd = FMath::Min(maxQuantity, remaining);
				slot.currentQuantity = ToAdd;

				// ──────────────── 여기서 Timestamp 설정 ────────────────
				slot.timeStamp = GFrameCounter; 
				// or GameInstance에서 TickCount 가져오기
				// or FDateTime::UtcNow().GetTicks()

				remaining -= ToAdd;

				if (remaining <= 0)
				{
					SortInventory();
					
					if(OwnerCharacter)
					{
						OwnerCharacter->BroadcastInventoryAmmoUpdate();
					}
					
					return true;
				}
			}
		}
	}
	// UBaseInstance* itemInstance = pickup->itemInstance;

	// 3) 여기까지 왔는데 Remaining > 0이면 인벤토리 꽉 찬 상황(false 호출)
	if(OwnerCharacter)
	{
		OwnerCharacter->BroadcastInventoryAmmoUpdate();
	}
	return remaining <= 0;
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



void UInventoryComponent::SortInventory()
{
	consumableItemSlot.Sort([](const FConsumableItemSlot& A, const FConsumableItemSlot& B)
	{
		UBaseDataAsset* DataA = A.itemInstance ? A.itemInstance->GetItemData() : nullptr;
		UBaseDataAsset* DataB = B.itemInstance ? B.itemInstance->GetItemData() : nullptr;

		if (!DataA && !DataB) return false;
		if (!DataA) return false;
		if (!DataB) return true;

		const int32 PriorityA = DataA->GetSortPriority();
		const int32 PriorityB = DataB->GetSortPriority();

		if (PriorityA != PriorityB) return PriorityA < PriorityB;

		return A.timeStamp < B.timeStamp;
	});
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

	if(OwnerCharacter)
	{
		OwnerCharacter->BroadcastCurrentAmmoUpdate();
		OwnerCharacter->BroadcastInventoryAmmoUpdate();
	}
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

	if(OwnerCharacter)
	{
		OwnerCharacter->BroadcastCurrentAmmoUpdate();
		OwnerCharacter->BroadcastInventoryAmmoUpdate();	}
}

void UInventoryComponent::EquipMeleeWeapon()
{
}

int32 UInventoryComponent::ConsumeItem(EWeaponType weaponType, int32 amountToConsume)
{
	if (amountToConsume <= 0) return 0;
	
	int32 remainingNeeded = amountToConsume;
	
	//인벤토리 전체 순회 (여러 슬롯에 나뉘어 있을 수 있음)
	bool bNeedSort = false; // [핵심] 정렬이 필요한지 체크하는 깃발
	
	for (FConsumableItemSlot& slot : consumableItemSlot)
	{
		//유효성 검사
		if (slot.IsEmpty()) continue;
		
		//AmmoDataAsset에 지정된 총타입과 현재들고 있는 총의 타입이 일치하는지 확인
		// if (Cast<UAmmoDataAsset>(slot.itemInstance->defaultItemData)->weaponType == weaponType)
		if (UAmmoInstance* ammoInstance = Cast<UAmmoInstance>(slot.itemInstance))
		{
			// 2. 캐스팅이 성공(nullptr이 아님)했을 때만 데이터에 접근합니다.
			if (ammoInstance->defaultAmmoData && ammoInstance->defaultAmmoData->ammoType == weaponType)
			{
				//차감 계산
				int32 TakeFromSlot = FMath::Min(slot.currentQuantity, remainingNeeded);
			
				//수량 적용
				slot.currentQuantity -= TakeFromSlot;
				ammoInstance->currentQuantity = slot.currentQuantity;
			
				remainingNeeded -= TakeFromSlot;
			
				//슬롯이 비었을 때 처리
				if (slot.currentQuantity <= 0)
				{
					slot.Clear();
				}
			
				bNeedSort = true;
			}
			if (remainingNeeded <= 0)
			{
				break;
			}
		}
	}
	//반복문이 완전히 끝난 후 정렬 실행
	if (bNeedSort)
	{
		SortInventory();
	}
	
	return amountToConsume - remainingNeeded;
}


int32 UInventoryComponent::GetItemQuantity(class UBaseDataAsset* targetItemData)
{
	int32 totalCount = 0;
	if (UWeaponDataAsset* weaponDA = Cast<UWeaponDataAsset>(targetItemData))
	{
		for (const FConsumableItemSlot& slot : consumableItemSlot)
		{
			if (!slot.IsEmpty() && Cast<UAmmoDataAsset>(Cast<UAmmoInstance>(slot.itemInstance)->GetItemData())->ammoType == weaponDA->weaponType)
			{
				totalCount += slot.currentQuantity;
			}
		}
	}
	

	return totalCount;
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


