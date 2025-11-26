// Fill out your copyright notice in the Description page of Project Settings.


#include "Human/Animation/HumanAnim.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Human/HumanCharacter.h"

void UHumanAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	//소유 Pawn 을 가져오자.
	pawnOwner = Cast<AHumanCharacter>(TryGetPawnOwner());
	
	currentWeaponNameEnum = EWeaponName::Unarmed;
}

void UHumanAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	//에님 블루프린트 프리뷰(혹은 썸네일)를 볼때는 게임실행상태가 아니라 pawnOwner이 없기에 크래시 남
	if (pawnOwner)
	{
		//이동 속도 설정 (vector 가져옴)
		velocity = pawnOwner->GetVelocity();
		//수평 이동 속력 (루트 x^2 + y^2 해도 됨) 
		groundSpeed = velocity.Size2D();
		//공중 여부 설정
		isAir = pawnOwner->GetMovementComponent()->IsFalling();
		
		if (pawnOwner->GetCurrentWeapon())
		{
			bIsRightClicking = pawnOwner->GetCurrentWeapon()->GetbIsRightClicking();
		}

		
		currentWeapon = pawnOwner->GetCurrentWeapon();
		bHasCurrentWeapon = (currentWeapon != nullptr);
		
		if (IsValid(currentWeapon))
		{
			WeaponAnimSetToUse = currentWeapon->weaponAnimSet;
		}
		else
		{
			WeaponAnimSetToUse = DefaultWeaponAnimSet;
		}
	}
}
