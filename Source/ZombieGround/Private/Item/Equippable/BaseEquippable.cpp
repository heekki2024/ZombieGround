// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/BaseEquippable.h"


// Sets default values
ABaseEquippable::ABaseEquippable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("mesh"));
	// 1. 메쉬 설정 (루트 컴포넌트)
	// 주의: 헤더 파일에 mesh 변수가 선언되어 있어야 합니다.
	RootComponent = mesh;
	mesh->SetCollisionProfileName(TEXT("NoCollision")); 
}

// Called when the game starts or when spawned
void ABaseEquippable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

