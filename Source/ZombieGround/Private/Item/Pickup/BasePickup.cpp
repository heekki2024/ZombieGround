// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/BasePickup.h"


// Sets default values
ABasePickup::ABasePickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 1. 틱 기능 자체는 살려두되 (나중에 켤 수 있음)
	PrimaryActorTick.bCanEverTick = true;

	// 2. 게임 시작 시에는 꺼둔 상태로 시작함 (핵심)
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 1. 컴포넌트 생성 (이름은 "MeshComp"로 지정)
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));

	// 2. 루트 컴포넌트로 설정
	// (만약 이 액터의 기준이 이 메쉬라면 RootComponent로 설정)
	RootComponent = meshComp;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// 3. 찾는데 성공했다면 할당하기
	if (CubeMeshAsset.Succeeded())
	{
		meshComp->SetStaticMesh(CubeMeshAsset.Object);
	}
	
	// 1. 물리 시뮬레이션 켜기
	meshComp->SetSimulatePhysics(true);

	// 2. (중요) 중력 켜기 (보통 기본값이 true지만 확실하게 하기 위해)
	meshComp->SetEnableGravity(true);

	// 3. (권장) 물리용 콜리전 프리셋으로 변경
	// 'BlockAll'이나 'PhysicsActor'를 써야 땅을 뚫고 떨어지지 않습니다.
	meshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	
	meshComp->SetCollisionObjectType(ECC_GameTraceChannel2);
}

// Called when the game starts or when spawned
void ABasePickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


