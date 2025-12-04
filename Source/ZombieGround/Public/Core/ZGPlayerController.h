// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API AZGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
protected:
	// 에디터에서 할당할 수 있게 노출
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HumanHUDClass;

	UPROPERTY()
	class UHumanHUD* HumanHUDWidget = nullptr;
};
