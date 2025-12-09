// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/BaseUserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UMainMenu : public UBaseUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnStartGameClicked();
	
	// meta = (BindWidget)은 에디터의 버튼과 이 변수를 이름으로 자동 연결해줍니다.
	UPROPERTY(meta = (BindWidget))
	class UBaseButtonWidget* StartButton;
    
	// 이동할 레벨 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName GameLevelName = FName("Lvl_ThirdPerson");
};
