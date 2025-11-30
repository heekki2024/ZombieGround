// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEquippable.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseEquippable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseEquippable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:

	UFUNCTION()
	virtual void OnLeftClickPressed(){}
	UFUNCTION()
	virtual void OnLeftClickReleased(){}
	// virtual void OnRightClickReleased(AHumanCharacter* OwnerCharacter) {}
	UFUNCTION()
	virtual void OnRightClickPressed(){}
	UFUNCTION()
	virtual void OnRightClickReleased(){}
};
