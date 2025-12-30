// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));

	// [추가] 스프링 암 자체의 위치 설정 (X: 15, Y: 0, Z: 55)
	springArmComponent->SetRelativeLocation(FVector(15.0f, 0.0f, 55.0f));

	// [추가] 스프링 암 끝부분(타겟)의 오프셋 설정 (Z: 20 올림)
	// springArmComponent->TargetOffset = FVector(0.0f, 0.0f, 20.0f);
	springArmComponent->SocketOffset = FVector(0.0f, 0.0f, 20.0f);

	springArmComponent->TargetArmLength = 0.f;
	springArmComponent->bUsePawnControlRotation = true;
	springArmComponent->bInheritPitch = true;
	springArmComponent->bInheritYaw = true;
	springArmComponent->bInheritRoll = false;
	springArmComponent->bDoCollisionTest = false;
	springArmComponent->SetupAttachment(GetRootComponent());
	
	firstPersonCamera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	firstPersonCamera->SetupAttachment(springArmComponent, FName("camera"));
	firstPersonCamera->SetFieldOfView(DefaultFOV);

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

