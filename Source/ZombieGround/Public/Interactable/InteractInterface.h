// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIEGROUND_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	// E키 눌렀을 때 호출
	virtual void OnInteract(AActor* InteractingActor) = 0;

	// UI 표시용
	virtual FText GetInteractText() const = 0;
};
