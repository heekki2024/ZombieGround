// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Zombie/ZombieAnimInstance.h"

#include "Character/Zombie/ZombieCharacter.h"
#include "GameFramework/PawnMovementComponent.h"


void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	zombieCharacter = Cast<AZombieCharacter>(TryGetPawnOwner());
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// Preview mode safety
	//에님 블루프린트 프리뷰(혹은 썸네일)를 볼때는 게임실행상태가 아니라 pawnOwner이 없기에 크래시 남
	if (!zombieCharacter)
	{
		zombieCharacter = Cast<AZombieCharacter>(TryGetPawnOwner());
		if (!zombieCharacter) return;
	}
	
	//이동 속도 설정 (vector 가져옴)
	//수평 이동 속력 (루트 x^2 + y^2 해도 됨) 
	groundSpeed = zombieCharacter->GetVelocity().Size2D();
	velocity = zombieCharacter->GetVelocity();
	direction = FMath::FindDeltaAngleDegrees(zombieCharacter->GetActorRotation().Yaw, velocity.ToOrientationRotator().Yaw);
	
	//공중 여부 설정
	isAir = zombieCharacter->GetMovementComponent()->IsFalling();
}

void UZombieAnimInstance::OnAttackHitEvent()
{
	
	if (zombieCharacter)
	{
		zombieCharacter->AttackHitCheck();
	}
}
