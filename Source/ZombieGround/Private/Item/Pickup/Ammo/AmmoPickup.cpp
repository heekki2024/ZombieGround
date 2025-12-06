// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/Ammo/AmmoPickup.h"

#include "Item/DataAsset/Ammo/AmmoDataAsset.h"
#include "Item/Instance/Ammo/AmmoInstance.h"


// Sets default values
AAmmoPickup::AAmmoPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// ammoInstance = CreateDefaultSubobject<UAmmoInstance>(TEXT("ammoInstance"));
	instance = CreateDefaultSubobject<UAmmoInstance>(TEXT("AmmoInstance"));
}

// Called when the game starts or when spawned
void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmoPickup::InitPickup(class UBaseDataAsset* initData)
{
	instance->InitInstance(initData);

	meshComp->SetStaticMesh(initData->pickupMesh);
}


//아이템 버릴시 초기화
void AAmmoPickup::LoadAmmoInstance(class UAmmoInstance* updatedAmmoInstance)
{
	// ammoInstance = updatedAmmoInstance;
	// meshComp->SetStaticMesh(ammoInstance->pickupMesh);
	
	instance = updatedAmmoInstance;
	meshComp->SetStaticMesh(instance->GetItemData<UBaseDataAsset>()->pickupMesh);

}

