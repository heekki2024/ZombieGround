// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiZombie/AiZombie.h"

#include "Character/AiZombie/AiZombieFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Character/Human/HumanCharacter.h"

// Sets default values
AAiZombie::AAiZombie()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	//이동 방향으로 회전 속성설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//엑터 컴포넌트이기 때문에 다른거의 자식으로 들어가지 않는다.
	fsm = CreateDefaultSubobject<UAiZombieFSM>(TEXT("FSM"));

	// [추가] PawnSensing 생성 및 설정
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 800.0f; // 시야 거리
	PawnSensing->SetPeripheralVisionAngle(60.0f); // 시야각 (Half Angle, 즉 전체 120도)

	// [추가] 스폰 시 자동으로 AI 컨트롤러 빙의
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAiZombie::BeginPlay()
{
	Super::BeginPlay();
	
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AAiZombie::OnSeePawn);
	}
}

void AAiZombie::OnSeePawn(APawn* Pawn)
{
	// 1. 감지된 대상이 인간인지 확인
	AHumanCharacter* Human = Cast<AHumanCharacter>(Pawn);
	// 2. FSM에 알림 (FSM 헤더에 함수 추가 필요)
	if (Human && fsm)
	{
		fsm->OnTargetDetected(Human);
	}
}


// Called every 조frame
void AAiZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAiZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


