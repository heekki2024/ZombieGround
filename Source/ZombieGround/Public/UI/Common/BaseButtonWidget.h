// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUserWidget.h"
#include "BaseButtonWidget.generated.h"

// 1. 델리게이트 선언 (이벤트 타입 정의)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBaseButtonClicked);

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UBaseButtonWidget : public UBaseUserWidget
{
	GENERATED_BODY()
	
public: 
	virtual void NativeConstruct() override;

	// 2. 외부(메인메뉴)에서 접근할 이벤트 변수 선언
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBaseButtonClicked OnBaseButtonClick;
	
	// 위젯 블루프린트 안에 들어있는 실제 버튼 (이름이 같아야 함)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UButton* Button; 

	// 내부 버튼 클릭을 처리할 함수
	UFUNCTION()
	void HandleInternalClicked();
};
