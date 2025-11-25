// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/WeaponPickup/SecondaryWeaponPickup/BaseSecondaryWeaponPickup.h"


// Sets default values
ABaseSecondaryWeaponPickup::ABaseSecondaryWeaponPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseSecondaryWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseSecondaryWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

