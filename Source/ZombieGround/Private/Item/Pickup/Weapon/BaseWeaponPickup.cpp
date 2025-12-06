// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/Weapon/BaseWeaponPickup.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Instance/Weapon/WeaponInstance.h"
// Sets default values
ABaseWeaponPickup::ABaseWeaponPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	instance = CreateDefaultSubobject<UWeaponInstance>(TEXT("WeaponInstance"));
	
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


//pickup쪽에서는 자세한 ammo,weapon dataasset내용을 몰라도 됨으로 casting하지 않아도 됨
void ABaseWeaponPickup::InitPickup(class UBaseDataAsset* initData)
{
	instance->InitInstance(initData);
	meshComp->SetStaticMesh(initData->pickupMesh);
}

void ABaseWeaponPickup::LoadWeaponInstance(class UWeaponInstance* updatedWeaponInstance)
{
	instance = updatedWeaponInstance;
	meshComp->SetStaticMesh(instance->pickupMesh);
}



