// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Glock.h"


// Sets default values
AGlock::AGlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

