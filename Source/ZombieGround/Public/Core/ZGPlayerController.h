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
	
public:
	// [설정] 에디터에서 넣어줄 위젯 클래스들
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HumanHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ZombieHUDClass;

protected:
	// 현재 화면에 떠 있는 위젯을 기억하는 변수
	UPROPERTY()
	UUserWidget* currentHUDWidget;

	// 빙의(캐릭터 교체)가 일어날 때 자동으로 호출되는 함수 오버라이드
	virtual void OnPossess(APawn* InPawn) override;

	// 실제 UI를 교체하는 함수
	void UpdateHUDState(APawn* NewPawn);
};
