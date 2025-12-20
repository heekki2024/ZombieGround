// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ZGGameMode.generated.h"

/**
 * 
 */

// 델리게이트: 누군가 좀비가 되었음을 알림 (UI 표시용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieAppeared, FString, ZombieName);

UCLASS()
class ZOMBIEGROUND_API AZGGameMode : public AGameMode
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
public:	
	// 카운트다운 종료 시 실행될 함수
	void PickRandomHuman();
	
	
	// 변신시킬 좀비 클래스 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "GameRule")
	TSubclassOf<class AZombieCharacter> ZombieClassToSpawn;
	
	
	// 변신시킬 Ai좀비 클래스 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "GameRule")
	TSubclassOf<class AAiZombie> AIZombieClassToSpawn;
	
	
	// 감염까지 남은 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRule")
	float InitialInfectionTime = 23.0f;
	
	// UI에서 남은 시간을 가져가기 위한 함수
	UFUNCTION(BlueprintCallable, Category = "GameRule")
	float GetRemainingTime() const;
	
	// 좀비 출현 알림 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GameRule")
	FOnZombieAppeared OnZombieAppeared;
	
private:
	FTimerHandle InfectionTimerHandle;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* GameStartSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class AHumanCharacter* Player;
};
