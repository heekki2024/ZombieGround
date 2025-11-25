// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/PrimaryWeapon/BasePrimaryWeapon.h"
#include "BaseAssaultRifle.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseAssaultRifle : public ABasePrimaryWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseAssaultRifle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
