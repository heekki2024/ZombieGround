// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BaseInteractable.h"


// Sets default values
ABaseInteractable::ABaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
}

// Called when the game starts or when spawned
void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseInteractable::OnInteract(AActor* InteractingActor)
{
	// 기본 구현: 단순 로그
	UE_LOG(LogTemp, Log, TEXT("%s interacted with %s"), 
		*InteractingActor->GetName(), *GetName());
}

FText ABaseInteractable::GetInteractText() const
{
	return InteractText;
}

