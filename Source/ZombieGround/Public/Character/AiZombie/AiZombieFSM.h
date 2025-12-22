// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AiZombieFSM.generated.h"

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	Idle UMETA(DisplayName = "IdleState"),
	Move UMETA(DisplayName = "MoveState"),
	Attack UMETA(DisplayName = "AttackState"),
	Damage UMETA(DisplayName = "DamageState") ,
	Die UMETA(DisplayName = "DieState")
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZOMBIEGROUND_API UAiZombieFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAiZombieFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	EZombieState zombieState = EZombieState::Idle;
	
	//컴포넌트 소유자 선언
	UPROPERTY()
	class AAiZombie* compOwner;
	
	UPROPERTY(EditAnywhere, Category=FSM)
	float idleDelayTime = 0.1;
	float currentTime = 0;
	void IdleState();
	
	UPROPERTY(EditAnywhere, Category=FSM)
	class AHumanCharacter* target;

	// [추가] 시야 감지 시 호출될 함수
	void OnTargetDetected(class AHumanCharacter* NewTarget);

	// [추가] 현재 목표로 하는 타겟 포인트 (패트롤용)
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class AActor* CurrentTargetPoint;

	// [추가] 맵에 있는 모든 타겟 포인트 목록
	UPROPERTY()
	TArray<class AActor*> AllTargetPoints;
	
	// [추가] 랜덤 타겟 포인트 선정 함수
	void FindRandomTargetPoint();
	
private:
	// 가장 가까운 인간을 찾아 반환하는 함수
	class AHumanCharacter* FindNearestTarget();
	
	//필요 속성 : 공격범위
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150;
	

	void MoveState();
	
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 1.0;
	
	void AttackState();
	
	
	//피격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 0.5f;
	
	// void DamageState();
	// void DieState();
	void ChangeState(EZombieState NewState);

	// [추가] 이동 중 타겟 재검색 주기 (0.5 ~ 1.0초 권장)
	float ReSearchRate = 1.0f; 
    
	// [추가] 재검색 타이머 누적 변수
	float ReSearchTimer = 0.0f;
	
	UPROPERTY()
	class AAIController* aiController;
};
