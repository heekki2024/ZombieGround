// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeTaskBase.h"
#include "UObject/Object.h"
#include "NavigationSystem.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "Controllers/ZombieAIController.h"
#include "GetRandomLocation.generated.h"

/**
  * StateTree instance data for the Set Focus task.
 * This struct holds the context data for the task, such as the actor to focus on.
*/
USTRUCT(BlueprintType)
struct FGetRandomLocationInstanceData
{
	GENERATED_BODY()

	/** The enemy character this task is associated with. */
	// UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	// TObjectPtr<AZombieAIController> zombieAIC;
	//
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<AZombieCharacter> zombieActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float RandomPatrolRadius = 1500.f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	// FVector SearchCenterLocation = FVector::ZeroVector;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Output)
	FVector RandomReachableLocation;
};

USTRUCT(meta = (DisplayName = "GetRandomLocation", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FFindLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	//Tell the runtime what instance-data type we're using
	using FInstanceDataType = FGetRandomLocationInstanceData;
	
	/**
	 * Returns the instance data type for this task.
	*/
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	
	FFindLocation()
	{
		// "이 태스크는 매 프레임 Tick 함수를 호출할 필요가 없습니다"라고 선언
		bShouldCallTick = false;
	}
	/**
	 * Called when entering the state
	 * @param Context The execution context for the state tree
	 * @param Transition The transition that triggered this state
*/
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		FGetRandomLocationInstanceData& Data = Context.GetInstanceData(*this);

		
		
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Context.GetWorld());
		if (!NavSys)
		{
			UE_LOG(LogTemp, Warning, TEXT("GetRandomLocation: NavSys is null."));
			return EStateTreeRunStatus::Failed;
		}

		// 중심점 설정: 입력값이 있으면 쓰고, 없으면 좀비의 현재 위치 사용
		FVector SearchOrigin = Data.zombieActor->GetActorLocation();
		
		FNavLocation NavLocation;

		const bool bFound = NavSys->GetRandomReachablePointInRadius(SearchOrigin, Data.RandomPatrolRadius, NavLocation);
		if (bFound)
		{
			Data.RandomReachableLocation = NavLocation.Location;
			UE_LOG(LogTemp, Warning, TEXT("GetRandomLocation: succeeded"), *SearchOrigin.ToString(), Data.RandomPatrolRadius);
			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GetRandomLocation: Could not find random location around %s within radius %f."), *SearchOrigin.ToString(), Data.RandomPatrolRadius);
			return EStateTreeRunStatus::Failed;
		}
	}

};
