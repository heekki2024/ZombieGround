// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiZombie/AiZombieFSM.h"

#include "AIController.h"
#include "Character/Human/HumanCharacter.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UAiZombieFSM::UAiZombieFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAiZombieFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	compOwner = Cast<AZombieCharacter>(GetOwner());	
	target = Cast<AHumanCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AHumanCharacter::StaticClass()));
	
	//ai controller 할당
	ai = Cast<AAIController>(compOwner->GetController());
}


// Called every frame
void UAiZombieFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. [핵심 수정] 타겟 유효성 검사 (안전 장치)
	// 타겟이 유효하지 않고, 현재 상태가 Die가 아니라면 Idle로 강제 전환
	if (!IsValid(target))
	{
		if (zombieState != EZombieState::Die && zombieState != EZombieState::Idle)
		{
			zombieState = EZombieState::Idle;
			currentTime = 0.0f; // 타이머 초기화
		}
	}

	FString stateStr = UEnum::GetValueAsString(zombieState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, *stateStr);
    
	switch (zombieState)
	{
	case EZombieState::Idle:
		IdleState();
		break;
	case EZombieState::Move:
		MoveState();
		break;
	case EZombieState::Attack:
		AttackState();
		break;
	case EZombieState::Damage:
		DamageState();
		break;
	case EZombieState::Die:
		DieState();
		break;
	}
}

void UAiZombieFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime)
	{
		// 3. 상태를 이동으로 전환한다.
		zombieState = EZombieState::Move;
	}
	
}

void UAiZombieFSM::MoveState()
{
	//플레이어가 없으면???
	//아무처리하지 않는다.
	if (!IsValid(target))
	{
		zombieState = EZombieState::Idle;
		return;
	}
	FVector direction = target->GetActorLocation() - compOwner->GetActorLocation();
	
	// 루트 a제곱+b제곱+c제곱
	float distance = direction.Size();
	
	direction.Normalize();
	
	// compOwner->AddMovementInput(direction);
	ai->MoveToLocation(target->GetActorLocation());
	
	//타겟이 공격범위 안에 들어오면 상태를 공격으로 전환한다.
	if (distance < attackRange)
	{
		zombieState = EZombieState::Attack;
		currentTime = attackDelayTime;
	}
	
}

void UAiZombieFSM::AttackState()
{
	if (!IsValid(target))
	{
		zombieState = EZombieState::Idle;
		return;
	}
	
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > attackDelayTime)
	{
		currentTime = 0;
		UE_LOG(LogTemp, Warning, TEXT("Attack!!!"));
		compOwner->PlayBasicAttackMontage();
		//타겟을 바라보게 하자
		FVector dir = target->GetActorLocation() - compOwner->GetActorLocation();
		FRotator rot = UKismetMathLibrary::MakeRotFromZX(compOwner->GetActorUpVector(), dir);
		compOwner->SetActorRotation(rot);
	}
	
	//거리
	float distance = FVector::Dist(target->GetActorLocation(), compOwner->GetActorLocation());
	if (distance > attackRange)
	{
		zombieState = EZombieState::Move;
	}
}

//changeState를 하나 만드는게 좋다. 초기값 설정을 위해.

void UAiZombieFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		currentTime = 0;
		zombieState = EZombieState::Idle;
	}
	
	float percent = GetWorld()->DeltaTimeSeconds * 10;
	FVector P = FMath::Lerp(compOwner->GetActorLocation(), compOwner->knockbackPos, percent);
	float dist = FVector::Dist(P, compOwner->GetActorLocation());
	if (dist < 10)
	{
		P = compOwner->GetActorLocation();
	}
	else
	{
		compOwner->SetActorLocation(P, true);
	}

	compOwner->PlayDamageMontage();
	
}

void UAiZombieFSM::DieState()
{
	compOwner->SetActorEnableCollision(false);
	compOwner->SetActorLocation(compOwner->GetActorLocation() + (-compOwner->GetActorUpVector() * 100 * GetWorld()->GetWorld()->DeltaTimeSeconds));
	if (compOwner->GetActorLocation().Z < -80)
		compOwner->Destroy();
	
}

