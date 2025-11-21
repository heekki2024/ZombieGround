// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Weapon/Pistols_A.h"


// Sets default values
APistols_A::APistols_A()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APistols_A::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APistols_A::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

