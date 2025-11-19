// Fill out your copyright notice in the Description page of Project Settings.


#include "Human/HumanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AHumanCharacter::AHumanCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	
	// Enhanced Input Subsystem 활성화
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_HumanPlayer)
				{
					// IMC_HumanPlayer를 우선순위 0으로 추가
					Subsystem->AddMappingContext(IMC_HumanPlayer, 0);
				}
			}
		}
	}
	
}

// Called every frame
void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* playerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (playerInput)
	{
		playerInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AHumanCharacter::Move);
		playerInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AHumanCharacter::Look);
		playerInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &AHumanCharacter::JumpAction);
	}
}

void AHumanCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();


	if (Controller)
	{
		// Forward / Backward
		AddMovementInput(GetActorForwardVector(), MoveVector.Y);
		// Right / Left
		AddMovementInput(GetActorRightVector(), MoveVector.X);
	}
}

void AHumanCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		// 좌우 회전 (Yaw)
		AddControllerYawInput(LookAxisVector.X * MouseSensitivity);

		// 상하 회전 (Pitch)
		// 상하 반전 적용 가능: bInvertMouseY
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity);
	}
}

void AHumanCharacter::JumpAction(const FInputActionValue& Value)
{
	Jump();
}

