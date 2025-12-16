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

public:
	// WBP_HumanHUD 안에서 만든 위젯 이름과 변수명이 같아야 함 (meta = (BindWidget))
	UPROPERTY()
	class AHumanCharacter* ownerCharacter;
	
	UPROPERTY(meta = (BindWidget))
	class UAmmoCounter* WBP_AmmoCounter;
	
	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* WBP_Inventory;
	
	// 실제 생성된 위젯을 담을 포인터
	UPROPERTY(meta = (BindWidget))
	class UStaminaBar* WBP_StaminaBar;
	
	
public:
	// 외부(Character)에서 호출하는 함수
	void ToggleInventory();
	
	
};
