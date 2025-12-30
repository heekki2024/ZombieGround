// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponentSchema.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"

#include "ZombieAIStateTreeSchema.generated.h"




/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Zombie AI State Tree Schema"))
class ZOMBIEGROUND_API UZombieAIStateTreeSchema : public UStateTreeAIComponentSchema
{
	GENERATED_BODY()
	
	UZombieAIStateTreeSchema(const FObjectInitializer& ObjectInitializer);

	
	// Returns the context data descriptions for the schema.
	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;
	
	/*
 * Returns the context description for the given context.
 * This is used to validate the context data for the StateTree.
*/
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

	
	/** Cache the context description for performance and reuse */
	static TArray<FStateTreeExternalDataDesc> ContextDataDescs;
};
