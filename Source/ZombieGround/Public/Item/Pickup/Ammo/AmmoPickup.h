// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Pickup/BasePickup.h"
#include "AmmoPickup.generated.h"

UCLASS()
class ZOMBIEGROUND_API AAmmoPickup : public ABasePickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY()
	class UAmmoInstance* ammoInstance;
	

public:
	UFUNCTION()
	void InitPickup(class UAmmoDataAsset* initData);

	UFUNCTION(BlueprintCallable)
	void LoadAmmoInstance(class UAmmoInstance* updatedAmmoInstance);
};
