// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/BaseWeapon.h"
#include "BasePrimaryWeapon.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABasePrimaryWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePrimaryWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
