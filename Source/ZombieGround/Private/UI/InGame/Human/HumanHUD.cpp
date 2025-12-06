// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Human/HumanHUD.h"

#include "Character/Human/HumanCharacter.h"
#include "UI/InGame/Human/AmmoCounter.h"
#include "UI/InGame/Human/InventoryWidget.h"

void UHumanHUD::NativeConstruct()
{
	Super::NativeConstruct();

	
	// 초기 상태 설정: 인벤토리 위젯의 Close 함수를 호출하여 초기화
	if (WBP_Inventory)
	{
		WBP_Inventory->CloseWidget();
	}
	
	// 1. 이 위젯을 소유한 플레이어 캐릭터를 가져옴
	ownerCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());

	if (ownerCharacter)
	{
		
		ownerCharacter->humanHud = this;

		// 2. 캐릭터의 델리게이트에 내 함수(UpdateAmmoUI)를 등록(Bind)함
		// 이미 등록된 경우 중복 방지를 위해 Remove 후 Add 하기도 함
		// HumanChar->OnAmmoChanged.RemoveDynamic(this, &UHumanHUD::UpdateAmmoUI);
		ownerCharacter->OnCurrentAmmoChanged.AddDynamic(WBP_AmmoCounter, &UAmmoCounter::UpdateCurrentAmmoText);
		ownerCharacter->OnInventoryAmmoChanged.AddDynamic(WBP_AmmoCounter, &UAmmoCounter::UpdateInventoryAmmoText);
        
		// 3. (선택사항) 시작하자마자 현재 상태 한 번 갱신 (초기값 0/0 방지)
		ownerCharacter->BroadcastCurrentAmmoUpdate(); 
		ownerCharacter->BroadcastInventoryAmmoUpdate(); 
		
		// 1. 자식 위젯(인벤토리 UI)이 잘 바인딩되었는지 확인
		if (WBP_Inventory)
		{
			if (ownerCharacter)
			{
				// 3. 캐릭터에서 인벤토리 컴포넌트 추출
				UInventoryComponent* InventoryComp = ownerCharacter->inventoryComponent;;

				// 4. 인벤토리 위젯 초기화 호출
				if (InventoryComp)
				{
					WBP_Inventory->InitInventory(InventoryComp);
				}
			}
		}
	}
}

void UHumanHUD::ToggleInventory()
{
	if (!WBP_Inventory) return;

	if (WBP_Inventory->IsOpen())
	{
		WBP_Inventory->CloseWidget();
		
		ownerCharacter->PC->SetShowMouseCursor(false);
        
		FInputModeGameOnly InputMode;
		ownerCharacter->PC->SetInputMode(InputMode);

	}
	else
	{
		
		WBP_Inventory->OpenWidget();
		
		ownerCharacter->PC->SetShowMouseCursor(true);
		
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget()); // 포커스 설정 (선택사항)
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		ownerCharacter->PC->SetInputMode(InputMode);
		
	}
}



