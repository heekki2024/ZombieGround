// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/HumanAnimInstance.h"
#include "Character/Human/HumanCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
#include "Item/Instance/Weapon/WeaponInstance.h"

void UHumanAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	//소유 Pawn 을 가져오자.
	pawnOwner = Cast<AHumanCharacter>(TryGetPawnOwner());
	
	// currentWeaponNameEnum
}

void UHumanAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// Preview mode safety
	//에님 블루프린트 프리뷰(혹은 썸네일)를 볼때는 게임실행상태가 아니라 pawnOwner이 없기에 크래시 남
	if (!pawnOwner)
	{
		pawnOwner = Cast<AHumanCharacter>(TryGetPawnOwner());
		if (!pawnOwner) return;
	}
	
	//이동 속도 설정 (vector 가져옴)
	velocity = pawnOwner->GetVelocity();
	//수평 이동 속력 (루트 x^2 + y^2 해도 됨) 
	groundSpeed = velocity.Size2D();
	//공중 여부 설정
	isAir = pawnOwner->GetMovementComponent()->IsFalling();
	
	auto* Inventory = pawnOwner->inventoryComponent;
	auto* WeaponActor = Inventory ? Inventory->currentWeaponActor : nullptr;
	auto* WeaponInstance = WeaponActor ? WeaponActor->weaponInstance : nullptr;
	
	currentWeaponActor = WeaponActor;
	currentWeaponInstance = WeaponInstance;
	
	// if (pawnOwner->inventoryComponent->currentWeaponActor->weaponInstance)
	// 	currentWeaponInstance = pawnOwner->inventoryComponent->currentWeaponActor->weaponInstance;
	//
	if (pawnOwner->currentWeaponActor)
	{
		bIsRightClicking = pawnOwner->currentWeaponActor->bIsRightClicking;
	}

	
	currentWeaponActor = pawnOwner->inventoryComponent->currentWeaponActor;
	
	bHasCurrentWeapon = (currentWeaponActor != nullptr);
	
	if (IsValid(currentWeaponInstance))
	{
		playerAnimData = currentWeaponInstance->defaultWeaponData->playerAnimData;
	}
	else
	{
		playerAnimData = defaultPlayerAnimData;
	}
	
}
