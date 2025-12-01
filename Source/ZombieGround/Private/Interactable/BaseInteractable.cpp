// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BaseInteractable.h"

#include "Character/Human/HumanCharacter.h"


// Sets default values
ABaseInteractable::ABaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	
	// meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 물리 및 쿼리 충돌 켜기
	// meshComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 객체 타입 설정
	// meshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // 모든 채널 Block
	// //채널 설정해줘야 아웃라인됨 (중요***)
	// meshComp->SetCollisionObjectType(ECC_GameTraceChannel2);
	
	// 1. 물리 시뮬레이션 켜기
	meshComp->SetSimulatePhysics(true);

	// 2. (중요) 중력 켜기 (보통 기본값이 true지만 확실하게 하기 위해)
	meshComp->SetEnableGravity(true);

	// 3. (권장) 물리용 콜리전 프리셋으로 변경
	// 'BlockAll'이나 'PhysicsActor'를 써야 땅을 뚫고 떨어지지 않습니다.
	meshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	
	meshComp->SetCollisionObjectType(ECC_GameTraceChannel4);
	meshComp->SetGenerateOverlapEvents(true);
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

void ABaseInteractable::OnInteract_Implementation(AHumanCharacter* interactingCharacter)
{
	IInteractInterface::OnInteract_Implementation(interactingCharacter);
	// 기본 구현: 단순 로그
	UE_LOG(LogTemp, Log, TEXT("%s interacted with %s"), 
		*interactingCharacter->GetName(), *GetName());
	
}

// FText ABaseInteractable::GetInteractText() const
// {
// 	return InteractText;
// }

