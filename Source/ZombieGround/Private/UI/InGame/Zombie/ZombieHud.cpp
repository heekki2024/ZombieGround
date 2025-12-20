// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Zombie/ZombieHud.h"

#include "Character/Zombie/ZombieCharacter.h"

void UZombieHud::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 1. 이 위젯을 소유한 플레이어 캐릭터를 가져옴
	ownerCharacter = Cast<AZombieCharacter>(GetOwningPlayerPawn());
	
	if (ownerCharacter)
	{
		ownerCharacter->zombieHUD = this;
	}
}
