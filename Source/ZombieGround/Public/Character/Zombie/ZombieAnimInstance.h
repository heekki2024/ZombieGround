// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseAnimInstance.h"
#include "ZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UZombieAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	//Animation 소유 Pawn
	UPROPERTY()
	class AZombieCharacter* pawnOwner;
	
	//이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector velocity;
	
	//공중에 있는지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool isAir;
	
	//수평 이동 속력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float groundSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsRightClicking;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class ABaseWeaponActor* currentWeaponActor;
	
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	bool bHasCurrentWeapon;	
	
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	class UWeaponInstance* currentWeaponInstance;
};
