// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/Weapon/WeaponActor/SecondaryWeapon/Pistol/BasePistolActor.h"

#include "Components/SpotLightComponent.h"
#include "Item/Instance/Weapon/BaseWeaponInstance.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABasePistolActor::ABasePistolActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

    
	// // 2. Light Component 생성 및 부착
	// lightCone = CreateDefaultSubobject<USpotLightComponent>(TEXT("LightCone"));
	// lightCone->SetupAttachment(mesh, TEXT("FlashlightSocket")); // 메쉬에 부착
	//
	// // 이제 RelativeLocation은 0으로 둡니다. (소켓 위치를 따라가므로)
	// lightCone->SetRelativeLocation(FVector::ZeroVector);
	// lightCone->SetRelativeRotation(FRotator::ZeroRotator);
	//
	// // 위치 설정
	// lightCone->SetRelativeLocation(FVector(10.0f, 0.0f, 0.0f)); 
	//
	// // -------------------------------------------------------
	// // [요청하신 수치 적용]
	// // -------------------------------------------------------
	// lightCone->Intensity = 2000.0f;          // 밝기 (5000 -> 200)
	// lightCone->AttenuationRadius = 1300.0f; // 도달 거리 (2000 -> 1300)
	// lightCone->OuterConeAngle = 30.0f;      // 퍼지는 각도 (25 -> 30)
 //    
	// // 그림자 및 기타 설정
	// lightCone->SetCastShadows(true);   
}

// Called when the game starts or when spawned
void ABasePistolActor::BeginPlay()
{
	Super::BeginPlay();
}



// Called every frame
void ABasePistolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void ABasePistolActor::ToggleLight()
// {
// 	// 1. 상태 뒤집기 (On <-> Off)
// 	
// 	weaponInstance->bIsLightOn = !weaponInstance->bIsLightOn;
//
// 	// 2. 조명 가시성 설정
// 	lightCone->SetVisibility(weaponInstance->bIsLightOn);
//
// 	// 3. 소리 재생 (사운드가 설정되어 있다면)
// 	if (toggleSound)
// 	{
// 		UGameplayStatics::PlaySoundAtLocation(this, toggleSound, GetActorLocation());
// 	}
// }

