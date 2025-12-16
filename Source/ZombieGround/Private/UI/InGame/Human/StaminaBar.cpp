// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Human/StaminaBar.h"

#include "Components/ProgressBar.h"

void UStaminaBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 에디터(UMG)에서 설정한 원래 색상을 가져와서 저장해둡니다.
	if (StaminaBar)
	{
		DefaultBarColor = StaminaBar->GetFillColorAndOpacity();
	}
}

void UStaminaBar::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar && MaxStamina > 0.0f)
	{
		// 1. 퍼센트 업데이트
		const float Percent = CurrentStamina / MaxStamina;
		StaminaBar->SetPercent(Percent);

		// 2. 색상 변경 로직
		if (CurrentStamina <= 10.0f)
		{
			// 스태미나가 10 이하일 때: 빨간색
			StaminaBar->SetFillColorAndOpacity(FLinearColor::Red);
		}
		else
		{
			// 스태미나가 충분할 때: 원래 저장해둔 색상으로 복구
			StaminaBar->SetFillColorAndOpacity(DefaultBarColor);
		}
	}
}
