// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/UQ3CharacterMovementComponent.h"

#include "GameFramework/Character.h"


// Sets default values for this component's properties
UUQ3CharacterMovementComponent::UUQ3CharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
	// 언리얼 기본 물리 설정 무시 (우리가 직접 제어하기 위해)
	BrakingDecelerationWalking = 0.0f; 
	GroundFriction = 0.0f; 
	AirControl = 0.0f; 
	// [중요] 서핑을 위해 공중 감속 제거 (기본값이 0이 아닐 수 있음)
	BrakingDecelerationFalling = 0.0f;
	
	// 기본값 설정 (Unity 파일 값 참조하여 스케일 조정)
	// 언리얼은 cm 단위이므로 Unity 값에 약 100을 곱하거나 적절히 튜닝해야 함
	GroundSettings.MaxSpeed = 700.0f; 
	GroundSettings.Acceleration = 14.0f; // 배수 로직이므로 작을 수 있음
	GroundSettings.Deceleration = 10.0f;

	AirSettings.MaxSpeed = 700.0f;
	AirSettings.Acceleration = 2.0f;
	AirSettings.Deceleration = 2.0f;

	StrafeSettings.MaxSpeed = 32.0f; // [수정] 100 -> 32 (퀘이크 표준값 30~32)
	StrafeSettings.Acceleration = 10.0f; // [수정] 50 -> 10 (너무 크면 확 튑니다)
}


// Called when the game starts
void UUQ3CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUQ3CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
	
}

void UUQ3CharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid,
	float BrakingDeceleration)
{
	// 1. 입력 벡터 가져오기
	FVector InputVector = GetLastInputVector();
	
	// 2. 점프 큐 처리 (버니합)
	if (bAutoBunnyHop && CharacterOwner && CharacterOwner->bPressedJump)
	{
		bJumpQueued = true;
	}

	// 3. 상태에 따른 이동 로직 분기
	if (MovementMode == MOVE_Walking)
	{
		Q3GroundMove(DeltaTime);
	}
	else if (MovementMode == MOVE_Falling)
	{
		Q3AirMove(DeltaTime);
	}
	else
	{
		// 수영, 사다리 등 다른 모드는 기본 로직 사용
		Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
	}	
	
}


void UUQ3CharacterMovementComponent::Q3GroundMove(float DeltaTime)
{
	// 점프 큐가 있으면 마찰력 무시 (속도 보존)
	if (!bJumpQueued)
	{
		Q3AppyFriction(1.0f, DeltaTime);
	}
	else
	{
		Q3AppyFriction(0.0f, DeltaTime);
	}

	FVector InputVector = GetLastInputVector();
	FVector WishDir = InputVector.GetSafeNormal(); // 정규화
	FVector WishDir2D = WishDir;
	WishDir2D.Z = 0; // 수평 이동만 고려

	float WishSpeed = WishDir2D.Size() * GroundSettings.MaxSpeed;
	
	Q3Accelerate(WishDir, WishSpeed, GroundSettings.Acceleration, DeltaTime);

	// 중력 초기화 (언리얼은 PhysWalking에서 바닥에 붙여주므로 -Gravity 설정 불필요할 수 있으나, 관성 유지를 위해)
	// Unity 코드: m_PlayerVelocity.y = -m_Gravity * Time.deltaTime;
	// 언리얼에서는 Z축 속도를 0으로 두거나 미세하게 누르는 힘을 줍니다.
	Velocity.Z = 0;

	// 점프 처리
	if (bJumpQueued)
	{
		DoJump(false); // 언리얼 기본 점프 함수 호출
		Velocity.Z = Q3JumpForce; // 점프 힘 적용
		bJumpQueued = false;
	}
}

void UUQ3CharacterMovementComponent::Q3AirMove(float DeltaTime)
{
	FVector InputVector = GetLastInputVector();
	FVector WishDir = InputVector.GetSafeNormal();

	float WishSpeed = WishDir.Size() * AirSettings.MaxSpeed;
	
	float Accel = 0.f;

	// CPM Air Control 로직
	// Unity: if (Vector3.Dot(m_PlayerVelocity, wishdir) < 0)
	float CurrentSpeedDir = FVector::DotProduct(Velocity, WishDir);
	
	if (CurrentSpeedDir < 0)
	{
		Accel = AirSettings.Deceleration;
	}
	else
	{
		Accel = AirSettings.Acceleration;
	}

	// Strafe 체크 (좌우 입력만 있을 때)
	// 언리얼 InputVector: X(앞뒤), Y(좌우). 좌표계 회전에 따라 다르니 로컬 기준 확인 필요.
	// 보통 GetLastInputVector는 월드 기준이므로 내적 등을 써야 하지만, 
	// 여기서는 단순히 WishSpeed가 높을 때 제한하는 로직으로 처리.
	
	if (WishDir.Size() > 0.0f && WishSpeed > StrafeSettings.MaxSpeed)
	{
		// 스트레이프 가속도 적용
		Accel = StrafeSettings.Acceleration;
	}

	Q3Accelerate(WishDir, WishSpeed, Accel, DeltaTime);

	if (Q3AirControl > 0)
	{
		CPM_AirControl(WishDir, WishSpeed, DeltaTime);
	}

	// 중력 적용 (언리얼 Z-Up)
	Velocity.Z += GetGravityZ() * DeltaTime;
}

void UUQ3CharacterMovementComponent::Q3AppyFriction(float t, float DeltaTime)
{
	FVector Vec = Velocity;
	Vec.Z = 0; // 수평 속도만
	
	float Speed = Vec.Size();
	float Drop = 0.0f;

	// 땅에 있을 때만
	if (MovementMode == MOVE_Walking)
	{
		// Unity: control = speed < Deceleration ? Deceleration : speed;
		float Control = (Speed < GroundSettings.Deceleration) ? GroundSettings.Deceleration : Speed;
		Drop = Control * Q3Friction * DeltaTime * t;
	}

	float NewSpeed = Speed - Drop;
	if (NewSpeed < 0) NewSpeed = 0;
	
	if (Speed > 0) NewSpeed /= Speed;

	Velocity.X *= NewSpeed;
	Velocity.Y *= NewSpeed;
	// Z축은 마찰력 적용 안 함
}

void UUQ3CharacterMovementComponent::Q3Accelerate(FVector TargetDir, float TargetSpeed, float Accel, float DeltaTime)
{
	float CurrentSpeed = FVector::DotProduct(Velocity, TargetDir);
	float AddSpeed = TargetSpeed - CurrentSpeed;

	if (AddSpeed <= 0) return;

	// Unity: accelspeed = accel * Time.deltaTime * targetSpeed;
	float AccelSpeed = Accel * DeltaTime * TargetSpeed; 

	if (AccelSpeed > AddSpeed)
	{
		AccelSpeed = AddSpeed;
	}

	Velocity += TargetDir * AccelSpeed;
}

void UUQ3CharacterMovementComponent::CPM_AirControl(FVector TargetDir, float TargetSpeed, float DeltaTime)
{
	// 전진/후진 입력이 없거나 속도가 너무 느리면 리턴
	// 여기서는 입력 벡터와 내적을 통해 횡이동인지 판별하는 로직이 필요하나 단순화
	if (FMath::Abs(TargetSpeed) < KINDA_SMALL_NUMBER || TargetSpeed < 1.0f) 
	{
		return;
	}

	float ZSpeed = Velocity.Z;
	Velocity.Z = 0;
	float Speed = Velocity.Size();
	Velocity.Normalize();

	float Dot = FVector::DotProduct(Velocity, TargetDir);
	float K = 32.0f;
	K *= Q3AirControl * Dot * Dot * DeltaTime;

	// 방향 전환
	if (Dot > 0)
	{
		Velocity.X = Velocity.X * Speed + TargetDir.X * K;
		Velocity.Y = Velocity.Y * Speed + TargetDir.Y * K;
		
		Velocity.Normalize();
	}

	Velocity.X *= Speed;
	Velocity.Y *= Speed;
	Velocity.Z = ZSpeed; // Z 속도 복구
}

