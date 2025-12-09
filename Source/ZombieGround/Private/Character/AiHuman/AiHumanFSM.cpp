// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AiHuman/AiHumanFSM.h"

#include "AIController.h"
#include "Character/AiHuman/AiHuman.h"


// Sets default values for this component's properties
UAiHumanFSM::UAiHumanFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAiHumanFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	compOwner = Cast<AAiHuman>(GetOwner());
	
}


// Called every frame
void UAiHumanFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
	if (!IsValid(compOwner)) return;

	// [수정 2] AIController가 없다면 매 프레임 다시 찾기를 시도합니다 (Lazy Initialization)
	// 인간이 스폰되고 컨트롤러가 빙의될 때까지 약간의 딜레이가 있기 때문입니다.
	if (!IsValid(aiController))
	{
		aiController = Cast<AAIController>(compOwner->GetController());
		// 아직도 컨트롤러가 없다면 아무것도 하지 않고 리턴 (오류 방지)
		if (!IsValid(aiController)) return; 
	}
}

