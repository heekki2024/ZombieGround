// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainMenu/MainMenuGameMode.h"

#include "Blueprint/UserWidget.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// 메뉴 레벨에서는 폰(캐릭터)이 필요 없으므로 DefaultPawn을 없앱니다.
	DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 1. 위젯 생성 및 화면 부착
	if (mainMenuWidgetClass)
	{
		mainMenuWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), mainMenuWidgetClass);
		if (mainMenuWidgetInstance)
		{
			mainMenuWidgetInstance->AddToViewport();
		}
	}

	// 2. 마우스 커서 보이기 및 입력 모드 설정
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetShowMouseCursor(true);
		
		FInputModeUIOnly InputMode;
		// 위젯이 생성되었다면 포커스를 잡음
		if (mainMenuWidgetInstance)
		{
			InputMode.SetWidgetToFocus(mainMenuWidgetInstance->TakeWidget());
		}
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		
		PC->SetInputMode(InputMode);
	}
}
