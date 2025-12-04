// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/BaseUserWidget.h"
#include "HumanHUD.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UHumanHUD : public UBaseUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 델리게이트에 바인딩할 함수 (UFUNCTION 필수)
	UFUNCTION()
	void UpdateCurrentAmmo(int32 currentAmmo);
	
	UFUNCTION()
	void UpdateInventoryAmmo(int32 inventoryAmmo);

protected:
	// WBP_HumanHUD 안에서 만든 위젯 이름과 변수명이 같아야 함 (meta = (BindWidget))
	UPROPERTY(meta = (BindWidget))
	class UAmmoCounter* WBP_AmmoCounter;
};
