// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/BasePickup.h"
#include "BaseWeaponPickup.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseWeaponPickup : public ABasePickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWeaponPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	// TSubclassOf를 사용하면 에디터에서 BaseWeapon을 상속받은 블루프린트만 선택할 수 있게 필터링됩니다.

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseWeapon> WeaponToSpawn;
	
public:
	UFUNCTION(BlueprintCallable)
	TSubclassOf<class ABaseWeapon> GetWeaponToSpawn() const {return WeaponToSpawn;}
};
