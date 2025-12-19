// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/HumanCharacter.h"

#include "Character/Zombie/ZombieCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Item/DataAsset/BaseDataAsset.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Flashlight/Flashlight.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
#include "Item/Equippable/Weapon/WeaponActor/SecondaryWeapon/Pistol/BasePistolActor.h"
#include "Item/Instance/Weapon/BaseWeaponInstance.h"
#include "Item/Pickup/BasePickup.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGame/Human/HumanHUD.h"


// Sets default values
AHumanCharacter::AHumanCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	springArmComponent->TargetArmLength = 0.f;
	springArmComponent->bUsePawnControlRotation = true;
	springArmComponent->bInheritPitch = true;
	springArmComponent->bInheritYaw = true;
	springArmComponent->bInheritRoll = false;
	springArmComponent->bDoCollisionTest = false;
	springArmComponent->SetupAttachment(GetRootComponent());
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	FirstPersonCamera->SetupAttachment(springArmComponent, FName("camera"));
	FirstPersonCamera->SetFieldOfView(DefaultFOV);

	
	inventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	
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
	
	PC = Cast<APlayerController>(GetController());
	
	// Overlap 이벤트 바인딩
	InteractionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AHumanCharacter::OnInteractableBeginOverlap);
	InteractionCapsule->OnComponentEndOverlap.AddDynamic(this, &AHumanCharacter::OnInteractableEndOverlap);

	
	// if (!FirstPersonCamera)
	// {
	// 	// 블루프린트에 추가된 CameraComponent를 찾아 연결합니다.
	// 	FirstPersonCamera = GetComponentByClass<UCameraComponent>();
	// }

	// 게임 시작 시 기본 FOV 적용
	// if (FirstPersonCamera)
	// {
	// 	FirstPersonCamera->SetFieldOfView(DefaultFOV);
	// }

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	
	// Enhanced Input Subsystem 활성화
	if (IsValid(PC))
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
	
	
	
	// [추가] 스태미나 초기화
	CurrentStamina = MaxStamina;
    
	// 초기 UI 업데이트 (필요 시)
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	
	

	
}

// Called every frame
void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	UpdateInteractableHighlight();
	UpdateRunSpeed(DeltaTime);
}

float AHumanCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser && DamageCauser->IsA(AZombieCharacter::StaticClass()))
	{
		AController* CurrentController = GetController();
		
		if (ZombieClassToSpawn && CurrentController)
		{
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AZombieCharacter* NewZombie = GetWorld()->SpawnActor<AZombieCharacter>(ZombieClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			if (NewZombie)
			{
				CurrentController->Possess(NewZombie);
				Destroy();
			}
		}
	}
	
	return ActualDamage;
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
		playerInput->BindAction(IA_MouseLeftClick, ETriggerEvent::Started, this, &AHumanCharacter::OnLeftClickPressed);
		playerInput->BindAction(IA_MouseLeftClick, ETriggerEvent::Completed, this, &AHumanCharacter::OnLeftClickReleased);
		playerInput->BindAction(IA_MouseRightClick, ETriggerEvent::Started, this, &AHumanCharacter::OnRightClickPressed);
		playerInput->BindAction(IA_MouseRightClick, ETriggerEvent::Completed, this, &AHumanCharacter::OnRightClickReleased);
		playerInput->BindAction(IA_Num1Key, ETriggerEvent::Started, this, &AHumanCharacter::OnNum1KeyPressed);	
		playerInput->BindAction(IA_Num2Key, ETriggerEvent::Started, this, &AHumanCharacter::OnNum2KeyPressed);	
		playerInput->BindAction(IA_Reload, ETriggerEvent::Started, this, &AHumanCharacter::Reload);	
		playerInput->BindAction(IA_Tab, ETriggerEvent::Started, this, &AHumanCharacter::OnTabPressed);	
		playerInput->BindAction(IA_Run, ETriggerEvent::Started, this, &AHumanCharacter::StartRun);	
		playerInput->BindAction(IA_Run, ETriggerEvent::Completed, this, &AHumanCharacter::StopRun);	
		playerInput->BindAction(IA_Toggle, ETriggerEvent::Started, this, &AHumanCharacter::Toggle);	

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
	// 1. 현재 스태미나가 점프 비용(30)보다 많은지 확인
	// (Unreal의 CanJump()는 공중에 있는지 등을 체크하므로, 스태미나 체크를 먼저 함)
	if (CurrentStamina >= JumpStaminaCost)
	{
		// 2. 스태미나 즉시 차감 (30 감소)
		CurrentStamina -= JumpStaminaCost;
        
		// [추가] 스태미나를 썼으니 현재 시간을 기록 (회복 딜레이 리셋)
		LastStaminaUseTime = GetWorld()->GetTimeSeconds();
		
		// 3. UI 업데이트 (중요: 값이 확 변했으니 즉시 알려줌)
		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);

		// 4. 실제 캐릭터 점프 실행
		Jump();
	}
	else
	{
		// 스태미나 부족 시 효과음이나 로그 등을 여기에 추가 가능
		// UE_LOG(LogTemp, Warning, TEXT("Not enough stamina to jump!"));
	}
}

void AHumanCharacter::Interact(const FInputActionValue& Value)
{
	if (!outLinedInteractable) return;
	
	IInteractInterface::Execute_OnInteract(outLinedInteractable, this);

}

void AHumanCharacter::OnRightClickPressed(const FInputActionValue& Value)
{	
	if (inventoryComponent->currentWeaponActor)
	{
		// 1. 조준 플래그 켜기
		bIsAiming = true;
       
		// 2. [핵심] 달리고 있었다면 즉시 취소
		// 플래그를 false로 만들면, 다시 Shift를 눌러서 StartRun을 호출하기 전까지는 걷기 상태가 됨
		// if (bWantsToSprint)
		// {
		// 	bWantsToSprint = false;
		// }

		inventoryComponent->currentWeaponActor->OnRightClickPressed();
	}
}

void AHumanCharacter::OnRightClickReleased(const FInputActionValue& Value)
{
	if (inventoryComponent->currentWeaponActor)
	{
		// 1. 조준 플래그 비활성화
		bIsAiming = false;

		inventoryComponent->currentWeaponActor->OnRightClickReleased();
	}
	// [추가] 무기가 없어도 버튼을 떼면 플래그는 꺼주는 게 안전함
	else 
	{
		bIsAiming = false;
	}
}

void AHumanCharacter::OnLeftClickPressed(const FInputActionValue& Value)
{
	if (inventoryComponent->currentWeaponActor)
	{
		inventoryComponent->currentWeaponActor->OnLeftClickPressed();
	}
}

void AHumanCharacter::OnLeftClickReleased(const FInputActionValue& Value)
{
	if (inventoryComponent->currentWeaponActor)
	{
		inventoryComponent->currentWeaponActor->OnLeftClickReleased();
	}
}

void AHumanCharacter::OnNum1KeyPressed(const FInputActionValue& Value)
{
	//인벤토리에 주무기가 있는지 확인한다. 없으면 return;
	//현재 들고 있는 무기가 인벤토리의 주무기에 해당하면 return;
	//총기 swap, 스왑한 무기를 현재 들고 있는 무기로 설정
	if (inventoryComponent->primaryWeaponSlot == nullptr) return;
	if (inventoryComponent->primaryWeaponSlot == inventoryComponent->currentWeaponActor->weaponInstance) return;
	inventoryComponent->EquipPrimaryWeapon();
	inventoryComponent->currentWeaponActor->weaponInstance = inventoryComponent->primaryWeaponSlot;

}

void AHumanCharacter::OnNum2KeyPressed(const FInputActionValue& Value)
{
	if (inventoryComponent->secondaryWeaponSlot == nullptr) return;
	if (inventoryComponent->secondaryWeaponSlot == inventoryComponent->currentWeaponActor->weaponInstance) return;

	inventoryComponent->EquipSecondaryWeapon();
	inventoryComponent->currentWeaponActor->weaponInstance = inventoryComponent->secondaryWeaponSlot;
}

void AHumanCharacter::Reload(const FInputActionValue& Value)
{
	inventoryComponent->currentWeaponActor->TryReload();
}

void AHumanCharacter::OnTabPressed(const FInputActionValue& Value)
{
	humanHud->ToggleInventory();
}

void AHumanCharacter::Toggle(const struct FInputActionValue& Value)
{
	if (ABasePistolActor* PistolActor = Cast<ABasePistolActor>(inventoryComponent->currentWeaponActor))
	{
		inventoryComponent->currentFlashlight->ToggleLight();
	}
}


void AHumanCharacter::OnInteractableBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	ECollisionChannel ObjType = OtherComp->GetCollisionObjectType();
	if (ObjType == ECC_GameTraceChannel2 || ObjType == ECC_GameTraceChannel4)
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

void AHumanCharacter::SetInteractableOutline(AActor* interactable, bool bEnable)
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

void AHumanCharacter::UpdateInteractableHighlight()
{
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
			SetInteractableOutline(HitActor, true);
			outLinedInteractable = HitActor;
		}
	}
}

void AHumanCharacter::BroadcastCurrentAmmoUpdate()
{
	// 인벤토리나 현재 무기가 유효한지 체크
	if (inventoryComponent && inventoryComponent->currentWeaponActor)
	{
		// 현재 들고 있는 무기의 인스턴스 가져오기
		UBaseWeaponInstance* currentWeaponInstance = inventoryComponent->currentWeaponActor->weaponInstance;
		if (currentWeaponInstance)
		{
			// [방송 송출] 현재 탄약과 (최대 탄약 + 보정치)를 보냄
			OnCurrentAmmoChanged.Broadcast(currentWeaponInstance->currentAmmo);
			return;
		}
	}

	// 무기가 없거나 오류 상황이면 0으로 방송
	OnCurrentAmmoChanged.Broadcast(0);
}

void AHumanCharacter::BroadcastInventoryAmmoUpdate()
{
	// 인벤토리나 현재 무기가 유효한지 체크
	if (inventoryComponent && inventoryComponent->currentWeaponActor)
	{
		// 현재 들고 있는 무기의 인스턴스 가져오기
		UBaseWeaponInstance* currentWeaponInstance = inventoryComponent->currentWeaponActor->weaponInstance;
		if (currentWeaponInstance)
		{
			// 1. 인벤토리에 있는 탄약 수
			int32 InventoryAmmo = inventoryComponent->GetItemQuantity(Cast<UWeaponDataAsset>(currentWeaponInstance->GetItemData()));
			
			// 2. 무기 내부에 숨겨진 예비 탄약 수
			int32 InternalAmmo = currentWeaponInstance->InternalReserveAmmo;

			// [방송 송출] 두 값을 합쳐서 보냄 (UI는 이 합계만 알면 됨)
			OnInventoryAmmoChanged.Broadcast(InventoryAmmo + InternalAmmo);
			return;
		}
	}
	// 무기가 없거나 오류 상황이면 0으로 방송
	OnInventoryAmmoChanged.Broadcast(0);
}

void AHumanCharacter::StartRun(const FInputActionValue& Value)
{
	// [수정] 스태미나가 최소 요구량보다 많을 때만 달리기 시작 가능
	if (CurrentStamina > MinStaminaToRun)
	{
		bWantsToSprint = true;
	}}

void AHumanCharacter::StopRun(const FInputActionValue& Value)
{
	bWantsToSprint = false; // 초기화

}

void AHumanCharacter::UpdateRunSpeed(float DeltaTime)
{
	float CurrentActualSpeed = GetVelocity().Size2D();
    float CurrentMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
    float CurrentTimeForRun = GetWorld()->GetTimeSeconds();
    bool bIsStaminaChanged = false; 

    // -------------------------------------------------------
    // 1. 목표 속도 결정 (우선순위: 조준 > 달리기 > 걷기)
    // -------------------------------------------------------
    float TargetMaxSpeed = WalkSpeed; // 350

    if (bIsAiming)
    {
        TargetMaxSpeed = AimWalkSpeed; // 250
    }
    else if (bWantsToSprint)
    {
        TargetMaxSpeed = runSpeed; // 650
    }

    // -------------------------------------------------------
    // 2. 가속도(Alpha) 및 속도 적용
    // -------------------------------------------------------
    
    // "달리기를 원하고" AND "조준 중이 아닐 때"만 가속 게이지가 차오름
    if (bWantsToSprint && !bIsAiming)
    {
        // A. 시간 경과에 따른 가속
        runAlpha += (DeltaTime / runAccelerationTime);

        // B. 벽 충돌 감지 (막히면 가속도 리셋)
        bool bIsBlocked = (CurrentMaxSpeed > WalkSpeed + 10.0f) && (CurrentActualSpeed < WalkSpeed - 50.0f);
        if (bIsBlocked)
        {
            runAlpha = FMath::FInterpTo(runAlpha, 0.0f, DeltaTime, 10.0f);
        }

        // C. 속도 적용 (Ease-In)
        runAlpha = FMath::Clamp(runAlpha, 0.0f, 1.0f);
        float NewSpeed = FMath::InterpEaseIn(WalkSpeed, runSpeed, runAlpha, runEaseExp);
        GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
    }
    else
    {
        // [핵심] 걷거나 '조준 중'일 때는 가속도(Alpha)를 0으로 초기화
        // 이렇게 해야 조준을 풀었을 때 Alpha가 0(속도 350)부터 다시 시작됨!
        runAlpha = 0.0f;
        
        // 조준(250)이나 걷기(350)로 갈 때는 FInterpTo로 빠르게 전환
        if (!FMath::IsNearlyEqual(CurrentMaxSpeed, TargetMaxSpeed))
        {
            float NewSpeed = FMath::FInterpTo(CurrentMaxSpeed, TargetMaxSpeed, DeltaTime, 10.0f);
            GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
        }
    }


    // -------------------------------------------------------
    // 3. 스태미나 로직 (조준 중에는 소모 안 함)
    // -------------------------------------------------------
    if (bWantsToSprint && !bIsAiming) 
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;
        LastStaminaUseTime = CurrentTimeForRun;
        bIsStaminaChanged = true;

        if (CurrentStamina <= 0.0f)
        {
            CurrentStamina = 0.0f;
            // 스태미나 다 닳았을 때만 강제로 달리기 해제
            bWantsToSprint = false; 
        }
    }
    else
    {
        // 회복 로직
        if (CurrentTimeForRun - LastStaminaUseTime >= StaminaRecoveryDelay)
        {
            if (CurrentStamina < MaxStamina)
            {
                CurrentStamina += StaminaRegenRate * DeltaTime;
                if (CurrentStamina > MaxStamina) CurrentStamina = MaxStamina;
                bIsStaminaChanged = true;
            }
        }
    }

    if (bIsStaminaChanged)
    {
        OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    }


    // ----------------------------------------------------------------
    // 4. FOV 로직 (기존 유지)
    // ----------------------------------------------------------------
    if (FirstPersonCamera)
    {
        float TargetFOV = DefaultFOV;

        if (bIsAiming)
        {
            TargetFOV = AimingFOV;
        }
        else 
        {
            TargetFOV = FMath::GetMappedRangeValueClamped(
                FVector2D(WalkSpeed, runSpeed),
                FVector2D(DefaultFOV, RunFOV),
                CurrentActualSpeed
            );
        }

        float InterpSpeed = bIsAiming ? 15.0f : 10.0f;
        float NewFOV = FMath::FInterpTo(FirstPersonCamera->FieldOfView, TargetFOV, DeltaTime, InterpSpeed);
        FirstPersonCamera->SetFieldOfView(NewFOV);
    }
    
    // 디버그 (수치 확인용)
    if (IsLocallyControlled() && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 0.0f, FColor::Yellow,
            FString::Printf(TEXT("Real: %.0f / Max: %.0f / Alpha: %.2f"), 
            CurrentActualSpeed, GetCharacterMovement()->MaxWalkSpeed, runAlpha)
        );
    }
}


