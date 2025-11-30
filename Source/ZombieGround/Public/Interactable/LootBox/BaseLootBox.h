// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/BaseInteractable.h"
#include "BaseLootBox.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseLootBox : public ABaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseLootBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

public:
	virtual void OnInteract(AActor* InteractingActor) override;
	
	// [변경] 단일 변수 -> 배열(Array)
	// 이제 에디터에서 + 버튼을 눌러 AK, M4, Sniper 등 여러 개를 등록할 수 있습니다.
	UPROPERTY(EditAnywhere, Category = "Loot")
	TArray<class UWeaponDataAsset*> weaponLootTable; 
    
	// 스폰할 픽업 액터 클래스 (이건 하나만 있어도 됨, 껍데기니까)
	UPROPERTY(EditAnywhere, Category = "Loot")
	TSubclassOf<class ABaseWeaponPickup> weaponPickupClass;
	
	UFUNCTION()
	void SpawnPrimaryWeapon();
};
