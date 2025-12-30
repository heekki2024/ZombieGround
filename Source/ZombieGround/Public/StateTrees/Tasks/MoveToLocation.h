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
#include "Navigation/PathFollowingComponent.h"
#include "MoveToLocation.generated.h"

/**
 * StateTree instance data for the MoveToLocation task.
 */
USTRUCT(BlueprintType)
struct FMoveToLocationInstanceData
{
	GENERATED_BODY()
	
	
		
	// ------------------------------------------------------------------
	// Context Binding (State Tree가 자동으로 연결해줌)
	// ------------------------------------------------------------------
    
	/** Context: AI Controller */
	UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
	TObjectPtr<AZombieAIController> ZombieAIC = nullptr;

	/** Context: Zombie Character */
	UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
	TObjectPtr<AZombieCharacter> ZombieActor = nullptr;

	// ------------------------------------------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float AcceptanceRadius = 50.0f;

	
	
	// Track the specific move request
	FAIRequestID MoveRequestID;
};

USTRUCT(meta = (DisplayName = "MoveToLocation", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FMoveToLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	//Tell the runtime what instance-data type we're using
	using FInstanceDataType = FMoveToLocationInstanceData;

	
	/**
	 * Returns the instance data type for this task.
	*/
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	/**
	 * Called when entering the state
	 * @param Context The execution context for the state tree
	 * @param Transition The transition that triggered this state
	 */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{


		
		FMoveToLocationInstanceData& Data = Context.GetInstanceData(*this);
		// 1. Context 데이터 유효성 검사 (자동 주입되었는지 확인)
		if (!Data.ZombieAIC || !Data.ZombieActor)
		{
			// State Tree 에디터에서 Context 바인딩이 누락되었을 수 있습니다.
			return EStateTreeRunStatus::Failed;
		}

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(Data.TargetLocation);
		MoveRequest.SetAcceptanceRadius(Data.AcceptanceRadius);

		FPathFollowingRequestResult RequestResult = Data.ZombieAIC->MoveTo(MoveRequest);
		
		// RequestID 저장
		Data.MoveRequestID = RequestResult.MoveId;

		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Move Request Result: %s, ID: %u"), *UEnum::GetValueAsString(RequestResult.Code), RequestResult.MoveId.GetID());

		// 3. 이동 요청 결과 확인
		if (RequestResult.Code == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveToLocation: Request Failed Immediately."));
			return EStateTreeRunStatus::Failed;
		}
    
		if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Already At Goal."));
			return EStateTreeRunStatus::Succeeded;
		}

		// 이동 중이라면 Running 상태 반환
		return EStateTreeRunStatus::Running;
	}

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: ExitState. Status: %s"), *UEnum::GetValueAsString(Transition.CurrentRunStatus));
	}

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		const FMoveToLocationInstanceData& Data = Context.GetInstanceData(*this);
	
		AAIController* AIC = Cast<AAIController>(Context.GetOwner());
		if (!AIC && Data.ZombieActor)
		{
			AIC = Cast<AAIController>(Data.ZombieActor->GetController());
		}
		
		if (!AIC)
		{
			return EStateTreeRunStatus::Failed;
		}

		// PathFollowingComponent를 통해 현재 이동 상태 확인
		UPathFollowingComponent* PathFollowingComp = AIC->GetPathFollowingComponent();
		if (!PathFollowingComp)
		{
			return EStateTreeRunStatus::Failed;
		}
		
		// 요청한 MoveID와 현재 수행 중인 MoveID가 다르면(이미 다른 명령으로 덮어씌워졌거나 종료됨) 확인 필요
		// 하지만 간단히 상태로만 체크해도 무방할 때가 많음. 여기서는 상태 로그를 켭니다.

		EPathFollowingStatus::Type Status = PathFollowingComp->GetStatus();

		// 이동이 완료되지 않았으면 계속 Running
		if (Status == EPathFollowingStatus::Moving)
		{
			// 로그 활성화
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Tick - Moving... (RequestID: %u)"), Data.MoveRequestID.GetID());
			return EStateTreeRunStatus::Running;
		}

		// 이동이 멈췄을 때, 성공적으로 도착했는지 확인
		if (PathFollowingComp->DidMoveReachGoal())
		{
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Succeeded (DidMoveReachGoal)."));
			return EStateTreeRunStatus::Succeeded;
		}

		// 그 외(막힘, 경로 소실 등)는 실패로 간주
		UE_LOG(LogTemp, Warning, TEXT("MoveToLocation: Failed in Tick. Status: %s"), *UEnum::GetValueAsString(Status));
		return EStateTreeRunStatus::Failed;
	}

};