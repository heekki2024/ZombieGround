// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "AiZombie.generated.h"



UCLASS()
class ZOMBIEGROUND_API AAiZombie : public AZombieCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAiZombie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAiZombieFSM* fsm;
	
	virtual void OnDamageProcess(FVector hitDirection) override;

};
