// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UBaseInstance : public UObject
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	virtual void InitInstance(class UBaseItemDataAsset* itemDataAsset) {};

};
