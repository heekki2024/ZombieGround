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
	class AZombieCharacter* compOwner;
	
	UPROPERTY(EditAnywhere, Category=FSM)
	float idleDelayTime = 0.1;
	float currentTime = 0;
	void IdleState();
	
	UPROPERTY(EditAnywhere, Category=FSM)
	class AHumanCharacter* target;
	
	//필요 속성 : 공격범위
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 205;
	

	void MoveState();
	
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 1.0;
	
	void AttackState();
	
	
	//피격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 0.5f;
	
	void DamageState();
	void DieState();


	
	UPROPERTY()
	class AAIController* ai;
};
