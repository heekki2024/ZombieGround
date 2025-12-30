// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTrees/Schemas/ZombieAIStateTreeSchema.h"

#include "Character/Zombie/ZombieCharacter.h"
#include "Controllers/ZombieAIController.h"

TArray<FStateTreeExternalDataDesc> UZombieAIStateTreeSchema::ContextDataDescs;

UZombieAIStateTreeSchema::UZombieAIStateTreeSchema(const FObjectInitializer& ObjectInitializer)
{
	AIControllerClass = AZombieAIController::StaticClass(); // Set the AIController class for this schema
	ContextActorClass = AZombieCharacter::StaticClass(); // Set the context actor class for this schema
	
	// Initialize context (AIController and Actor seen in StateTree Editor)
	// This is done once to avoid repeated allocations and ensure consistency across executions.
	
	if (ContextDataDescs.IsEmpty())
	{
		// Built-in Context Actor
		ContextDataDescs.Add(FStateTreeExternalDataDesc(FName(TEXT("Actor")),
			AZombieCharacter::StaticClass(),
			// FGuid::NewGuid()
			FGuid(0x8A123456, 0x11112222, 0x33334444, 0x55556666) // <--- 고정값 1
		));
		
		// Built-in AIController
		ContextDataDescs.Add(FStateTreeExternalDataDesc(
			FName(TEXT("AIController")),
			AZombieAIController::StaticClass(),
			// FGuid::NewGuid()
			FGuid(0x9B654321, 0xAAAABBBB, 0xCCCCDDDD, 0xEEEEFFFF) // <--- 고정값 2
		));
	}
}

TConstArrayView<FStateTreeExternalDataDesc> UZombieAIStateTreeSchema::GetContextDataDescs() const
{
	return ContextDataDescs;
}


/*
 * This function is called to validate if a struct is allowed in the StateTree context.
 * It can be used to restrict which structs can be used in the StateTree.
 * @param InScriptStruct The struct to check.
 * @return true if the struct is allowed, false otherwise.
 * @notes This allows the editor to see our custom structs like FPGAS_CompareGameplayTagCondition, etc. By default the base class returns false so our custom structs are not visible.
*/
bool UZombieAIStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	
	// if (Super::IsStructAllowed(InScriptStruct))
	// {
	//     return true;
	// }

	// // Add your custom condition structs here
	// if (InScriptStruct == FPGAS_CompareGameplayTagCondition::StaticStruct())
	// {
	//     return true;
	// }

	return true; // Allow all structs for now
}