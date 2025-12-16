// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/BaseUserWidget.h"
#include "StaminaBar.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UStaminaBar : public UBaseUserWidget
{
	GENERATED_BODY()
	
protected:
	// 위젯이 화면에 생성될 때 호출되는 함수 (초기화용)
	virtual void NativeConstruct() override;

protected:
	// [중요] meta = (BindWidget)은 에디터의 위젯 이름과 이 변수를 자동으로 연결해줍니다.
	// 에디터에서 만든 Progress Bar의 이름이 반드시 "StaminaProgressBar"여야 합니다.
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;
	
public:
	// 스태미나 바를 업데이트하는 함수
	UFUNCTION(BlueprintCallable)
	void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

private:
	// 원래 색상을 저장해둘 변수
	FLinearColor DefaultBarColor;
};
