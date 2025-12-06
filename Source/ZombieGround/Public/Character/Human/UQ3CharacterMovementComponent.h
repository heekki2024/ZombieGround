// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UQ3CharacterMovementComponent.generated.h"


// Unity의 MovementSettings 클래스 대응
USTRUCT(BlueprintType)
struct FQ3MovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 1500.0f; // Unity 단위보다 큼 (cm 단위)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Deceleration = 10.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZOMBIEGROUND_API UUQ3CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUQ3CharacterMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	
public:
	// --- Q3 Settings (Unity 파일 변수 매핑) ---
	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	FQ3MovementSettings GroundSettings;

	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	FQ3MovementSettings AirSettings;

	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	FQ3MovementSettings StrafeSettings;

	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	float Q3Friction = 6.0f;

	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	float Q3JumpForce = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	bool bAutoBunnyHop = false;

	UPROPERTY(EditAnywhere, Category = "Quake Movement")
	float Q3AirControl = 0.3f; // CPM Air Control
	

protected:
	// 언리얼의 속도 계산 함수 오버라이드
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	
	// // 점프 입력 체크 오버라이드 (버니합 용)
	// virtual void CheckJumpInput(float DeltaTime) override;
	
private:
	// Q3PlayerController.cs의 핵심 함수들 이식
	void Q3GroundMove(float DeltaTime);
	void Q3AirMove(float DeltaTime);
	void Q3AppyFriction(float t, float DeltaTime);
	void Q3Accelerate(FVector TargetDir, float TargetSpeed, float Accel, float DeltaTime);
	void CPM_AirControl(FVector TargetDir, float TargetSpeed, float DeltaTime);

	bool bJumpQueued = false;
};
