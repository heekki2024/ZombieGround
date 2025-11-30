// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/LootBox/BaseLootBox.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Pickup/Weapon/BaseWeaponPickup.h"

// Sets default values
ABaseLootBox::ABaseLootBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseLootBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseLootBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseLootBox::OnInteract(AActor* InteractingActor)
{
	Super::OnInteract(InteractingActor);
}

void ABaseLootBox::SpawnPrimaryWeapon()
{
	// 1. 방어 코드: 배열이 비어있으면 아무것도 안 함 (크래시 방지)
	if (weaponLootTable.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LootTable is empty in %s"), *GetName());
		return;
	}
	
	if (!weaponPickupClass) {return;}
	UWeaponDataAsset* selectedWeaponDA;
	// 2. 랜덤 뽑기: 0 ~ (배열개수 - 1) 사이의 랜덤 숫자 생성
	while (true)
	{
		int32 randomIndex = FMath::RandRange(0, weaponLootTable.Num() - 1);
		if (weaponLootTable[randomIndex]->weaponSlot == EWeaponSlot::Primary)
		{
			// 3. 당첨된 데이터 에셋 가져오기
			selectedWeaponDA = weaponLootTable[randomIndex];
			break;
		}
	}
	// 4. 스폰 위치 설정 (상자보다 약간 위)
	FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100.0f);
	FRotator SpawnRotation = GetActorRotation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 5. 픽업 액터(껍데기) 스폰
	ABaseWeaponPickup* NewPickup = GetWorld()->SpawnActor<ABaseWeaponPickup>(
		weaponPickupClass, 
		SpawnLocation, 
		SpawnRotation, 
		SpawnParams
	);
	
	// 6. 데이터 주입 (중요: 여기서 랜덤으로 뽑힌 AK나 M4 정보를 넣어줌)
	if (NewPickup)
	{
		NewPickup->InitPickup(selectedWeaponDA);
	}
}

