// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Human/HumanHUD.h"

#include "Character/Human/HumanCharacter.h"
#include "UI/InGame/Human/AmmoCounter.h"

void UHumanHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 이 위젯을 소유한 플레이어 캐릭터를 가져옴
	AHumanCharacter* ownerCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());

	if (ownerCharacter)
	{
		// 2. 캐릭터의 델리게이트에 내 함수(UpdateAmmoUI)를 등록(Bind)함
		// 이미 등록된 경우 중복 방지를 위해 Remove 후 Add 하기도 함
		// HumanChar->OnAmmoChanged.RemoveDynamic(this, &UHumanHUD::UpdateAmmoUI);
		ownerCharacter->OnCurrentAmmoChanged.AddDynamic(this, &UHumanHUD::UpdateCurrentAmmo);
		ownerCharacter->OnInventoryAmmoChanged.AddDynamic(this, &UHumanHUD::UpdateInventoryAmmo);
        
		// 3. (선택사항) 시작하자마자 현재 상태 한 번 갱신 (초기값 0/0 방지)
		ownerCharacter->BroadcastCurrentAmmoUpdate(); 
		ownerCharacter->BroadcastInventoryAmmoUpdate(); 

	}
}

void UHumanHUD::UpdateCurrentAmmo(int32 currentAmmo)
{
	// 자식 위젯이 유효한지 확인 후 값 전달
	if (WBP_AmmoCounter)
	{
		WBP_AmmoCounter->UpdateCurrentAmmoText(currentAmmo);
	}
}

void UHumanHUD::UpdateInventoryAmmo(int32 inventoryAmmo)
{
	// 자식 위젯이 유효한지 확인 후 값 전달
	if (WBP_AmmoCounter)
	{
		WBP_AmmoCounter->UpdateInventoryAmmoText(inventoryAmmo);
	}
}
