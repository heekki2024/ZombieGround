// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ZGGameMode.h"

#include "Character/AiZombie/AiZombie.h"
#include "Character/Human/HumanCharacter.h"
#include "Kismet/GameplayStatics.h"

void AZGGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 첫 번째 플레이어 컨트롤러를 가져와서 설정
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetShowMouseCursor(false);
        
		FInputModeGameOnly GameInputMode;
		PC->SetInputMode(GameInputMode);
	}
	
	// 15초 뒤에 PickRandomHost 함수 실행
	if (InitialInfectionTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			InfectionTimerHandle, 
			this, 
			&AZGGameMode::PickRandomHuman, 
			InitialInfectionTime, 
			false
		);
	}
	
	if (GameStartSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GameStartSound);
	}
}

void AZGGameMode::PickRandomHuman()
{
	// 1. 맵에 있는 모든 HumanCharacter(플레이어 + AI)를 찾음
	TArray<AActor*> HumanActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHumanCharacter::StaticClass(), HumanActors);
	
	if (HumanActors.Num() == 0) return;
	
	int32 RandomIndex = FMath::RandRange(0, HumanActors.Num() - 1);
	AHumanCharacter* SelectedHuman = Cast<AHumanCharacter>(HumanActors[RandomIndex]);
    
	if (!SelectedHuman || !AiZombieClass) return;

	// 기존 컨트롤러 가져오기
	AController* OldController = SelectedHuman->GetController();
    
	// 위치값 저장
	FVector SpawnLoc = SelectedHuman->GetActorLocation();
	FRotator SpawnRot = SelectedHuman->GetActorRotation();
    
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 좀비 스폰
	AZombieCharacter* NewZombie = GetWorld()->SpawnActor<AZombieCharacter>(AiZombieClass, SpawnLoc, SpawnRot, SpawnParams);

	if (NewZombie)
	{
		// === [핵심 수정 부분] ===
        
		// 1. 플레이어인 경우: 컨트롤러를 그대로 유지 (화면 전환)
		if (OldController && OldController->IsPlayerController())
		{
			OldController->Possess(NewZombie);
			UE_LOG(LogTemp, Warning, TEXT("Player turned into Zombie!"));
		}
		// 2. AI인 경우: 인간 AI 컨트롤러는 버리고, 좀비 AI 컨트롤러를 새로 생성
		else
		{
			// 인간 컨트롤러는 좀비 로직을 모를 확률이 높으므로 UnPossess 후 제거 권장
			if (OldController)
			{
				OldController->UnPossess();
				OldController->Destroy(); // 기존 인간 뇌 제거
			}

			// 좀비 블루프린트에 설정된 'Default AI Controller'를 사용하여 자동으로 생성시키거나
			// 강제로 SpawnDefaultController()를 호출해 줍니다.
			NewZombie->SpawnDefaultController();
            
			UE_LOG(LogTemp, Warning, TEXT("AI Unit turned into Zombie! New AI Controller Spawned."));
		}

		// 기존 인간 삭제
		SelectedHuman->Destroy();

		if (OnZombieAppeared.IsBound())
		{
			// 이름 처리가 애매해지므로 단순히 알림만 보냄
			OnZombieAppeared.Broadcast(TEXT("Someone"));
		}
	}
}

float AZGGameMode::GetRemainingTime() const
{
	if (GetWorldTimerManager().IsTimerActive(InfectionTimerHandle))
	{
		return GetWorldTimerManager().GetTimerRemaining(InfectionTimerHandle);
	}
	return 0.0f;
}
