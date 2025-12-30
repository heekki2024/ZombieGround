// Fill out your copyright notice in the Description page of Project Settings.


#include "World/SearchZone.h"

#include "NavigationSystem.h"
#include "Components/BillboardComponent.h"


// Sets default values
ASearchZone::ASearchZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 위치 정보를 갖기 위해 루트 컴포넌트 필요
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	// [추가] 에디터 아이콘(Billboard) 설정 코드
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		// 1. 루트 컴포넌트에 붙입니다. (루트가 이동하면 아이콘도 따라감)
		SpriteComponent->SetupAttachment(RootComponent);

		// 2. 엔진 기본 아이콘(타겟 포인트 모양)을 로드합니다.
		// 만약 다른 아이콘을 원하시면 경로를 변경하세요 (예: S_Note, S_Actor 등)
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(TEXT("/Engine/EditorResources/S_TargetPoint.S_TargetPoint"));
        
		if (IconFinder.Succeeded())
		{
			SpriteComponent->SetSprite(IconFinder.Object);
		}
        
		// 3. 아이콘 크기 조절 (필요 시 수정)
		SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f)); 
	}
#endif
}

void ASearchZone::BeginPlay()
{
	Super::BeginPlay();
}

float ASearchZone::GetNavPathDistanceToClosestEntryPoint(const FVector& ObserverLocation, UWorld* WorldContext) const
{
	if (EntryPoints.IsEmpty() || !WorldContext)
	{
		return FLT_MAX;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(WorldContext);
	if (!NavSys)
	{
		return FVector::Dist(ObserverLocation, GetActorLocation());
	}

	float MinPathLength = FLT_MAX;
	bool bFoundAnyPath = false;

	for (const FVector& LocalPoint : EntryPoints)
	{
		FVector TargetWorldPoint = TransformLocalToWorld(LocalPoint);
		double PathLength = 0.0;

		// [수정됨] 반환 타입이 ENavigationQueryResult::Type 입니다.
		ENavigationQueryResult::Type Result = NavSys->GetPathLength(ObserverLocation, TargetWorldPoint, PathLength);

		// [수정됨] 성공 여부 확인 상수도 ENavigationQueryResult::Success 입니다.
		if (Result == ENavigationQueryResult::Success)
		{
			if (PathLength < MinPathLength)
			{
				MinPathLength = (float)PathLength;
				bFoundAnyPath = true;
			}
		}
	}

	return bFoundAnyPath ? MinPathLength : FLT_MAX;
}

FVector ASearchZone::GetClosestEntryPointLocation(const FVector& ObserverLocation) const
{
	if (EntryPoints.IsEmpty())
	{
		return GetActorLocation();
	}

	float MinDistSquared = FLT_MAX;
	FVector BestLocation = GetActorLocation();

	for (const FVector& LocalPoint : EntryPoints)
	{
		FVector WorldPoint = TransformLocalToWorld(LocalPoint);
		float DistSq = FVector::DistSquared(ObserverLocation, WorldPoint);

		if (DistSq < MinDistSquared)
		{
			MinDistSquared = DistSq;
			BestLocation = WorldPoint;
		}
	}

	return BestLocation;
}

// FVector ASearchZone::GetRandomRoomLocation() const
// {
// 	if (RoomPoints.IsEmpty())
// 	{
// 		return GetActorLocation();
// 	}
//
// 	int32 RandomIndex = FMath::RandRange(0, RoomPoints.Num() - 1);
// 	return TransformLocalToWorld(RoomPoints[RandomIndex]);
// }

FVector ASearchZone::TransformLocalToWorld(const FVector& LocalPoint) const
{
	return GetActorTransform().TransformPosition(LocalPoint);
}
