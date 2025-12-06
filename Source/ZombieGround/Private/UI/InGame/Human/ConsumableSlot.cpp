// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Human/ConsumableSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Inventory/InventoryComponent.h"
#include "Item/DataAsset/BaseDataAsset.h"
#include "Item/DataAsset/Ammo/AmmoDataAsset.h"

void UConsumableSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UConsumableSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭인지 확인
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (InventoryComp && SlotIndex >= 0)
		{
			// 인벤토리 컴포넌트에 "나(이 인덱스)를 드롭해줘"라고 요청
			InventoryComp->DropConsumableByIndex(SlotIndex);
			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UConsumableSlot::InitSlot(UInventoryComponent* InComp, int32 InIndex)
{
	InventoryComp = InComp;
	SlotIndex = InIndex;
}

void UConsumableSlot::UpdateSlot(const struct FConsumableSlotData& slotData)
{
	if (slotData.IsEmpty())
	{
		// 1. 비어있으면 Collapsed (보이지 않음)
		if (ItemVerticalBox)
		{
			ItemVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		// 2. 아이템이 있으면 Visible (보임)
		if (ItemVerticalBox)
		{
			ItemVerticalBox->SetVisibility(ESlateVisibility::Visible);
		}

		// 수량 텍스트 설정
		if (ItemCurrentQuantity)
		{
			ItemCurrentQuantity->SetText(FText::AsNumber(slotData.itemInstance->currentQuantity));
		}
		
		// 이미지 설정
		if (ItemImage && slotData.itemInstance)
		{
			if (UAmmoDataAsset* ammoData = slotData.GetInstance<UAmmoInstance>()->GetItemData<UAmmoDataAsset>())
			
			if (ammoData && ammoData->icon) // itemIcon은 DataAsset에 있는 UTexture2D 변수라고 가정
			{
				ItemImage->SetBrushFromTexture(ammoData->icon);
			}
		}
	}
}
