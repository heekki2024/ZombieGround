// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/ZombieAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "World/SearchZone.h"


// Sets default values
AZombieAIController::AZombieAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create the StateTree AI Component
	// [수정] 블루프린트 충돌 방지를 위해 컴포넌트 내부 이름을 변경했습니다.
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("ZombieStateTreeComponent"));
	check(StateTreeAI);
	
	// [중요] 빙의 전에 자동으로 시작되지 않도록 막습니다.
	StateTreeAI->SetStartLogicAutomatically(false);

	// ensure we start the StateTree when we possess the pawn
	bStartAILogicOnPossess = true;

	// ensure we're attached to the possessed character.
	// this is necessary for EnvQueries to work correctly
	bAttachToPawn = true;
	
	// Perception 컴포넌트 생성
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
}

// Called when the game starts or when spawned
void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();
    
	// 1. 초기화
	CachedSearchZones.Empty();
	CachedWorldEntryPoints.Empty();

	// 2. 맵의 모든 SearchZone 찾기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASearchZone::StaticClass(), FoundActors);
    
	for (AActor* Actor : FoundActors)
	{
		if (ASearchZone* Zone = Cast<ASearchZone>(Actor))
		{
			// A. 존(건물) 액터 자체를 캐싱 (나중에 건물의 상태 등을 체크할 때 사용)
			CachedSearchZones.Add(Zone);

			// B. 존 내부의 진입점(EntryPoints)들을 월드 좌표로 변환하여 캐싱
			// 주의: ASearchZone의 EntryPoints 변수가 public이거나 GetEntryPoints() 함수가 필요합니다.
			if (!Zone->EntryPoints.IsEmpty())
			{
				FTransform ZoneTransform = Zone->GetActorTransform();
              
				for (const FVector& LocalPoint : Zone->EntryPoints)
				{
					// 로컬 좌표 -> 월드 좌표 변환
					FVector WorldPoint = ZoneTransform.TransformPosition(LocalPoint);
					CachedWorldEntryPoints.Add(WorldPoint);
				}
			}
		}
	}

	// 디버깅용 로그 (선택 사항)
	// UE_LOG(LogTemp, Log, TEXT("Cached %d Zones and %d Entry Points."), CachedSearchZones.Num(), CachedWorldEntryPoints.Num());
}

// Called every frame
void AZombieAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

