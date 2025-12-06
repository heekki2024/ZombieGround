// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Human/InventoryWidget.h"

#include "Components/UniformGridPanel.h"
#include "Inventory/InventoryComponent.h"
#include "UI/InGame/Human/ConsumableSlot.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::InitInventory(UInventoryComponent* NewInventoryComp)
{
	if (!NewInventoryComp) return;
	
	InventoryComp = NewInventoryComp;

	// 1. 델리게이트 재설정
	// InventoryComp->OnInventoryUpdated.RemoveDynamic(this, &UInventoryWidget::RefreshInventoryUI);
	InventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshInventoryUI);

	// 2. [수정됨] 새로 생성하지 않고, GridPanel 안에 이미 배치된 슬롯들을 가져옴
	if (UGP_ConsumableSlots)
	{
		CreatedSlots.Empty();

		// GridPanel의 자식 개수만큼 반복 (이미 배치한 10개가 됨)
		int32 ChildrenCount = UGP_ConsumableSlots->GetChildrenCount();

		for (int32 i = 0; i < ChildrenCount; ++i)
		{
			// i번째 자식을 가져와서 InventoryItemSlot으로 형변환
			UWidget* ChildWidget = UGP_ConsumableSlots->GetChildAt(i);
			UConsumableSlot* ExistingSlot = Cast<UConsumableSlot>(ChildWidget);

			if (ExistingSlot)
			{
				// 슬롯 초기화 (컴포넌트 연결, 인덱스 부여)
				ExistingSlot->InitSlot(InventoryComp, i);
                
				// 나중에 Refresh할 때 쓰기 위해 배열에 저장
				CreatedSlots.Add(ExistingSlot);
			}
		}
	}

	// 3. 최초 UI 갱신
	RefreshInventoryUI();
}

void UInventoryWidget::RefreshInventoryUI()
{
	if (!InventoryComp) return;

	const TArray<FConsumableSlotData>& SlotsData = InventoryComp->GetConsumableSlots();

	// 캐싱해둔 슬롯(UI)들을 순회하며 데이터 업데이트
	for (int32 i = 0; i < CreatedSlots.Num(); ++i)
	{
		// UI 슬롯은 있지만 데이터가 없는 경우(인벤토리 크기가 더 작을 때) 등을 대비해 안전하게 처리
		if (CreatedSlots.IsValidIndex(i))
		{
			if (SlotsData.IsValidIndex(i))
			{
				// 데이터가 있으면 해당 데이터로 업데이트
				CreatedSlots[i]->UpdateSlot(SlotsData[i]);
			}
			else
			{
				// 데이터 범위를 벗어난 슬롯은 빈 슬롯 처리
				// FConsumableItemSlot()은 기본 생성자가 빈 슬롯이라고 가정
				CreatedSlots[i]->UpdateSlot(FConsumableSlotData()); 
			}
		}
	}
}

void UInventoryWidget::OpenWidget()
{
	// 1. 가시성 켜기
	SetVisibility(ESlateVisibility::Visible);

	// 2. (선택사항) 열릴 때 필요한 로직 수행
	// 예: 아이템 리스트 새로고침, 애니메이션 재생, 사운드 재생
	// RefreshInventory(); 
	// PlayAnimation(OpenAnim);
}

void UInventoryWidget::CloseWidget()
{
	// 1. 가시성 끄기
	SetVisibility(ESlateVisibility::Collapsed);

	// 2. (선택사항) 닫힐 때 로직
	// PlaySound(CloseSound);
}