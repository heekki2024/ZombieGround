// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "BaseInteractable.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseInteractable : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseInteractable();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
public:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* meshComp;
	
	
public:
	
	// 인터페이스 구현
	virtual void OnInteract(AActor* InteractingActor) override;
	virtual FText GetInteractText() const override;

	
protected:
	UPROPERTY(EditAnywhere, Category="Interaction")
	FText InteractText = FText::FromString("Press E to interact");
};
