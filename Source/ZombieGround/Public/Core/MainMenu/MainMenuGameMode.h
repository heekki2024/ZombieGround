// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API AMainMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
	AMainMenuGameMode();
	
public:
	virtual void BeginPlay() override;
	
	// 에디터에서 WBP_MainMenu를 넣어줄 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> mainMenuWidgetClass;

	// 생성된 위젯을 담아둘 변수
	UPROPERTY()
	class UUserWidget* mainMenuWidgetInstance;
};
