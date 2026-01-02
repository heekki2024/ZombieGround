// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeTaskBase.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "Controllers/ZombieAIController.h"
#include "Character/Human/HumanCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "ChaseMoveTask.generated.h"


USTRUCT()
struct FChaseMoveTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
	TObjectPtr<AZombieAIController> ZombieAIC = nullptr;

	UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
	TObjectPtr<AZombieCharacter> ZombieActor = nullptr;

	// 공격 사거리 (이 거리 안에 들어오면 공격 시도)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackRange = 200.0f;
	
	UPROPERTY(EditAnywhere, Category = Output)
	AActor* TargetActor = nullptr;
	
	// [추가] 타겟 재검색 주기 (0.3초)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float TargetCheckInterval = 0.3f;

	// [추가] 내부 타이머 변수 (에디터 노출 불필요)
	UPROPERTY()
	float TimeSinceLastCheck = 0.0f;
	


};

USTRUCT(meta = (DisplayName = "Chase Move", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FChaseMoveTask : public FStateTreeTaskBase
{

	GENERATED_BODY()

	using FInstanceDataType = FChaseMoveTaskInstanceData;

	FChaseMoveTask() { bShouldCallTick = true; }
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		FInstanceDataType& Data = Context.GetInstanceData(*this);
		
		UE_LOG(LogTemp, Log, TEXT("ChaseMoveTask: EnterState. Target: %s"), *GetNameSafe(Data.ZombieAIC ? Data.ZombieAIC->TargetActor : nullptr));

		if (!Data.ZombieAIC || !Data.ZombieAIC->TargetActor) 
		{
			UE_LOG(LogTemp, Warning, TEXT("ChaseMoveTask: Failed. Missing Controller or Target."));
			return EStateTreeRunStatus::Failed;
		}

		// // [이동 명령] 공격과 무관하게 무조건 이동
		// Data.ZombieAIC->MoveToActor(Data.ZombieAIC->TargetActor, 50.0f); 
		// Data.TargetActor = Data.ZombieAIC->TargetActor;
		//
		return EStateTreeRunStatus::Running;
	}

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		FInstanceDataType& Data = Context.GetInstanceData(*this);
    
		// 1. 현재 타겟 유효성 검사 (이건 매 프레임 하는 게 안전함)
		if (!IsValid(Data.ZombieAIC->TargetActor))
		{
			Data.ZombieAIC->StopMovement();
			return EStateTreeRunStatus::Failed;
		}

		// 2. 타이머 누적
		Data.TimeSinceLastCheck += DeltaTime;

		// 3. 0.3초가 지났는지 확인
		if (Data.TimeSinceLastCheck >= Data.TargetCheckInterval)
		{
			// 타이머 리셋 (0으로 초기화하거나 Interval만큼 빼줌)
			Data.TimeSinceLastCheck = 0.0f; 

			// [무거운 로직 실행] 더 가까운 적 탐색
			AActor* BetterTarget = FindClosestHostile(Data);
           
			// 타겟 변경 감지
			// if (BetterTarget && BetterTarget != Data.ZombieAIC->TargetActor) 
			if (BetterTarget)
			{
				// 컨트롤러 및 데이터 갱신
				Data.ZombieAIC->TargetActor = BetterTarget;
				Data.TargetActor = BetterTarget;

				// 이동 목표 변경
				Data.ZombieAIC->MoveToActor(BetterTarget, 50.0f);
               
				// 로그 (필요시)
				// UE_LOG(LogTemp, Log, TEXT("Target switched to closer human"));
			}
		}
		else
		{
			// 동기화만 유지
			Data.TargetActor = Data.ZombieAIC->TargetActor;
		}

		return EStateTreeRunStatus::Running;
	}
	
private:
	// 기존 Controller에 있던 UpdateClosestTarget 로직을 여기로 가져옴
	AActor* FindClosestHostile(FInstanceDataType& Data) const
	{
		UAIPerceptionComponent* PerceptionComp = Data.ZombieAIC->GetAIPerceptionComponent();
		if (!PerceptionComp) return nullptr;

		TArray<AActor*> HostileActors;
		PerceptionComp->GetPerceivedHostileActors(HostileActors);

		AActor* ClosestActor = nullptr;
		float MinDistanceSq = FLT_MAX;
		FVector MyLocation = Data.ZombieActor->GetActorLocation();

		for (AActor* Enemy : HostileActors)
		{
			if (!IsValid(Enemy)) continue;

			// 시야 확인 (Visible Check)
			const FActorPerceptionInfo* Info = PerceptionComp->GetActorInfo(*Enemy);
			if (Info && !Info->LastSensedStimuli.IsEmpty())
			{
				bool bIsVisible = false;
				for (const FAIStimulus& Stimulus : Info->LastSensedStimuli)
				{
					if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>() && Stimulus.WasSuccessfullySensed())
					{
						bIsVisible = true;
						break;
					}
				}
				if (!bIsVisible) continue; // 안 보이면 패스
			}
			else
			{
				continue; 
			}

			// 거리 계산
			float DistSq = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());
			if (DistSq < MinDistanceSq)
			{
				MinDistanceSq = DistSq;
				ClosestActor = Enemy;
			}
		}

		// 더 가까운 적이 없으면(nullptr) 기존 타겟 유지를 위해 현재 타겟 반환할지, 
		// 아니면 nullptr을 반환해서 갱신 로직에서 처리할지 결정.
		// 여기서는 찾은 놈(혹은 nullptr)을 반환.
		return ClosestActor;
	}
};
