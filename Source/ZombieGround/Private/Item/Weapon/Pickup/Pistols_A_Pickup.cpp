// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Pickup/Pistols_A_Pickup.h"


// Sets default values
APistols_A_Pickup::APistols_A_Pickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APistols_A_Pickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APistols_A_Pickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

