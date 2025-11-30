// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/Weapon/SecondaryWeaponPickup/PistolPickup/BasePistolPickup.h"


// Sets default values
ABasePistolPickup::ABasePistolPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABasePistolPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasePistolPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

