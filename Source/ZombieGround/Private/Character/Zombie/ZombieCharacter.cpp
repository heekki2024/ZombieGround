// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Zombie/ZombieCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Human/HumanCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactable/InteractInterface.h"


// Sets default values
AZombieCharacter::AZombieCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionCapsule"));
	InteractionCapsule->SetupAttachment(RootComponent);

	// 스케일 설정
	InteractionCapsule->SetWorldScale3D(FVector(4.7f, 4.7f, 4.7f));

	// 캡슐 크기 설정
	InteractionCapsule->SetCapsuleHalfHeight(40.30f);
	InteractionCapsule->SetCapsuleRadius(22.f);

	// Collision Preset 적용
	InteractionCapsule->SetCollisionProfileName(TEXT("InteractionCollision"));
	
	// 필요 시 Overlap 이벤트 발생 가능
	InteractionCapsule->SetGenerateOverlapEvents(true);
	
	
}

// Called when the game starts or when spawned
void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PC = Cast<APlayerController>(GetController());
	


	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	
	// Enhanced Input Subsystem 활성화
	if (IsValid(PC))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_ZombiePlayer)
				{    
					// IMC_HumanPlayer를 우선순위 0으로 추가
					Subsystem->AddMappingContext(IMC_ZombiePlayer, 0);
				}
			}
		}
	}
}

// Called every frame
void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	AActor* HitActor = GetCenterScreenInteractable();
	
	if(!HitActor)
	{
		// 이전에 하이라이트된 액터 끄기
		if (outLinedInteractable)
		{
			SetInteractableOutline(outLinedInteractable, false);
		}
		outLinedInteractable = nullptr;
	}else if (HitActor->Implements<UInteractInterface>())
	{

		// 이전에 하이라이트된 액터 끄기
		if (outLinedInteractable && outLinedInteractable != HitActor)
		{
			SetInteractableOutline(outLinedInteractable, false);
		}
	
		// 새로운 액터 하이라이트
		if (HitActor && HitActor != outLinedInteractable)
		{
			SetInteractableOutline(HitActor, true);
			outLinedInteractable = HitActor;
		}
	}
}

// Called to bind functionality to input
void AZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* playerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (playerInput)
	{
		playerInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AZombieCharacter::Move);
		playerInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AZombieCharacter::Look);
		playerInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &AZombieCharacter::JumpAction);
		playerInput->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AZombieCharacter::Interact);
		playerInput->BindAction(IA_MouseLeftClick, ETriggerEvent::Started, this, &AZombieCharacter::OnLeftClickPressed);
		playerInput->BindAction(IA_MouseLeftClick, ETriggerEvent::Completed, this, &AZombieCharacter::OnLeftClickReleased);
		playerInput->BindAction(IA_MouseRightClick, ETriggerEvent::Started, this, &AZombieCharacter::OnRightClickPressed);
		playerInput->BindAction(IA_MouseRightClick, ETriggerEvent::Completed, this, &AZombieCharacter::OnRightClickReleased);
		playerInput->BindAction(IA_Num1Key, ETriggerEvent::Started, this, &AZombieCharacter::OnNum1KeyPressed);	
		playerInput->BindAction(IA_Num2Key, ETriggerEvent::Started, this, &AZombieCharacter::OnNum2KeyPressed);	
		playerInput->BindAction(IA_Reload, ETriggerEvent::Started, this, &AZombieCharacter::Reload);	
		playerInput->BindAction(IA_Tab, ETriggerEvent::Started, this, &AZombieCharacter::OnTabPressed);	
	}
}

void AZombieCharacter::Move(const FInputActionValue& Value)
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

void AZombieCharacter::Look(const FInputActionValue& Value)
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

void AZombieCharacter::JumpAction(const FInputActionValue& Value)
{
	Jump();

}

void AZombieCharacter::Interact(const FInputActionValue& Value)
{
	if (!outLinedInteractable) return;
	
	IInteractInterface::Execute_OnInteract(outLinedInteractable, this);

}

void AZombieCharacter::OnRightClickPressed(const FInputActionValue& Value)
{
	
}

void AZombieCharacter::OnRightClickReleased(const FInputActionValue& Value)
{
}

void AZombieCharacter::OnLeftClickPressed(const FInputActionValue& Value)
{
}

void AZombieCharacter::OnLeftClickReleased(const FInputActionValue& Value)
{
}

void AZombieCharacter::OnNum1KeyPressed(const FInputActionValue& Value)
{
}

void AZombieCharacter::OnNum2KeyPressed(const FInputActionValue& Value)
{
}

void AZombieCharacter::Reload(const FInputActionValue& Value)
{
}

void AZombieCharacter::OnTabPressed(const FInputActionValue& Value)
{
}

void AZombieCharacter::SetInteractableOutline(AActor* interactable, bool bEnable)
{
	if (!interactable) return;

	// 1. StaticMesh 처리
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	interactable->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* MeshComp : StaticMeshComponents)
	{
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(bEnable);
			MeshComp->SetCustomDepthStencilValue(1); // 필요 시 스텐실 값
		}
	}

	// 2. SkeletalMesh 처리
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	interactable->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	for (USkeletalMeshComponent* SkeletalComp : SkeletalMeshComponents)
	{
		if (SkeletalComp)
		{
			SkeletalComp->SetRenderCustomDepth(bEnable);
			SkeletalComp->SetCustomDepthStencilValue(1);
		}
	}
}

AActor* AZombieCharacter::GetCenterScreenInteractable()
{
	if (!PC) return nullptr;

	int32 SizeX = 0, SizeY = 0;
	PC->GetViewportSize(SizeX, SizeY);
	if (SizeX == 0 || SizeY == 0) return nullptr;

	const float ScreenX = SizeX * 0.5f;
	const float ScreenY = SizeY * 0.5f;

	FVector WorldLoc, WorldDir;
	if (!PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLoc, WorldDir))
		return nullptr;

	const float TraceDistance = 200.f;
	FVector TraceStart = WorldLoc;
	FVector TraceEnd = WorldLoc + WorldDir * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	// ───── 디버그 라인 그리기 ─────
	// Hit 여부에 따라 색상 변경
	// FColor LineColor = bHit ? FColor::Green : FColor::Red;
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 1.f, 0, 1.f);

	if (!bHit) return nullptr;

	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return nullptr;

	// Overlap Capsule 확인 (InteractionCapsule)
	if (InteractionCapsule)
	{
		TArray<AActor*> OverlappedActors;
		InteractionCapsule->GetOverlappingActors(OverlappedActors);

		for (AActor* OverlapActor : OverlappedActors)
		{
			if (OverlapActor == HitActor)
			{
				return HitActor;
			}
		}
	}

	return nullptr;
}

