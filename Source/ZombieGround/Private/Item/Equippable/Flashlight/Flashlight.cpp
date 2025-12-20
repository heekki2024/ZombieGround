// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/Flashlight/Flashlight.h"

#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFlashlight::AFlashlight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 1. 메쉬 설정 (루트 컴포넌트)
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlashlightMesh"));
	RootComponent = mesh;
	mesh->SetCollisionProfileName(TEXT("NoCollision")); // 캐릭터에 부착 시 충돌 방지
	
	// Light Component 생성 및 부착 (기존처럼 스켈레탈 메쉬의 자식으로 유지)
	lightCone = CreateDefaultSubobject<USpotLightComponent>(TEXT("LightCone"));
	lightCone->SetupAttachment(mesh);

	// 조명의 기본 위치와 방향 설정 (모델링에 따라 회전값이 필요할 수 있음)
	// 보통 X축이 정면이므로 상황에 맞춰 회전시키세요.
	lightCone->SetRelativeLocation(FVector(10.0f, 0.0f, 0.0f)); 
	
	// -------------------------------------------------------
	// [요청하신 수치 적용]
	// -------------------------------------------------------
	lightCone->Intensity = 2000.0f;          // 밝기 (5000 -> 200)
	lightCone->AttenuationRadius = 2000.0f; // 도달 거리 (2000 -> 1300)
	lightCone->OuterConeAngle = 30.0f;      // 퍼지는 각도 (25 -> 30)
    
	// 그림자 및 기타 설정
	lightCone->SetCastShadows(true);  
	bIsLightOn = true; // 기본은 꺼진 상태
}

// Called when the game starts or when spawned
void AFlashlight::BeginPlay()
{
	Super::BeginPlay();
	lightCone->SetVisibility(bIsLightOn);
}

// Called every frame
void AFlashlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlashlight::ToggleLight()
{
	// 1. 상태 뒤집기 (On <-> Off)
	bIsLightOn = !bIsLightOn;

	// 2. 조명 가시성 설정
	lightCone->SetVisibility(bIsLightOn);

	// 3. 소리 재생 (사운드가 설정되어 있다면)
	if (toggleSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, toggleSound, GetActorLocation());
	}
}



