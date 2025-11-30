// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/Weapon/WeaponActor/PrimaryWeapon/AssaultRifle/BaseAssaultRifle.h"


// Sets default values
ABaseAssaultRifle::ABaseAssaultRifle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseAssaultRifle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

