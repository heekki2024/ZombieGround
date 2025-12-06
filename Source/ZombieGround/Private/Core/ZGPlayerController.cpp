// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ZGPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "UI/InGame/Human/HumanHUD.h"

void AZGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어에서만 위젯 생성 (멀티플레이 안전)
	if (IsLocalController() && HumanHUDClass)
	{
		HumanHUDWidget = CreateWidget<UHumanHUD>(this, HumanHUDClass);
		if (HumanHUDWidget)
		{
			HumanHUDWidget->AddToViewport(0); // Z-order 0
			// 옵션: SetInputModeGameAndUI, bShowMouseCursor 등 조절
			// FInputModeGameOnly InputMode;
			// SetInputMode(InputMode);
		}
	}
	
	
}
