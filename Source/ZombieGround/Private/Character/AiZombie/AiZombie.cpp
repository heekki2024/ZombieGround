// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiZombie/AiZombie.h"

#include "Character/AiZombie/AiZombieFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
AAiZombie::AAiZombie()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	//이동 방향으로 회전 속성설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//엑터 컴포넌트이기 때문에 다른거의 자식으로 들어가지 않는다.
	fsm = CreateDefaultSubobject<UAiZombieFSM>(TEXT("FSM"));
}

// Called when the game starts or when spawned
void AAiZombie::BeginPlay()
{
	Super::BeginPlay();
	

	
}

// Called every frame
void AAiZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAiZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAiZombie::OnDamageProcess(FVector hitDirection)
{
	// Super::OnDamageProcess();
	
	//체력
	currentHP--;
	
	//경과시간 초기화
	fsm->currentTime = 0;
	
	if (currentHP > 0)
	{
		fsm->zombieState = EZombieState::Damage;
		
		//살아있음
		hitDirection.Z = 0;
		FVector force = hitDirection * knockbackPower;
		knockbackPos  = GetActorLocation() + force;
		// SetActorLocation(knockbackPos, true);
		
		// float percent = GetWorld()->DeltaTimeSeconds * 10;
		// FVector P = FMath::Lerp(GetActorLocation(), knockbackPos,percent);
		//
		// // 원충돌 거의 도착했다는걸 보장하기 위해.
		// float dist = FVector::Dist(P, GetActorLocation());
		// if (dist < 10)
		// {
		// 	P = GetActorLocation();
		// }
		// else
		// {
		// 	SetActorLocation(P, true);
		// }
	}
	else
	{
		fsm->zombieState = EZombieState::Die;
	}
}


