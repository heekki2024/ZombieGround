// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ZGPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/Human/HumanCharacter.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "UI/InGame/Human/HumanHUD.h"

void AZGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// // 로컬 플레이어에서만 위젯 생성 (멀티플레이 안전)
	// if (IsLocalController() && HumanHUDClass)
	// {
	// 	currentHUDWidget = CreateWidget<UHumanHUD>(this, HumanHUDClass);
	// 	if (currentHUDWidget)
	// 	{
	// 		currentHUDWidget->AddToViewport(0); // Z-order 0
	// 		// 옵션: SetInputModeGameAndUI, bShowMouseCursor 등 조절
	// 		// FInputModeGameOnly InputMode;
	// 		// SetInputMode(InputMode);
	// 	}
	// }
}

void AZGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// 로컬 플레이어인 경우에만 UI 업데이트 (서버/AI 제외)
	if (IsLocalController())
	{
		UpdateHUDState(InPawn);
	}
}

void AZGPlayerController::UpdateHUDState(APawn* NewPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateHUDState Called. NewPawn: %s"), NewPawn ? *NewPawn->GetName() : TEXT("None"));

	// 1. 기존에 떠 있던 위젯이 있다면 제거 (매우 중요!)
	if (currentHUDWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Removing existing HUD widget: %s"), *currentHUDWidget->GetName());
		currentHUDWidget->RemoveFromParent();
		currentHUDWidget = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("currentHUDWidget is null. No widget to remove."));
	}

	// 2. 새로 조종하게 된 폰이 무엇인지 검사
	if (!NewPawn) return;

	TSubclassOf<UUserWidget> TargetWidgetClass = nullptr;

	// A. 인간인지 확인
	if (NewPawn->IsA(AHumanCharacter::StaticClass()))
	{
		TargetWidgetClass = HumanHUDClass;
		UE_LOG(LogTemp, Warning, TEXT("Switching to Human HUD"));
	}
	// B. 좀비인지 확인 (AZombieCharacter 상속받은 모든 좀비 포함)
	else if (NewPawn->IsA(AZombieCharacter::StaticClass()))
	{
		TargetWidgetClass = ZombieHUDClass;
		UE_LOG(LogTemp, Warning, TEXT("Switching to Zombie HUD"));
	}

	// 3. 해당 위젯 생성 및 부착
	if (TargetWidgetClass)
	{
		currentHUDWidget = CreateWidget<UUserWidget>(this, TargetWidgetClass);
		if (currentHUDWidget)
		{
			currentHUDWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Created new HUD widget: %s"), *currentHUDWidget->GetName());
		}
	}
}
