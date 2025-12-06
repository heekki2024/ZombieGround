// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/BaseUserWidget.h"
#include "ConsumableSlot.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UConsumableSlot : public UBaseUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;
    
	// 우클릭 감지
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ItemVerticalBox; // 아이템 내용물 컨테이너

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCurrentQuantity;

private:
	UPROPERTY()
	class UInventoryComponent* InventoryComp;

	int32 SlotIndex = -1;
	
public:
	// 초기화 함수
	void InitSlot(UInventoryComponent* InComp, int32 InIndex);
    
	// UI 업데이트 함수
	void UpdateSlot(const struct FConsumableSlotData& SlotData);

};
