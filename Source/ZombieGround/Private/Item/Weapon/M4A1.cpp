// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/M4A1.h"


// Sets default values
AM4A1::AM4A1()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AM4A1::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AM4A1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

