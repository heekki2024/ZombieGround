// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Weapon/Assault_Rifle_A.h"


// Sets default values
AAssault_Rifle_A::AAssault_Rifle_A()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAssault_Rifle_A::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAssault_Rifle_A::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

