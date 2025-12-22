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
	UPROPERTY()
	class UStaticMesh* pickupMesh;
	
	UPROPERTY()
	class USkeletalMesh* actorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBaseDataAsset* defaultItemData;

	UPROPERTY()
	int32 currentQuantity;
	
public:
	
	UFUNCTION(BlueprintCallable)
	virtual void InitInstance(class UBaseDataAsset* itemDataAsset) {};

	UFUNCTION(BlueprintCallable)
	virtual UBaseDataAsset* GetItemData() const {return defaultItemData;};

	//템플릿 함수
	template <typename T>
	T* GetItemData() const
	{
		return Cast<T>(defaultItemData);
	}
};
