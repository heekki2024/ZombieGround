// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseAnimInstance.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "HumanAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGROUND_API UHumanAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	//Animation 소유 Pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class AHumanCharacter* humanCharacter;
	
	// //이동 속도
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// FVector velocity;
	
	//공중에 있는지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool isAir;
	
	//수평 이동 속력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float groundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float direction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsRightClicking;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsCrouching;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class ABaseWeaponActor* currentWeaponActor;
	
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	bool bHasCurrentWeapon;	
	
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	class UBaseWeaponInstance* currentWeaponInstance;
	
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	FPlayerAnimData playerAnimData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	FPlayerAnimData defaultPlayerAnimData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float deltaYaw;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float deltaPitch;

	// [추가] ADS 전환 시간 (애니메이션 블루프린트용)
	UPROPERTY(BlueprintReadOnly, Category="Animation")
	float LowReadyToAdsTime = 0.2f;

	UPROPERTY(BlueprintReadOnly, Category="Animation")
	float AdsToLowReadyTime = 0.2f;
};
