// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "Character/Human/HumanCharacter.h"
#include "Item/DataAsset/Ammo/AmmoDataAsset.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Flashlight/Flashlight.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
#include "Item/Instance/Ammo/AmmoInstance.h"
#include "Item/Instance/Weapon/BaseWeaponInstance.h"
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

	consumableSlot.SetNum(MaxItemSlots); // 초기 8개 nullptr

}




// Called when the game starts
void UInventoryComponent::BeginPlay()
{   
	Super::BeginPlay();
	ownerCharacter = Cast<AHumanCharacter>(GetOwner());

	// ...

	// 초기 보조 무기 지급
	if (StartingSecondaryWeaponDataAsset)
	{
		UBaseWeaponInstance* NewWeaponInstance = NewObject<UBaseWeaponInstance>(this);
		NewWeaponInstance->InitInstance(StartingSecondaryWeaponDataAsset);
		
		// [추가] 기본 지급 무기에만 숨겨진 탄약 100발 부여
		NewWeaponInstance->InternalReserveAmmo = 100;
		
		secondaryWeaponSlot = NewWeaponInstance;
	}
	EquipSecondaryWeapon();
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
		if (weaponPickup->instance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary)
		{
			AddPrimaryToSlot(weaponPickup);
			weaponPickup->Destroy();
			EquipPrimaryWeapon();
		}else if (weaponPickup->instance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
		{
			AddSecondaryToSlot(weaponPickup);
			weaponPickup->Destroy();
			EquipSecondaryWeapon();

		}else if (weaponPickup->instance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Melee)
		{
			AddMeleeToSlot(weaponPickup);
			weaponPickup->Destroy();
			EquipPrimaryWeapon();
		}
	}
	else
	{
		bool bShouldDestoryPickup = AddConsumableToSlot(pickup);
		if (bShouldDestoryPickup)
		{
			pickup->Destroy();
		}
	}
}

void UInventoryComponent::AddPrimaryToSlot(class ABaseWeaponPickup* weaponPickup)
{
	if (!IsValid(primaryWeaponSlot))
	{
		// 슬롯이 비어있거나 GC로 삭제됨
		primaryWeaponSlot = weaponPickup->GetInstance<UBaseWeaponInstance>();
	}
	else
	{
		DropItemFromSlot(primaryWeaponSlot);
		
		
		// 정상적으로 무기 존재
		primaryWeaponSlot = weaponPickup->GetInstance<UBaseWeaponInstance>();
	}
}

void UInventoryComponent::AddSecondaryToSlot(class ABaseWeaponPickup* weaponPickup)
{
	if (!IsValid(secondaryWeaponSlot))
	{
		// 슬롯이 비어있거나 GC로 삭제됨
		secondaryWeaponSlot = weaponPickup->GetInstance<UBaseWeaponInstance>();
	}
	else
	{
		DropItemFromSlot(secondaryWeaponSlot);
		// 정상적으로 무기 존재
		secondaryWeaponSlot = weaponPickup->GetInstance<UBaseWeaponInstance>();
	}
}

void UInventoryComponent::AddMeleeToSlot(class ABaseWeaponPickup* weaponPickup)
{
}


bool UInventoryComponent::AddConsumableToSlot(ABasePickup* pickup)
{
	// // 1. 유효성 검사 (특정 타입이 아닌 Base 클래스로 접근)
	// if (!pickup) return false;
	
	UBaseInstance* incomingInstance = pickup->GetItemInstance();
	if (!incomingInstance || !incomingInstance->GetItemData()) return false;
	
	//아이템 정보 추출
	const int32 MaxQuantity = incomingInstance->GetItemData()->maxQuantity;
	
	// [중요] 시작 전 원래 수량 저장
	const int32 StartQuantity = incomingInstance->currentQuantity;

	// ... 기존 로직 (MaxQuantity, remaining 초기화 등) ...
	int32 remaining = StartQuantity;
	
	//아이템이 겹칠 수 있는 타입인지 확인(최대 수량이 1보다 크면 겹치기 가능
	const bool bIsStackable = (MaxQuantity > 1);
	
	//1단계 : 이미 있는 슬롯에 합치기 (Stackable 아이템만 해당)
	if (bIsStackable)
	{
		for (FConsumableSlotData& slot : consumableSlot)
		{
			//슬롯이 비어있지 않고 같은 종류의 아이템이며 꽉 차지 않았을 때 
			if (!slot.IsEmpty() && slot.itemInstance->GetItemData<UAmmoDataAsset>() && slot.itemInstance->currentQuantity < MaxQuantity)
			{
				const int32 Space = MaxQuantity - slot.itemInstance->currentQuantity;
				const int32 ToAdd = FMath::Min(Space, remaining);
				
				slot.itemInstance->currentQuantity += ToAdd;
				remaining -= ToAdd;
				
				//다 넣었으면 종료
				if (remaining <= 0)
				{
					incomingInstance->currentQuantity = 0; //원본 비우기
					SortInventory();
					// if(OwnerCharacter) OwnerCharacter->BroadcastInventoryUpdate();
					if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
					
					if (ownerCharacter) ownerCharacter->BroadcastInventoryAmmoUpdate();
					return true;
				}
			}
		}
	}
	
	//2단계 : 빈슬롯 채우기
	for (FConsumableSlotData& slot : consumableSlot)
	{
		if (slot.IsEmpty())
		{
			//이 슬롯에 넣을 수 있는 최대량 계산
			//stackable이 아니면 MaxStackSize가 1일테니 한개만 들어감
			const int32 ToAdd = FMath::Min(MaxQuantity, remaining);
			
			//Cast A : 남은 걸 전부 이 슬롯에 넣을 수 있는 경우 -> 원본 객체를 그대로 이동
			if (remaining <= ToAdd)
			{
				UBaseInstance* newInstance = DuplicateObject<UBaseInstance>(incomingInstance, this);

				slot.itemInstance = newInstance;
				slot.itemInstance->currentQuantity = ToAdd;
				remaining = 0;
			}
			else
			{
				UBaseInstance* newInstance = DuplicateObject<UBaseInstance>(incomingInstance, this);
				
				slot.itemInstance = newInstance;
				slot.itemInstance->currentQuantity = ToAdd;
				
				remaining -= ToAdd;
			}
			
			//타임 스탬프 갱신
			slot.timeStamp = GFrameCounter;
			
			//다 넣었으면 종료
			if (remaining <= 0)
			{
				incomingInstance->currentQuantity = 0; //원본 비우기
				SortInventory();
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				
				if (ownerCharacter) ownerCharacter->BroadcastInventoryAmmoUpdate();
				return true;
			}
		}
	}
	
	// ----------------------------------------------------------------
	// 3단계: 마무리 (일부만 들어갔거나, 아예 못 들어간 경우)
	// ----------------------------------------------------------------
    
	// 원본(Pickup)의 남은 수량 갱신 (예: 50개 중 20개만 먹고 30개 남음)
	incomingInstance->currentQuantity = remaining;

	if(ownerCharacter)
	{
		ownerCharacter->BroadcastInventoryAmmoUpdate();
	}
	

	
	// 결과 판단 로직
	EInventoryPickupResult Result;

	if (remaining <= 0)
	{
		Result = EInventoryPickupResult::Success; // 다 먹음
	}
	else if (remaining == StartQuantity)
	{
		Result = EInventoryPickupResult::Failed_Full; // 하나도 안 줄어듦 -> 꽉 차서 못 먹음
		// UE_LOG(LogTemp, Warning, TEXT("꽉 차서 못 먹음"));

	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("일부만 먹고 꽉 참"));
		Result = EInventoryPickupResult::Partial; // 줄어들긴 함 -> 일부만 먹고 꽉 참
	}

	// [추가 3] 부분적으로 먹었을 때도 UI 갱신 필요 (StartQuantity와 다르면 무언가 먹은 것임)
	if (Result == EInventoryPickupResult::Partial)
	{
		if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
	}
	
	// 결과 방송 (UI에서 받아서 처리)
	// Success가 아닐 때만 알림을 띄우고 싶다면 조건문 추가
	// if (Result != EInventoryPickupResult::Success)
	// {
	// 	if (OnPickupResult.IsBound())
	// 	{
	// 		OnPickupResult.Broadcast(Result);
	// 	}
	// }
	
	// 남은 게 없으면(0) true 반환 -> Pickup Actor 파괴
	// 남은 게 있으면(>0) false 반환 -> Pickup Actor 유지 (수량만 줄어듦)
	return remaining <= 0;
}


void UInventoryComponent::DropItemFromSlot(class UBaseInstance* itemInstance)
{
	// 1️⃣ 기존 장착 무기 제거
	if (IsValid(currentWeaponActor))
	{
		currentWeaponActor->Destroy();
		currentWeaponActor = nullptr;
	}
	
	// 2. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ownerCharacter;
	SpawnParams.Instigator = ownerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// [수정 포인트 1] 바라보는 방향(Aim Direction) 가져오기
	// GetActorForwardVector() 대신 GetControlRotation()을 사용합니다.
	// GetControlRotation()은 마우스/스틱으로 조종하는 카메라의 회전값(Pitch, Yaw)을 포함합니다.
	FRotator ControlRotation = ownerCharacter->GetControlRotation();
	FVector AimDirection = ControlRotation.Vector(); // 회전값을 방향 벡터로 변환

	// [수정 포인트 2] 스폰 위치 계산
	// 바라보는 방향으로 100만큼 떨어진 곳에서 스폰
	FVector SpawnLocation = ownerCharacter->GetActorLocation() + (AimDirection * 50.f); 
    
	// [옵션] 스폰 회전값도 시선과 일치시킬지, 아니면 랜덤하게 할지 결정
	// 무기가 날아가는 방향으로 머리를 돌리려면 ControlRotation을 넣으세요.
	FRotator SpawnRotation = ControlRotation; 
	
	if (UBaseWeaponInstance* weaponInstance = Cast<UBaseWeaponInstance>(itemInstance))
	{
		ABaseWeaponPickup* newPickup = GetWorld()->SpawnActor<ABaseWeaponPickup>(
		weaponInstance->GetItemData<UWeaponDataAsset>()->pickupClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
		);
		
	
		// 5. 물리 임펄스 적용
		if (newPickup) // Pickup이 잘 생성되었는지 확인
		{
			newPickup->LoadWeaponInstance(weaponInstance);

			// Force direction: 캐릭터의 정면 방향
			FVector ThrowDirection = ownerCharacter->GetActorForwardVector();

			// 물리력 가하기
			UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(newPickup->GetRootComponent());
			if (RootComp && RootComp->IsSimulatingPhysics())
			{
				RootComp->AddImpulse(ThrowDirection * 300.f, NAME_None, true);
			}
		}
	
		if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary)
		{
			primaryWeaponSlot = nullptr;
		}else if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
		{
			secondaryWeaponSlot = nullptr;
		}else if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Melee)
		{
			meleeWeaponSlot = nullptr;
		}
	}else if (UAmmoInstance* ammoInstance = Cast<UAmmoInstance>(itemInstance))
	{
		AAmmoPickup* newPickup = GetWorld()->SpawnActor<AAmmoPickup>(
			ammoInstance->GetItemData<UAmmoDataAsset>()->GetPickupClass<AAmmoPickup>(),
			SpawnLocation,
			ControlRotation,
			SpawnParams
		);

		if (newPickup)
		{
			// 기존 인스턴스 복제 혹은 데이터 전달 로직 필요
			// 여기서는 기존 로직의 LoadWeaponInstance 패턴을 따른다고 가정합니다.
			newPickup->LoadAmmoInstance(ammoInstance); // << 픽업 클래스에 맞는 초기화 함수 호출 필요
                 
			// Force direction: 캐릭터의 정면 방향
			FVector ThrowDirection = ownerCharacter->GetActorForwardVector();

			// 물리력 가하기
			UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(newPickup->GetRootComponent());
			if (RootComp && RootComp->IsSimulatingPhysics())
			{
				RootComp->AddImpulse(ThrowDirection * 300.f, NAME_None, true);
			}
		}
	}
}



void UInventoryComponent::SortInventory()
{
	consumableSlot.Sort([](const FConsumableSlotData& A, const FConsumableSlotData& B)
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
	//인벤토리에 주무기가 있는지 확인한다. 없으면 return;
	//현재 들고 있는 무기가 인벤토리의 주무기에 해당하면 return;
	//총기 swap, 스왑한 무기를 현재 들고 있는 무기로 설정
	if (primaryWeaponSlot == nullptr) return;
	if (IsValid(currentWeaponActor) && currentWeaponActor->weaponInstance == primaryWeaponSlot) return;	
	
	// 2. [중요] "나 이거 낄거야"라고 예약 (Pending)
	PendingWeaponInstance = primaryWeaponSlot;
	
	// 4. 교체 로직 실행
	SwapWeaponInternal();
	
	// if (!IsValid(primaryWeaponSlot))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("DropWeapon: primaryWeapon is invalid"));
	// 	return;
	// }
	// if (IsValid(currentWeaponActor) && currentWeaponActor->weaponInstance == primaryWeaponSlot) return;
	//
	// // 2. 스폰 파라미터 설정
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.Owner = ownerCharacter;
	// SpawnParams.Instigator = ownerCharacter;
	// SpawnParams.SpawnCollisionHandlingOverride = 
	// 	ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// // 3. 스폰 위치/회전은 대충 캐릭터 위치 기준으로
	// FVector SpawnLocation = ownerCharacter->GetActorLocation();
	// FRotator SpawnRotation = ownerCharacter->GetActorRotation();
	//
	// // 4. 액터 스폰
	// ABaseWeaponActor* newCurrentWeapon = GetWorld()->SpawnActor<ABaseWeaponActor>(
	// 	primaryWeaponSlot->GetItemData<UWeaponDataAsset>()->actorClass,
	// 	SpawnLocation,
	// 	SpawnRotation,
	// 	SpawnParams
	// );
	//
	// newCurrentWeapon->LoadWeaponInstance(primaryWeaponSlot);
	//
	//
	// // 6. 무기 저장
	// currentWeaponActor = newCurrentWeapon;
	// // currentWeaponNameEnum = currentWeapon->weaponDetails.WeaponName;
	//
	// // 7. Attach (부착)
	// currentWeaponActor->AttachToComponent(
	// 	ownerCharacter->GetMesh(),
	// 	FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	// 	primaryWeaponSlot->GetItemData<UWeaponDataAsset>()->rHandRifleSocketName
	// );
	//
	// if(ownerCharacter)
	// {
	// 	ownerCharacter->BroadcastCurrentAmmoUpdate();
	// 	ownerCharacter->BroadcastInventoryAmmoUpdate();
	// }
	//
	// currentWeaponActor->weaponInstance = primaryWeaponSlot;
}

void UInventoryComponent::EquipSecondaryWeapon()
{
	//인벤토리에 주무기가 있는지 확인한다. 없으면 return;
	//현재 들고 있는 무기가 인벤토리의 주무기에 해당하면 return;
	//총기 swap, 스왑한 무기를 현재 들고 있는 무기로 설정
	if (secondaryWeaponSlot == nullptr) return;
	if (IsValid(currentWeaponActor) && currentWeaponActor->weaponInstance == secondaryWeaponSlot) return;	
	
	// 2. [중요] "나 이거 낄거야"라고 예약 (Pending)
	PendingWeaponInstance = secondaryWeaponSlot;
	
	// 4. 교체 로직 실행
	SwapWeaponInternal();
	
	
	// if (!IsValid(secondaryWeaponSlot))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("DropWeapon: primaryWeapon is invalid"));
	// 	return;
	// }
	// if (IsValid(currentWeaponActor) && currentWeaponActor->weaponInstance == secondaryWeaponSlot) return;
	//
	//
	// // 1️⃣ 기존 장착 무기 제거
	// if (IsValid(currentWeaponActor))
	// {
	// 	currentWeaponActor->Destroy();
	// 	currentWeaponActor = nullptr;
	// }
	//
	// // 2. 스폰 파라미터 설정
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.Owner = ownerCharacter;
	// SpawnParams.Instigator = ownerCharacter;
	// SpawnParams.SpawnCollisionHandlingOverride = 
	// 	ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// // if (!IsValid(currentFlashlight))
	// // {
	// // 	currentFlashlight = GetWorld()->SpawnActor<AFlashlight>(flashlightClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnParams);
	// //
	// // 	if (currentFlashlight)
	// // 	{
	// // 		// Mesh의 소켓 이름(예: "Hand_R_Socket")에 부착
	// // 		currentFlashlight->AttachToComponent(ownerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_l_flashlight"));
	// // 	}
	// // }
	//
	// // 3. 스폰 위치/회전은 대충 캐릭터 위치 기준으로
	// FVector SpawnLocation = ownerCharacter->GetActorLocation();
	// FRotator SpawnRotation = ownerCharacter->GetActorRotation();
	//
	// // 4. 액터 스폰
	// ABasePistolActor* newCurrentWeapon = GetWorld()->SpawnActor<ABasePistolActor>(
	// 	secondaryWeaponSlot->GetItemData<UWeaponDataAsset>()->actorClass,
	// 	SpawnLocation,
	// 	SpawnRotation,
	// 	SpawnParams
	// );
	//
	// newCurrentWeapon->LoadWeaponInstance(secondaryWeaponSlot);
	//
	// // 6. 무기 저장
	// currentWeaponActor = newCurrentWeapon;
	// // currentWeaponNameEnum = currentWeapon->weaponDetails.WeaponName;
	//
	// // 7. Attach (부착)
	// currentWeaponActor->AttachToComponent(
	// 	ownerCharacter->GetMesh(),
	// 	FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	// 	secondaryWeaponSlot->GetItemData<UWeaponDataAsset>()->rHandPistolSocketName
	// );
	//
	// if(ownerCharacter)
	// {
	// 	ownerCharacter->BroadcastCurrentAmmoUpdate();
	// 	ownerCharacter->BroadcastInventoryAmmoUpdate();	
	// }
	// currentWeaponActor->weaponInstance = secondaryWeaponSlot;

}

void UInventoryComponent::EquipMeleeWeapon()
{
}

int32 UInventoryComponent::ConsumeItem(int32 amountToConsume)
{
	// weaponInstance->GetItemData<UWeaponDataAsset>()->weaponType,
	if (amountToConsume <= 0) return 0;
	
	int32 remainingNeeded = amountToConsume;
	
	//인벤토리 전체 순회 (여러 슬롯에 나뉘어 있을 수 있음)
	bool bNeedSort = false; // [핵심] 정렬이 필요한지 체크하는 깃발
	
	for (FConsumableSlotData& slot : consumableSlot)
	{
		//유효성 검사
		if (slot.IsEmpty()) continue;
		
		//AmmoDataAsset에 지정된 총타입과 현재들고 있는 총의 타입이 일치하는지 확인
		// if (Cast<UAmmoDataAsset>(slot.itemInstance->defaultItemData)->weaponType == weaponType)
		if (UAmmoInstance* ammoInstance = Cast<UAmmoInstance>(slot.itemInstance))
		{
			// 2. 캐스팅이 성공(nullptr이 아님)했을 때만 데이터에 접근합니다.
			if (ammoInstance->GetItemData<UAmmoDataAsset>() && ammoInstance->GetItemData<UAmmoDataAsset>()->ammoType == Cast<UWeaponDataAsset>(currentWeaponActor->weaponInstance->defaultItemData)->weaponType)
			{
				//차감 계산
				int32 TakeFromSlot = FMath::Min(slot.itemInstance->currentQuantity, remainingNeeded);
			
				//수량 적용
				slot.itemInstance->currentQuantity -= TakeFromSlot;
				ammoInstance->currentQuantity = slot.itemInstance->currentQuantity;
			
				remainingNeeded -= TakeFromSlot;
			
				//슬롯이 비었을 때 처리
				if (slot.itemInstance->currentQuantity <= 0)
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
	// 5. UI 업데이트 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
	
	return amountToConsume - remainingNeeded;
}


int32 UInventoryComponent::GetItemQuantity(class UBaseDataAsset* targetItemData)
{
	int32 totalCount = 0;
	if (UWeaponDataAsset* weaponDA = Cast<UWeaponDataAsset>(targetItemData))
	{
		for (const FConsumableSlotData& slot : consumableSlot)
		{
			if (!slot.IsEmpty() && Cast<UAmmoDataAsset>(Cast<UAmmoInstance>(slot.itemInstance)->GetItemData())->ammoType == weaponDA->weaponType)
			{
				totalCount += slot.itemInstance->currentQuantity;
			}
		}
	}
	

	return totalCount;
}

void UInventoryComponent::DropConsumableByIndex(int32 SlotIndex)
{
	if (!consumableSlot.IsValidIndex(SlotIndex)) return;

    FConsumableSlotData& slot = consumableSlot[SlotIndex];
    
    // 빈 슬롯이면 무시
    if (slot.IsEmpty()) return;
    
	if (UAmmoInstance* ammoInstance = slot.GetInstance<UAmmoInstance>())
	{
		if (!ammoInstance) return;

		// AAmmoPickup* ammoPickup = ;
		
	    // 1. 실제 pickup 드롭
		DropItemFromSlot(ammoInstance);

	    // 3. 인벤토리에서 제거
	    slot.Clear(); // 슬롯 비우기 (Instance = nullptr, Qty = 0)

	    // 4. 정렬
	    SortInventory();

	    // 5. UI 업데이트 알림
	    if (OnInventoryUpdated.IsBound())
	    {
	        OnInventoryUpdated.Broadcast();
	    }
	}
}

void UInventoryComponent::SwapWeaponInternal()
{
	if (IsValid(currentWeaponActor))
	{
		// A. 기존 무기가 있으면 "넣어라(StartUnequip)" 명령
		// 델리게이트 바인딩: 무기가 사라지면(Destroy 직전) 나한테 알려줘
		currentWeaponActor->OnUnequipFinished.AddDynamic(this, &UInventoryComponent::OnCurrentWeaponUnequipped);
        
		// 애니메이션 재생 시작 -> 끝나면 스스로 Destroy됨
		currentWeaponActor->StartUnequip();
	}
	else
	{
		// B. 기존 무기가 없으면(맨손) 바로 스폰
		SpawnPendingWeapon();
	}
}

void UInventoryComponent::OnCurrentWeaponUnequipped()
{
	// 기존 무기는 이미 FinishUnequip()에서 Destroy()를 호출했으므로
	// 포인터만 비워줍니다.
	currentWeaponActor = nullptr;

	// 대기 중이던 새 무기 스폰
	SpawnPendingWeapon();
}

void UInventoryComponent::SpawnPendingWeapon()
{
	if (!PendingWeaponInstance || !ownerCharacter) return;
    
    UWeaponDataAsset* WeaponData = PendingWeaponInstance->GetItemData<UWeaponDataAsset>();
    if (!WeaponData) return;
	
	// =================================================================
	// [추가] 플래시라이트 관리 로직 (Primary: 제거 / Secondary: 생성)
	// =================================================================
	if (WeaponData->weaponSlot == EWeaponSlot::Primary)
	{
		// 주무기를 들 때는 플래시라이트 끄기(제거)
		if (IsValid(currentFlashlight))
		{
			currentFlashlight->Destroy();
			currentFlashlight = nullptr;
		}
	}
	else if (WeaponData->weaponSlot == EWeaponSlot::Secondary)
	{
		// 보조무기를 들 때는 플래시라이트 켜기(생성)
		// 이미 있으면 또 만들지 않음
		if (!IsValid(currentFlashlight) && flashlightClass)
		{
			FActorSpawnParameters FlashlightSpawnParams;
			FlashlightSpawnParams.Owner = ownerCharacter;
			FlashlightSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			currentFlashlight = GetWorld()->SpawnActor<AFlashlight>(
				flashlightClass, 
				ownerCharacter->GetActorLocation(), 
				ownerCharacter->GetActorRotation(), 
				FlashlightSpawnParams
			);

			if (currentFlashlight)
			{
				// 캐릭터의 왼손 등 지정된 소켓에 부착
				currentFlashlight->AttachToComponent(
					ownerCharacter->GetMesh(), 
					FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
					TEXT("hand_l_flashlight")
				);
			}
		}
	}

	// =================================================================
	

    // 1. 스폰 파라미터 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = ownerCharacter;
    SpawnParams.Instigator = ownerCharacter;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLocation = ownerCharacter->GetActorLocation();
    FRotator SpawnRotation = ownerCharacter->GetActorRotation();

    // 2. 소켓 이름 결정 (주무기/보조무기에 따라 다름)
    FName TargetSocketName = NAME_None;
    
    // WeaponSlot 타입에 따라 소켓 분류
    if (WeaponData->weaponSlot == EWeaponSlot::Primary)
    {
        TargetSocketName = WeaponData->rHandRifleSocketName;
    }
    else if (WeaponData->weaponSlot == EWeaponSlot::Secondary)
    {
        TargetSocketName = WeaponData->rHandPistolSocketName;
    }
    // Melee 등 추가 가능...

    // 3. 액터 스폰 (BaseWeaponActor로 통일하거나, 데이터 에셋의 클래스 사용)
    // PistolActor와 WeaponActor가 다르다면 Cast나 템플릿으로 분기해도 되지만, 
    // 여기서는 DataAsset의 ActorClass를 믿고 스폰합니다.
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(
        WeaponData->actorClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    // 4. 캐스팅 및 초기화
    ABaseWeaponActor* NewWeaponActor = Cast<ABaseWeaponActor>(NewActor);
    if (NewWeaponActor)
    {
        // 데이터 로드
        NewWeaponActor->LoadWeaponInstance(PendingWeaponInstance);
        
        // 멤버 변수 갱신
        currentWeaponActor = NewWeaponActor;
        currentWeaponActor->weaponInstance = PendingWeaponInstance;

        // 5. 부착 (Attach)
        if (TargetSocketName != NAME_None)
        {
            NewWeaponActor->AttachToComponent(
                ownerCharacter->GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                TargetSocketName
            );
        }

        // [중요] WeaponActor의 BeginPlay()에서 StartEquip()이 자동 호출되므로
        // 여기서는 별도로 StartEquip을 호출할 필요가 없습니다.
        
    	// [추가] 데이터 주입과 부착이 끝난 후, 여기서 수동으로 호출!
    	NewWeaponActor->StartEquip();
    	
        // 6. UI 및 탄약 갱신
        ownerCharacter->BroadcastCurrentAmmoUpdate();
        ownerCharacter->BroadcastInventoryAmmoUpdate();
    }
    
    // Pending 초기화 (선택사항, 안전을 위해)
    // PendingWeaponInstance = nullptr;
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


