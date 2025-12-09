// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MainMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Common/BaseButtonWidget.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼이 유효하면 클릭 이벤트와 함수를 연결
	if (StartButton)
	{
		StartButton->OnBaseButtonClick.AddDynamic(this, &UMainMenu::OnStartGameClicked);
		
		// // --- 추가된 부분 시작 ---
		// if (APlayerController* PC = GetOwningPlayer())
		// {
		// 	// 1. 마우스 커서 보이기
		// 	PC->SetShowMouseCursor(true);
		//
		// 	// 2. 입력 모드를 UI 전용으로 설정 (게임 조작 막기 & UI 포커스)
		// 	FInputModeUIOnly InputModeData;
		// 	InputModeData.SetWidgetToFocus(TakeWidget());
		// 	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
  //       
		// 	PC->SetInputMode(InputModeData);
		// }
	}
}


void UMainMenu::OnStartGameClicked()
{
	// 레벨 이름이 비어있지 않다면 해당 레벨 오픈
	if (!GameLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, GameLevelName);
	}
}
