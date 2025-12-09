// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/BaseButtonWidget.h"

#include "Components/Button.h"

void UBaseButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 내부의 실제 버튼이 눌리면 -> 내 함수(HandleInternalClicked)를 실행해라
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UBaseButtonWidget::HandleInternalClicked);
	}
}

void UBaseButtonWidget::HandleInternalClicked()
{
	// 3. 외부로 신호 발송! (BroadCast)
	if (OnBaseButtonClick.IsBound())
	{
		OnBaseButtonClick.Broadcast();
	}
}
