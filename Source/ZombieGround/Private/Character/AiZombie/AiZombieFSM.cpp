// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiZombie/AiZombieFSM.h"

#include "AIController.h"
#include "Character/AiZombie/AiZombie.h"
#include "Character/Human/HumanCharacter.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/TargetPoint.h" // [추가]

// Sets default values for this component's properties
UAiZombieFSM::UAiZombieFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// [수정] 대기 시간을 3초로 늘려 너무 자주 이동하지 않게 함
	idleDelayTime = 0.1f;
}

// Called when the game starts
void UAiZombieFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	compOwner = Cast<AAiZombie>(GetOwner());
    
	// [추가] 맵에 배치된 모든 TargetPoint 수집
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), AllTargetPoints);
	
	if (AllTargetPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("FSM: No TargetPoints found! Patrol will not work."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("FSM: Found %d TargetPoints."), AllTargetPoints.Num());
	}
}


// Called every frame
void UAiZombieFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(compOwner)) return;

	// [수정 2] AIController가 없다면 매 프레임 다시 찾기를 시도합니다 (Lazy Initialization)
	if (!IsValid(aiController))
	{
		aiController = Cast<AAIController>(compOwner->GetController());
	}

	// [수정 3] 타겟 유효성 검사 및 상태 복구
	if (!IsValid(target))
	{
		if (zombieState != EZombieState::Die)
		{
			zombieState = EZombieState::Idle;
		}
	}

	FString stateStr = UEnum::GetValueAsString(zombieState);
	// GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, *stateStr); // 화면 로그는 유지
	
	// [디버그] 상태 머신 실행 확인
	// UE_LOG(LogTemp, Log, TEXT("FSM Tick: State = %s"), *stateStr);
    
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
	// case EZombieState::Damage:
	// 	DamageState();
	// 	break;
	// case EZombieState::Die:
	// 	DieState();
	// 	break;
	}
}



void UAiZombieFSM::OnTargetDetected(AHumanCharacter* NewTarget)
{
	target = NewTarget;
	zombieState = EZombieState::Move;
	currentTime = 0;
	
	if (aiController)
	{
		aiController->StopMovement();
	}
}

void UAiZombieFSM::FindRandomTargetPoint()
{
	// 타겟 포인트가 없거나 1개뿐이라면 선택의 여지가 없음
	if (AllTargetPoints.Num() <= 1)
	{
		if (AllTargetPoints.Num() == 1)
		{
			CurrentTargetPoint = AllTargetPoints[0];
		}
		else
		{
			CurrentTargetPoint = nullptr;
		}
		return;
	}

	// 현재 지점을 제외한 새로운 랜덤 지점 찾기
	AActor* NewTarget = nullptr;
	int32 MaxAttempts = 10; // 무한 루프 방지

	do 
	{
		int32 RandomIndex = FMath::RandRange(0, AllTargetPoints.Num() - 1);
		NewTarget = AllTargetPoints[RandomIndex];
		MaxAttempts--;
	} 
	while (NewTarget == CurrentTargetPoint && MaxAttempts > 0);

	CurrentTargetPoint = NewTarget;
}

void UAiZombieFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 1. 플레이어 감지 시 추격
	if (IsValid(target))
	{
		zombieState = EZombieState::Move;
		currentTime = 0;
		return;
	}

	if (currentTime >= idleDelayTime)
	{
		if (!IsValid(aiController)) return;

		// 2. 타겟 포인트 목록이 비었다면 재수집 (BeginPlay에서 실패했을 경우 대비)
		if (AllTargetPoints.Num() == 0)
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), AllTargetPoints);
		}

		// 3. 목적지 선정 및 이동
		if (AllTargetPoints.Num() > 0)
		{
			// 현재 목적지가 없다면 새로 선정
			if (!IsValid(CurrentTargetPoint))
			{
				FindRandomTargetPoint();
			}
			
			if (IsValid(CurrentTargetPoint))
			{
				aiController->MoveToLocation(CurrentTargetPoint->GetActorLocation());
				zombieState = EZombieState::Move;
				currentTime = 0;
			}
		}
	}
}

void UAiZombieFSM::MoveState()
{
	if (!IsValid(aiController)) return;

	// A. 추격 모드
	if (IsValid(target))
	{
		aiController->MoveToLocation(target->GetActorLocation());

		float distance = FVector::Dist(target->GetActorLocation(), compOwner->GetActorLocation());
		if (distance < attackRange)
		{
			zombieState = EZombieState::Attack;
			currentTime = attackDelayTime;
		}
	}
	// B. 패트롤 모드
	else
	{
		if (IsValid(CurrentTargetPoint))
		{
			float dist = FVector::Dist(compOwner->GetActorLocation(), CurrentTargetPoint->GetActorLocation());
			
			// 도착 판정 (150cm)
			if (dist < 150.0f) 
			{
				// 도착했으므로 타겟을 비우고 Idle로 가서 다음 타겟 찾기
				CurrentTargetPoint = nullptr;
				zombieState = EZombieState::Idle;
				currentTime = 0;
			}
		}
		else
		{
			// 목적지가 없으면 Idle로
			zombieState = EZombieState::Idle;
			currentTime = 0;
		}
	}
}

// [이전 코드 삭제됨] - 새로운 MoveState가 상단에 구현되어 있습니다.

void UAiZombieFSM::AttackState()
{
	if (!IsValid(target) || !IsValid(compOwner))
	{
		zombieState = EZombieState::Idle;
		return;
	}

	// [추가] 공격 상태에서도 계속 이동 (MoveToLocation 호출)
	// 이렇게 해야 공격 애니메이션(상체)이 나오면서 다리는 계속 따라감
	if (IsValid(aiController))
	{
		aiController->MoveToLocation(target->GetActorLocation());
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

// void UAiZombieFSM::DamageState()
// {
// 	if (!IsValid(compOwner))
// 	{
// 		return;
// 	}
//
// 	currentTime += GetWorld()->DeltaTimeSeconds;
// 	if (currentTime > damageDelayTime)
// 	{
// 		currentTime = 0;
// 		zombieState = EZombieState::Idle;
// 	}
// 	
// 	float percent = GetWorld()->DeltaTimeSeconds * 10;
// 	FVector P = FMath::Lerp(compOwner->GetActorLocation(), compOwner->knockbackPos, percent);
// 	float dist = FVector::Dist(P, compOwner->GetActorLocation());
// 	if (dist < 10)
// 	{
// 		P = compOwner->GetActorLocation();
// 	}
// 	else
// 	{
// 		compOwner->SetActorLocation(P, true);
// 	}
//
// 	compOwner->PlayDamageMontage();
// 	
// }

// void UAiZombieFSM::DieState()
// {
// 	if (!IsValid(compOwner))
// 	{
// 		return;
// 	}
//
// 	compOwner->SetActorEnableCollision(false);
// 	compOwner->SetActorLocation(compOwner->GetActorLocation() + (-compOwner->GetActorUpVector() * 100 * GetWorld()->GetWorld()->DeltaTimeSeconds));
// 	if (compOwner->GetActorLocation().Z < -80)
// 		compOwner->Destroy();
// 	
// }

void UAiZombieFSM::ChangeState(EZombieState NewState)
{
	zombieState = NewState;
	currentTime = 0.0f;
}