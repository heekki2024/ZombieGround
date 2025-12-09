// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiZombie/AiZombieFSM.h"

#include "AIController.h"
#include "Character/AiZombie/AiZombie.h"
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
	compOwner = Cast<AAiZombie>(GetOwner());
    
	// [수정 1] BeginPlay에서는 AI Controller를 못 가져올 수도 있으므로 변수 초기화만 합니다.
	// 타겟 찾는 로직도 여기서 제거하고 Tick이나 IdleState로 넘기는 것이 안전합니다.
}


// Called every frame
void UAiZombieFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(compOwner)) return;

	// [수정 2] AIController가 없다면 매 프레임 다시 찾기를 시도합니다 (Lazy Initialization)
	// 좀비가 스폰되고 컨트롤러가 빙의될 때까지 약간의 딜레이가 있기 때문입니다.
	if (!IsValid(aiController))
	{
		aiController = Cast<AAIController>(compOwner->GetController());
		// 아직도 컨트롤러가 없다면 아무것도 하지 않고 리턴 (오류 방지)
		if (!IsValid(aiController)) return; 
	}

	// [수정 3] 타겟 유효성 검사 및 상태 복구
	// 타겟이 죽거나 사라졌다면 다시 Idle로 가서 새로운 타겟을 찾도록 유도
	if (!IsValid(target))
	{
		// 죽은 상태가 아니라면 Idle로 전환하여 재탐색 유도
		if (zombieState != EZombieState::Die)
		{
			zombieState = EZombieState::Idle;
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
	// 타겟이 없거나 유효하지 않으면 가장 가까운 인간 탐색
	if (!IsValid(target))
	{
		target = FindNearestTarget();
	}

	// 타겟을 찾았다면 바로 이동 상태로 전환 (또는 딜레이 후 전환)
	if (IsValid(target))
	{
		// 원한다면 바로 추격 시작
		zombieState = EZombieState::Move;
		currentTime = 0;
		return; 
	}

	// 타겟을 못 찾았을 때만 대기 시간 로직 수행
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime)
	{
		// 여기서는 랜덤 패트롤 로직 등을 넣을 수 있음
		// 지금은 타겟 없으면 계속 Idle 유지
	}
	
}

class AHumanCharacter* UAiZombieFSM::FindNearestTarget()
{
	// 1. 맵의 모든 HumanCharacter 찾기
	TArray<AActor*> FoundHumans;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHumanCharacter::StaticClass(), FoundHumans);

	// 찾은 사람이 없으면 nullptr 반환
	if (FoundHumans.Num() == 0) return nullptr;

	AHumanCharacter* NearestHuman = nullptr;
	float MinDistanceSq = FLT_MAX; // 비교를 위해 가장 큰 숫자로 초기화

	FVector MyPos = compOwner->GetActorLocation();

	// 2. 반복문을 돌며 거리 비교
	for (AActor* Actor : FoundHumans)
	{
		AHumanCharacter* Human = Cast<AHumanCharacter>(Actor);
		if (Human)
		{
			// 현재 좀비와 인간 사이의 거리(제곱) 계산
			// DistSquared가 Dist보다 루트 연산이 없어서 성능이 조금 더 좋습니다.
			float DistSq = FVector::DistSquared(MyPos, Human->GetActorLocation());

			// 현재까지 찾은 최소 거리보다 더 가깝다면 갱신
			if (DistSq < MinDistanceSq)
			{
				MinDistanceSq = DistSq;
				NearestHuman = Human;
			}
		}
	}

	return NearestHuman;
}

void UAiZombieFSM::MoveState()
{
	// 1. 예외 처리
	if (!IsValid(target) || !IsValid(compOwner))
	{
		zombieState = EZombieState::Idle;
		return;
	}

	// AI Controller Lazy Init
	if (!IsValid(aiController))
	{
		aiController = Cast<AAIController>(compOwner->GetController());
		if (!IsValid(aiController)) return;
	}
    
	// ----------------------------------------------------------------
	// [추가된 로직] 주기적으로 더 가까운 타겟이 있는지 확인
	// ----------------------------------------------------------------
	ReSearchTimer += GetWorld()->DeltaTimeSeconds;
    
	// 설정한 주기(ReSearchRate)가 지났으면 재검색
	if (ReSearchTimer > ReSearchRate)
	{
		ReSearchTimer = 0.0f; // 타이머 초기화

		// 현재 타겟보다 더 가까운 타겟이 있는지 찾음
		AHumanCharacter* NewTarget = FindNearestTarget();

		// 새로운 타겟이 존재하고, 기존 타겟과 다르다면 교체
		if (NewTarget && NewTarget != target)
		{
			target = NewTarget;
			// 타겟이 바뀌었으니 이동 명령을 즉시 갱신하기 위해 Stop을 한 번 호출해주는 것도 좋음(선택 사항)
			// aiController->StopMovement(); 
		}
	}
	// ----------------------------------------------------------------

	// 2. 이동 처리
	// (매 프레임 호출해도 되지만, MoveToLocation은 내부적으로 최적화되어 있어 괜찮습니다)
	aiController->MoveToLocation(target->GetActorLocation());
    
	// 3. 거리 계산 및 공격 범위 확인
	// 현재 타겟과의 거리 계산
	float distance = FVector::Dist(target->GetActorLocation(), compOwner->GetActorLocation());
    
	// 타겟이 공격범위 안에 들어오면 상태를 공격으로 전환한다.
	if (distance < attackRange)
	{
		zombieState = EZombieState::Attack;
		currentTime = attackDelayTime;
       
		// 상태가 바뀌면 타이머도 초기화 해주는 습관을 들이면 좋습니다.
		ReSearchTimer = 0.0f; 
	}
}

void UAiZombieFSM::AttackState()
{
	if (!IsValid(target) || !IsValid(compOwner))
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
	if (!IsValid(compOwner))
	{
		return;
	}

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
	if (!IsValid(compOwner))
	{
		return;
	}

	compOwner->SetActorEnableCollision(false);
	compOwner->SetActorLocation(compOwner->GetActorLocation() + (-compOwner->GetActorUpVector() * 100 * GetWorld()->GetWorld()->DeltaTimeSeconds));
	if (compOwner->GetActorLocation().Z < -80)
		compOwner->Destroy();
	
}

