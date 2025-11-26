// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/GrenadeProjectile/BaseGrenadeProjectile.h"


// Sets default values
ABaseGrenadeProjectile::ABaseGrenadeProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGrenadeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

