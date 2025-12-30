// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Zombie/AnimNotify/AnimNotify_ZBasicAttack.h"

#include "Character/Zombie/ZombieCharacter.h"

void UAnimNotify_ZBasicAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		// 오너를 좀비 캐릭터로 캐스팅
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(MeshComp->GetOwner());
		if (Zombie)
		{
			// 좀비 클래스에 만들어둔 함수 호출!
			Zombie->BasicAttackHit();
		}
	}
}
