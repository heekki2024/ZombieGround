// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Zombie/ZombieAnimInstance.h"

#include "Character/Zombie/ZombieCharacter.h"
#include "GameFramework/PawnMovementComponent.h"


void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	pawnOwner = Cast<AZombieCharacter>(TryGetPawnOwner());
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// Preview mode safety
	//에님 블루프린트 프리뷰(혹은 썸네일)를 볼때는 게임실행상태가 아니라 pawnOwner이 없기에 크래시 남
	if (!pawnOwner)
	{
		pawnOwner = Cast<AZombieCharacter>(TryGetPawnOwner());
		if (!pawnOwner) return;
	}
	
	//이동 속도 설정 (vector 가져옴)
	velocity = pawnOwner->GetVelocity();
	//수평 이동 속력 (루트 x^2 + y^2 해도 됨) 
	groundSpeed = velocity.Size2D();
	//공중 여부 설정
	isAir = pawnOwner->GetMovementComponent()->IsFalling();
	
	// auto* Inventory = pawnOwner->inventoryComponent;
	// auto* WeaponActor = Inventory ? Inventory->currentWeaponActor : nullptr;
	// auto* WeaponInstance = WeaponActor ? WeaponActor->weaponInstance : nullptr;
	
	// currentWeaponActor = WeaponActor;
	// currentWeaponInstance = WeaponInstance;
	
	// if (pawnOwner->inventoryComponent->currentWeaponActor->weaponInstance)
	// 	currentWeaponInstance = pawnOwner->inventoryComponent->currentWeaponActor->weaponInstance;
	// //
	// if (currentWeaponActor)
	// {
	// 	bIsRightClicking = currentWeaponActor->bIsRightClicking;
	// }
	//
	// bHasCurrentWeapon = (currentWeaponActor != nullptr);
	//
	// if (IsValid(currentWeaponInstance))
	// {
	// 	playerAnimData = currentWeaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData;
	// }
	// else
	// {
	// 	playerAnimData = defaultPlayerAnimData;
	// }
}
