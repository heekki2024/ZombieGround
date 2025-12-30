// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeTaskBase.h"
#include "UObject/Object.h"
#include "ChaseTask.generated.h"

/**
  * StateTree instance data for the Set Focus task.
 * This struct holds the context data for the task, such as the actor to focus on.
*/
USTRUCT(BlueprintType)
struct FChaseTaskInstanceData
{
	GENERATED_BODY()

	/** The enemy character this task is associated with. */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AZombieCharacter> Actor;

	/** The AI controller managing the enemy character. */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AZombieAIController> AIController;

	/** The actor to chase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	TObjectPtr<AActor> TargetHuman = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float AcceptanceRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FVector LocationToMove;
	// /** Whether to set focus on the actor */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task", meta = (DisplayName = "Set Focus", ToolTip = "If true, the AI will set focus on the specified actor. If false, it will clear focus."))
	// bool bSetFocusOnActor = true;
};

USTRUCT(meta = (DisplayName = "ChaseHumanTask", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FChaseTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	//Tell the runtime what instance-data type we're using
	using FInstanceDataType = FChaseTaskInstanceData;
	
	/**
	 * Returns the instance data type for this task.
	*/
	virtual const UStruct* GetInstanceDataType() const override { return FChaseTaskInstanceData::StaticStruct(); }
	
	/**
	 * Called when entering the state
	 * @param Context The execution context for the state tree
	 * @param Transition The transition that triggered this state
*/
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		const FChaseTaskInstanceData& Data = Context.GetInstanceData(*this);
		if (ensure(Data.TargetHuman))
		{
			if (AAIController* AI = Cast<AAIController>(Context.GetOwner()))
			{
				AI->MoveToActor(Data.TargetHuman, Data.AcceptanceRadius);
			}
		}

		// Keep the task alive so that TickState will be invoked
		return EStateTreeRunStatus::Running;
	}
	
};
