// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/ZombieAIController.h"

#include "Character/Human/HumanCharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
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
	
	// 1. Perception 컴포넌트 생성 (생성자에 있으면 기존 코드 유지)
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    
	// 2. 시각 설정 (Sight Config)
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1500.0f;           // 감지 반경
	SightConfig->LoseSightRadius = 2000.0f;       // 시야 상실 반경
	SightConfig->PeripheralVisionAngleDegrees = 90.0f; // 시야각
    
	// 모든 세력 감지 (적, 중립, 아군)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	
	
	// [중요] 빙의 전에 자동으로 시작되지 않도록 막습니다.
	StateTreeAI->SetStartLogicAutomatically(false);

	// ensure we start the StateTree when we possess the pawn
	bStartAILogicOnPossess = true;

	// ensure we're attached to the possessed character.
	// this is necessary for EnvQueries to work correctly
	bAttachToPawn = true;
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// 델리게이트 바인딩
	if (PerceptionComponent)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnTargetDetected);
	}
}

void AZombieAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지된 액터가 인간인지 확인
	if (Cast<AHumanCharacter>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// 인간을 발견하면 거리와 상관없이 즉시 타겟으로 설정
			// 이를 통해 State Tree가 추격(Chase) 상태로 전환될 수 있게 함
			TargetActor = Actor;
			UE_LOG(LogTemp, Log, TEXT("Zombie spotted human: %s"), *Actor->GetName());
		}
		else if (TargetActor == Actor)
		{
			// 현재 추격 중인 타겟을 놓쳤다면 타겟 해제
			TargetActor = nullptr;
			UE_LOG(LogTemp, Log, TEXT("Zombie lost sight of human: %s"), *Actor->GetName());
		}
	}
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
	
	// [중요] 매 프레임 가장 가까운 적을 재검색합니다.
	// 이렇게 해야 추격 도중 옆에 더 가까운 인간이 지나가면 타겟을 그쪽으로 바꿉니다.
	// (성능 최적화가 필요하다면 0.2초~0.5초 간격으로 실행되도록 타이머를 걸 수도 있습니다.)
}

FGenericTeamId AZombieAIController::GetGenericTeamId() const
{
	// [핵심 변경] 내 변수가 아니라, 내가 조종 중인 좀비의 팀 ID를 리턴
	IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
	if (TeamAgent)
	{
		return TeamAgent->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

ETeamAttitude::Type AZombieAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	// 1. 상대방 팀 가져오기
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);
	if (!OtherTeamAgent) return ETeamAttitude::Neutral;

	FGenericTeamId OtherTeamId = OtherTeamAgent->GetGenericTeamId();

	// 2. 내 팀 가져오기 (위에서 만든 GetGenericTeamId 함수 호출 -> 좀비 캐릭터의 ID 반환)
	FGenericTeamId MyTeamId = GetGenericTeamId();

	// 3. 비교 로직
	if (OtherTeamId != MyTeamId)
	{
		return ETeamAttitude::Hostile; // 적이다!
	}

	return ETeamAttitude::Friendly;
}





