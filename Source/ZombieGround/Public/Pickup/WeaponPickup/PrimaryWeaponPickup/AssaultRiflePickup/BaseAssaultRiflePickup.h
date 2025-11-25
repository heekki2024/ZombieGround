// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/WeaponPickup/PrimaryWeaponPickup/BasePrimaryWeaponPickup.h"
#include "BaseAssaultRiflePickup.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseAssaultRiflePickup : public ABasePrimaryWeaponPickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseAssaultRiflePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
