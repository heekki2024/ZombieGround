// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
#include "BasePistolActor.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABasePistolActor : public ABaseWeaponActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePistolActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
// public:
// 	// 빛을 쏘는 컴포넌트
// 	UPROPERTY(VisibleAnywhere, Category = "Components")
// 	class USpotLightComponent* lightCone;
// 	
//
// 	
// 	// 켜고 끌 때 재생할 효과음 (에디터에서 할당)
// 	UPROPERTY(EditAnywhere, Category = "Effects")
// 	class USoundBase* toggleSound;
// 	
// 	
// 	UFUNCTION()
// 	void ToggleLight();
};
