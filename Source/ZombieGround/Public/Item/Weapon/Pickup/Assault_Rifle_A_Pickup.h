// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "Assault_Rifle_A_Pickup.generated.h"

UCLASS()
class ZOMBIEGROUND_API AAssault_Rifle_A_Pickup : public ABasePickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAssault_Rifle_A_Pickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
