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
	
	// 1. 인터페이스 함수 정의
	// BlueprintNativeEvent: C++과 블루프린트 양쪽에서 구현 가능하게 함
	// 파라미터(Caller): 누가 상호작용을 시도했는지 알려줌 (주로 Character)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(class AHumanCharacter* characterPickingUp,class ABasePickup* interactedPickup);
};
