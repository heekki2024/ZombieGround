// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Human/HumanCharacter.h"
#include "AiHuman.generated.h"

UCLASS()
class ZOMBIEGROUND_API AAiHuman : public AHumanCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAiHuman();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:

};
