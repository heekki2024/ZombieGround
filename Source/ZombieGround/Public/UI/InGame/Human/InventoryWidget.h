// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/BaseUserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UInventoryWidget : public UBaseUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	
public:
	// 캐릭터의 인벤토리 컴포넌트와 연결하는 함수
	UFUNCTION(BlueprintCallable)
	void InitInventory(class UInventoryComponent* NewInventoryComp);
	
	// 슬롯 갱신 함수
	UFUNCTION()
	void RefreshInventoryUI();

private:
	UPROPERTY()
	class UInventoryComponent* InventoryComp;
    
	// UI에 이미 배치된 슬롯들을 캐싱해두는 배열
	UPROPERTY()
	TArray<class UConsumableSlot*> CreatedSlots;
	

	// // 생성할 슬롯 위젯 클래스 (WBP_InventorySlot 지정)
	// UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	// TSubclassOf<UConsumableSlot> SlotWidgetClass;

	
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* UGP_ConsumableSlots;
	
	
public:
	
	// 외부(HUD)에서 호출할 함수
	void OpenWidget();
	void CloseWidget();
    
	// 현재 열려있는지 확인하는 유틸리티
	bool IsOpen() const { return GetVisibility() == ESlateVisibility::Visible; }
};
