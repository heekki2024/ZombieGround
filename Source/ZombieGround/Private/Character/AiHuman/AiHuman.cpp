// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiHuman/AiHuman.h"


// Sets default values
AAiHuman::AAiHuman()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAiHuman::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAiHuman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAiHuman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

