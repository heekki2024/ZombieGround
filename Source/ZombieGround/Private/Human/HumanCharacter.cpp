// Fill out your copyright notice in the Description page of Project Settings.


#include "Human/HumanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AHumanCharacter::AHumanCharacter()
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
void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Overlap 이벤트 바인딩
	InteractionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AHumanCharacter::OnInteractableBeginOverlap);
	InteractionCapsule->OnComponentEndOverlap.AddDynamic(this, &AHumanCharacter::OnInteractableEndOverlap);
	

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
	
	AActor* HitActor = GetCenterScreenInteractable();

	// 이전에 하이라이트된 액터 끄기
	if (HighlightedActor && HighlightedActor != HitActor)
	{
		SetActorOutline(HighlightedActor, false);
	}

	// 새로운 액터 하이라이트
	if (HitActor && HitActor != HighlightedActor)
	{
		SetActorOutline(HitActor, true);
		HighlightedActor = HitActor;
	}
	else if(!HitActor)
	{
		HighlightedActor = nullptr;
	}
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
		playerInput->BindAction(IA_Interact, ETriggerEvent::Started, this, &AHumanCharacter::Interact);
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

void AHumanCharacter::Interact(const FInputActionValue& Value)
{
	
}

void AHumanCharacter::OnInteractableBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel2)
	{
		OverlappingInteractables.Add(OtherActor);
	}


}

void AHumanCharacter::OnInteractableEndOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{

	OverlappingInteractables.Remove(OtherActor);
}

AActor* AHumanCharacter::GetCenterScreenInteractable()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
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
	FColor LineColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 1.f, 0, 1.f);

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

void AHumanCharacter::SetActorOutline(AActor* Actor, bool bEnable)
{
	if (!Actor) return;

	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* InteractableItemMesh : MeshComponents)
	{
		if (InteractableItemMesh)
		{
			InteractableItemMesh->SetRenderCustomDepth(bEnable);
			// 필요시 Stencil Value도 설정 가능
			InteractableItemMesh->SetCustomDepthStencilValue(1);
		}
	}
}