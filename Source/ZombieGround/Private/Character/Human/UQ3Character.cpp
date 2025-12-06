// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/UQ3Character.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Human/UQ3CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


AUQ3Character::AUQ3Character(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UUQ3CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	
	PrimaryActorTick.bCanEverTick = true;

	// 1. 카메라 설정
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true; // 컨트롤러 회전을 따름
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f)); // 눈 높이

	// 기본 회전 설정
	TargetYaw = 0.0f;
	TargetPitch = 0.0f;
}

// Called when the game starts or when spawned
void AUQ3Character::BeginPlay()
{
	Super::BeginPlay(); // 2번 호출되어 있던 부분 하나 제거함

	// 1. PlayerController 가져오기
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// 2. 커서 설정 (기존 코드)
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());

		// 3. [중요] Enhanced Input Local Player Subsystem 가져오기
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 4. 매핑 컨텍스트 추가 (DefaultMappingContext 변수가 헤더에 있어야 함)
			// 우선순위는 0으로 설정
			if (IMC_UQ3Player)
			{
				Subsystem->AddMappingContext(IMC_UQ3Player, 0);
			}
		}
	}
    
	// 초기 회전값 동기화
	TargetYaw = GetControlRotation().Yaw;
	TargetPitch = GetControlRotation().Pitch;
}

// Called every frame
void AUQ3Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// --- [MouseLook.cs] 스무딩 처리 ---
	if (bUseSmoothing)
	{
		FRotator CurrentRot = GetControlRotation();
		FRotator TargetRot = FRotator(TargetPitch, TargetYaw, 0.0f);
		
		// 부드럽게 보간 (Quaternion Slerp 대신 RInterpTo 사용)
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, SmoothTime);
		
		if (Controller)
		{
			Controller->SetControlRotation(NewRot);
		}
	}

	// --- [Q3PlayerDebug.cs] FPS 및 속도 표시 ---
	if (bShowDebugStats)
	{
		// 1. 현재 속도 계산 (XY 평면 속도)
		float CurrentSpeed = GetVelocity().Size2D(); // Quake는 수평 속도를 중시함
		
		// 2. 최고 속도 갱신
		if (CurrentSpeed > TopSpeed)
		{
			TopSpeed = CurrentSpeed;
		}

		// 3. FPS 계산 (언리얼은 1.0 / DeltaTime)
		float FPS = 1.0f / DeltaTime;

		// 4. 화면 출력 (Unity OnGUI 대응)
		// Key: -1(새 줄), Time: 0(한 프레임만)
		FString DebugMsg = FString::Printf(TEXT("FPS: %.0f\nSpeed: %.2f (units/s)\nTop: %.2f"), FPS, CurrentSpeed, TopSpeed);
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, DebugMsg);
	}
}

// Called to bind functionality to input
void AUQ3Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// // 마우스 입력 바인딩 (프로젝트 세팅 -> 입력에 'Turn', 'LookUp' 축이 있어야 함)
	// PlayerInputComponent->BindAxis("Turn", this, &AUQ3Character::Turn);
	// PlayerInputComponent->BindAxis("LookUp", this, &AUQ3Character::LookUp);
	// // [추가] 키보드 이동 입력 바인딩 (프로젝트 세팅의 이름과 일치해야 함)
	// PlayerInputComponent->BindAxis("MoveForward", this, &AUQ3Character::MoveForward);
	// PlayerInputComponent->BindAxis("MoveRight", this, &AUQ3Character::MoveRight);
	//
	// PlayerInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AUQ3Character::Turn);
	// PlayerInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AUQ3Character::Turn);
	// PlayerInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AUQ3Character::Turn);
	// PlayerInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AUQ3Character::Turn);
	
	// UEnhancedInputComponent로 캐스팅
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// [수정] 각 Action에 맞는 함수를 연결해야 합니다.
		// 기존: 전부 &AUQ3Character::Turn으로 되어 있었음
        
		// 1. 회전 (좌우 시점)
		EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AUQ3Character::Turn);
        
		// 2. 고개 들기 (상하 시점) - [수정됨]
		EnhancedInputComponent->BindAction(IA_Lookup, ETriggerEvent::Triggered, this, &AUQ3Character::LookUp);
        
		// 3. 앞뒤 이동 - [수정됨]
		EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &AUQ3Character::MoveForward);
        
		// 4. 좌우 이동 - [수정됨]
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &AUQ3Character::MoveRight);
		
		// [핵심] Started: 누르는 그 순간에 딱 한 번 호출됨 -> Jump() 실행
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AUQ3Character::Input_Jump);
            
		// [핵심] Completed: 키를 떼면 호출됨 -> StopJumping() 실행 (점프 높이 조절 등을 위해 필요)
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AUQ3Character::Input_StopJump);
		
	}
}

void AUQ3Character::LookUp(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	if (Val == 0.f) return;

	// 언리얼은 마우스 Y가 위쪽이 -인 경우가 많아 확인 필요 (보통 -1 곱함)
	float Delta = Val * YSensitivity * -1.0f; 

	if (bUseSmoothing)
	{
		TargetPitch += Delta;
		// [MouseLook.cs] ClampVerticalRotation 대응
		// 언리얼 기본 카메라는 -90 ~ 90으로 자동 클램프되지만, 변수도 클램프
		TargetPitch = FMath::Clamp(TargetPitch, -89.0f, 89.0f);
	}
	else
	{
		AddControllerPitchInput(Delta);
		TargetPitch = GetControlRotation().Pitch;
	}
}

void AUQ3Character::Turn(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	if (Val == 0.f) return;

	float Delta = Val * XSensitivity;

	if (bUseSmoothing)
	{
		TargetYaw += Delta; // 목표값만 누적
	}
	else
	{
		AddControllerYawInput(Delta); // 즉시 회전
		TargetYaw = GetControlRotation().Yaw; // 동기화
	}
}

void AUQ3Character::MoveForward(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	if ((Controller != nullptr) && (Val != 0.0f))
	{
		// 컨트롤러가 보고 있는 방향(Yaw)을 기준으로 앞쪽 방향을 찾습니다.
		// Pitch(위아래) 회전은 무시해야 땅으로 파고들지 않고 수평으로 이동합니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전방 벡터(X축)를 구함
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        
		// 이동 입력 추가 (무브먼트 컴포넌트가 이를 받아 처리함)
		AddMovementInput(Direction, Val);
	}
}

void AUQ3Character::MoveRight(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	if ((Controller != nullptr) && (Val != 0.0f))
	{
		// 컨트롤러가 보고 있는 방향(Yaw)을 기준으로 오른쪽 방향을 찾습니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 우측 벡터(Y축)를 구함
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 이동 입력 추가
		AddMovementInput(Direction, Val);
	}
}

void AUQ3Character::Input_Jump(const FInputActionValue& Value)
{
	// 내부적으로 bPressedJump = true로 만듦
	// CheckJumpInput(부모)이 이걸 감지해서 "땅에 있으면 점프" 시킴
	Jump();
}

void AUQ3Character::Input_StopJump(const FInputActionValue& Value)
{
	// bPressedJump = false로 만듦
	// 점프 키를 떼면 점프 높이를 낮게 조절하는 등의 처리가 내부적으로 됨
	StopJumping();
}

// void AUQ3Character::CheckJumpInput(float DeltaTime)
// {
// 	// 퀘이크 식 자동 점프(Bunny Hop) 구현 예시
//
// 	// 점프 키를 누르고 있으면
// 	if (bPressedJump)
// 	{
// 		// 움직임 컴포넌트에게 즉시 점프 시도 명령
// 		// (눌렀다 떼는 체크인 bWasJumping 과정을 무시하여 연속 점프 가능하게 함)
// 		GetCharacterMovement()->DoJump(false);
// 	}
//
// 	// 부모 로직은 호출하지 않거나, 필요한 경우에만 호출
// 	// Super::CheckJumpInput(DeltaTime);
// }

void AUQ3Character::CheckJumpInput(float DeltaTime)
{
	// [수정] 강제 점프 로직(DoJump)을 지우고, 부모의 기본 로직만 호출합니다.
	// 부모의 로직은 'bPressedJump'가 true여도 땅에 닿았다가 다시 누르지 않으면 점프하지 않도록 막아줍니다.
	Super::CheckJumpInput(DeltaTime);
}

