// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/Weapon/BaseWeaponPickup.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Instance/Weapon/WeaponInstance.h"
// Sets default values
ABaseWeaponPickup::ABaseWeaponPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	weaponInstance = CreateDefaultSubobject<UWeaponInstance>(TEXT("WeaponInstance"));
}

// Called when the game starts or when spawned
void ABaseWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeaponPickup::InitPickup(class UWeaponDataAsset* initData)
{
	weaponInstance->InitWeaponInstance(initData);

	meshComp->SetStaticMesh(initData->weaponPickupMesh);
}

void ABaseWeaponPickup::LoadWeaponInstance(class UWeaponInstance* updatedWeaponInstance)
{
	weaponInstance = updatedWeaponInstance;
}



