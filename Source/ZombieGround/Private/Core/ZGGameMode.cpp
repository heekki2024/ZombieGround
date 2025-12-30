// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ZGGameMode.h"

#include "Character/Human/HumanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	AHumanCharacter* selectedHuman = Cast<AHumanCharacter>(HumanActors[RandomIndex]);
	
	selectedHuman->OnInfected();
	// // 기존 컨트롤러 가져오기
	// AController* selectedHumanController = SelectedHuman->GetController();
	// UWorld* World = GetWorld();
	//
	// // 필수 요소가 없으면 중단
	// if (!selectedHumanController || !World) 
	// {
	// 	return; 
	// }
	//
	// // 2. [추출] 인간의 현재 운동 상태 저장
	// FVector LastVelocity = SelectedHuman->GetVelocity();
	// EMovementMode LastMode = SelectedHuman->GetCharacterMovement()->MovementMode;
	// FVector SpawnLocation = SelectedHuman->GetActorLocation();
	// FRotator SpawnRotation = SelectedHuman->GetActorRotation();
	//
	// // 3. 스폰할 클래스 결정 (Player vs AI)
	// // 삼항 연산자를 쓰거나 if문으로 'ClassToSpawn' 변수에만 할당합니다.
	// TSubclassOf<AZombieCharacter> TargetClass = nullptr;
	//
	// if (selectedHumanController->IsA(APlayerController::StaticClass()))
	// {
	// 	TargetClass = ZombieClassToSpawn;
	// }
	// else
	// {
	// 	// AAiZombie가 AZombieCharacter를 상속받았다면 이렇게 하나로 퉁칠 수 있습니다.
	// 	TargetClass = AIZombieClassToSpawn; 
	// }
	//
	// // 클래스가 비어있으면 중단
	// if (!TargetClass) return;
	//
	// // 4. 좀비 스폰 (한 번만 작성)
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// // 부모 클래스인 AZombieCharacter로 받아도 자식 기능(AI 등)은 정상 작동합니다.
	// AZombieCharacter* NewZombie = World->SpawnActor<AZombieCharacter>(TargetClass, SpawnLocation, SpawnRotation, SpawnParams);
	//
	// if (NewZombie)
	// {
	// 	// 5. [중요] 빙의 (영혼 옮기기)
	// 	// 이걸 안 하면 플레이어가 좀비를 조종할 수 없습니다.
	// 	selectedHumanController->Possess(NewZombie);
	//
	// 	// 6. 운동량 주입
	// 	UCharacterMovementComponent* ZombieCMC = NewZombie->GetCharacterMovement();
	// 	if (ZombieCMC)
	// 	{
	// 		// 이동 모드 동기화
	// 		if (LastMode == MOVE_Falling || LastMode == MOVE_Flying)
	// 		{
	// 			ZombieCMC->SetMovementMode(LastMode);
	// 		}
	//
	// 		// 속도 주입
	// 		ZombieCMC->Velocity = LastVelocity;
	// 		ZombieCMC->UpdateComponentVelocity();
	// 	}
	// }
	//
	// // 7. 인간 파괴
	// SelectedHuman->Destroy();
	//
}

float AZGGameMode::GetRemainingTime() const
{
	if (GetWorldTimerManager().IsTimerActive(InfectionTimerHandle))
	{
		return GetWorldTimerManager().GetTimerRemaining(InfectionTimerHandle);
	}
	return 0.0f;
}
