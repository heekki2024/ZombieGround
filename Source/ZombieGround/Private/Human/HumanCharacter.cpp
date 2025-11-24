// Fill out your copyright notice in the Description page of Project Settings.


#include "Human/HumanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Weapon/Pickup/BasePickup.h"
#include "Item/Weapon/Weapon/BaseWeapon.h"


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

	// 1. 먼저 Pickup인지 확인해봅니다.
	if (ABasePickup* pickup = Cast<ABasePickup>(HitActor))
	{
		// 이전에 하이라이트된 액터 끄기
		if (HighlightedPickup && HighlightedPickup != HitActor)
		{
			SetActorOutline(HighlightedPickup, false);
		}

		// 새로운 액터 하이라이트
		if (HitActor && HitActor != HighlightedPickup)
		{
			SetActorOutline(pickup, true);
			HighlightedPickup = pickup;
		}
	}else if(!HitActor)
	{
		
		// 이전에 하이라이트된 액터 끄기
		if (HighlightedPickup)
		{
			SetActorOutline(HighlightedPickup, false);
		}
		HighlightedPickup = nullptr;

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
		playerInput->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AHumanCharacter::Interact);
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
	if (!HighlightedPickup) return;
	IInteractInterface::Execute_OnInteract(HighlightedPickup, this, HighlightedPickup);
	SwapWeapon(HighlightedPickup->GetWeaponToSpawn());

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

void AHumanCharacter::SetActorOutline(ABasePickup* pickup, bool bEnable)
{
	if (!pickup) return;

	TArray<UStaticMeshComponent*> MeshComponents;
	pickup->GetComponents<UStaticMeshComponent>(MeshComponents);

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

void AHumanCharacter::SwapWeapon(TSubclassOf<ABaseWeapon> weaponToSpawn)
{
	if (!currentWeapon)
	DropCurrentWeapon();

    // 2. 스폰 파라미터 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    SpawnParams.SpawnCollisionHandlingOverride = 
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 3. 스폰 위치/회전은 대충 캐릭터 위치 기준으로
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();

    // 4. 액터 스폰
    ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(
        weaponToSpawn,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    if (!IsValid(NewWeapon))
    {
        UE_LOG(LogTemp, Error, TEXT("SwapWeapon: Failed to spawn weapon %s"),
            *weaponToSpawn->GetName());
        return;
    }
	

    // 6. 무기 저장
    currentWeapon = NewWeapon;
	// currentWeaponNameEnum = currentWeapon->weaponDetails.WeaponName;
	
	
    // 7. Attach (부착)
    NewWeapon->AttachToComponent(
        GetMesh(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        NewWeapon->weaponDetails.socketName
    );
	
	
	HighlightedPickup->Destroy();
}


void AHumanCharacter::DropCurrentWeapon()
{
	if (!IsValid(currentWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("DropWeapon: CurrentWeapon is invalid"));
		return;
	}
	
	// 1. 무기 픽업 클래스가 있는지 확인 (무기 클래스 내부에 있을 것으로 가정)
	TSubclassOf<ABasePickup> PickupClass = currentWeapon->pickupClass;
	if (!PickupClass)
	{
		UE_LOG(LogTemp, Error, TEXT("DropWeapon: Weapon has no PickupClass"));
		return;
	}
	
	// [수정 포인트 1] 바라보는 방향(Aim Direction) 가져오기
	// GetActorForwardVector() 대신 GetControlRotation()을 사용합니다.
	// GetControlRotation()은 마우스/스틱으로 조종하는 카메라의 회전값(Pitch, Yaw)을 포함합니다.
	FRotator ControlRotation = GetControlRotation();
	FVector AimDirection = ControlRotation.Vector(); // 회전값을 방향 벡터로 변환

	// [수정 포인트 2] 스폰 위치 계산
	// 바라보는 방향으로 100만큼 떨어진 곳에서 스폰
	FVector SpawnLocation = GetActorLocation() + (AimDirection * 50.f); 
    
	// [옵션] 스폰 회전값도 시선과 일치시킬지, 아니면 랜덤하게 할지 결정
	// 무기가 날아가는 방향으로 머리를 돌리려면 ControlRotation을 넣으세요.
	FRotator SpawnRotation = ControlRotation; 
    
	// 3. 무기 제거 (기존 코드 동일)
	currentWeapon->Destroy();
	currentWeapon = nullptr;
    
	// 4. 픽업 액터 스폰 (기존 코드 동일)
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = 
	   ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABasePickup* Pickup = GetWorld()->SpawnActor<ABasePickup>(
	   PickupClass,
	   SpawnLocation,
	   SpawnRotation,
	   SpawnParams
	);
    
	// 5. 물리 임펄스 적용
	if (Pickup) // Pickup이 잘 생성되었는지 확인
	{
		UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Pickup->GetRootComponent());
		if (RootComp && RootComp->IsSimulatingPhysics())
		{
			// [수정 포인트 3] 바라보는 방향(AimDirection)으로 힘을 가함
			// 400.f는 좀 약할 수 있으니 테스트해보며 조절하세요 (예: 1000.f)
			FVector ThrowForce = AimDirection * 600.f; 
			RootComp->AddImpulse(ThrowForce, NAME_None, true);
		}
	}
}
