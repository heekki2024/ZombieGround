// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChaseMoveTask.h"
#include "StateTreeTaskBase.h"
#include "UObject/Object.h"
#include "AttackTask.generated.h"

/**
 * 
 */

USTRUCT()
struct FAttackTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
	TObjectPtr<AZombieAIController> ZombieAIC = nullptr;

	UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
	TObjectPtr<AZombieCharacter> ZombieActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Input)
	AActor* TargetHuman = nullptr;

	// 공격 사거리 (이 거리 안에 들어오면 공격 시도)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackRange = 200.0f;
	
	// 공격 쿨타임 (연속 공격 방지)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackCooldown = 1.5f;

	// 내부 변수: 마지막 공격 시간
	float LastAttackTime = -999.0f;
};

USTRUCT(meta = (DisplayName = "Attack", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
    
	using FInstanceDataType = FAttackTaskInstanceData; // 데이터 구조체 재사용

	FAttackTask() { bShouldCallTick = true; }
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		FInstanceDataType& Data = Context.GetInstanceData(*this);
		AActor* Target = Data.TargetHuman;

		if (!IsValid(Target)) return EStateTreeRunStatus::Failed;

		// 1. 거리 계산
		float DistanceSq = FVector::DistSquared(Data.ZombieActor->GetActorLocation(), Target->GetActorLocation());
		float AttackRangeSq = FMath::Square(Data.AttackRange);

		// Debug Log
		// UE_LOG(LogTemp, Display, TEXT("AttackTask Tick: DistSq: %f, RangeSq: %f, CooldownRem: %f"), 
		// 	DistanceSq, AttackRangeSq, Data.AttackCooldown - (World->GetTimeSeconds() - Data.LastAttackTime));

		// 2. 공격 조건 (사거리 안 + 쿨타임)
		if (DistanceSq <= AttackRangeSq)
		{
			UWorld* World = Data.ZombieActor->GetWorld();
			if (World->GetTimeSeconds() - Data.LastAttackTime >= Data.AttackCooldown)
			{
				UE_LOG(LogTemp, Warning, TEXT("AttackTask: Triggering Attack! Distance: %f"), FMath::Sqrt(DistanceSq));

				// [공격 실행] 이동을멈추지 않고 애니메이션만 재생
				Data.ZombieActor->PlayBasicAttackMontage();
                
				Data.LastAttackTime = World->GetTimeSeconds();
			}
			else
			{
				// UE_LOG(LogTemp, Display, TEXT("AttackTask: In Cooldown."));
			}
		}
		else
		{
			// UE_LOG(LogTemp, Display, TEXT("AttackTask: Out of Range."));
		}

		// 공격을 했다고 해서 Succeeded로 끝내지 않음 (계속 따라가며 때려야 하므로)
		return EStateTreeRunStatus::Running;
	}
};
