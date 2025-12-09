// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/HumanCharacter.h"

#include "Character/Zombie/ZombieCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Item/DataAsset/BaseDataAsset.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"
#include "Item/Instance/Weapon/WeaponInstance.h"
#include "Item/Pickup/BasePickup.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGame/Human/HumanHUD.h"


// Sets default values
AHumanCharacter::AHumanCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
	

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	
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
	
}

// Called every frame
void AHumanCharacter::Tick(float DeltaTime)
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
	if (!outLinedInteractable) return;
	
	IInteractInterface::Execute_OnInteract(outLinedInteractable, this);

}

void AHumanCharacter::OnRightClickPressed(const FInputActionValue& Value)
{	
	if (inventoryComponent->currentWeaponActor)
	{
		inventoryComponent->currentWeaponActor->OnRightClickPressed();
	}
}

void AHumanCharacter::OnRightClickReleased(const FInputActionValue& Value)
{
	if (inventoryComponent->currentWeaponActor)
	{
		inventoryComponent->currentWeaponActor->OnRightClickReleased();
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
	inventoryComponent->currentWeaponActor->Reload();
}

void AHumanCharacter::OnTabPressed(const FInputActionValue& Value)
{
	humanHud->ToggleInventory();
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

void AHumanCharacter::BroadcastCurrentAmmoUpdate()
{
	// 인벤토리나 현재 무기가 유효한지 체크
	if (inventoryComponent && inventoryComponent->currentWeaponActor)
	{
		// 현재 들고 있는 무기의 인스턴스 가져오기
		UWeaponInstance* currentWeaponInstance = inventoryComponent->currentWeaponActor->weaponInstance;
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
		UWeaponInstance* currentWeaponInstance = inventoryComponent->currentWeaponActor->weaponInstance;
		if (currentWeaponInstance)
		{
			// [방송 송출] 현재 탄약과 (최대 탄약 + 보정치)를 보냄
			OnInventoryAmmoChanged.Broadcast(inventoryComponent->GetItemQuantity(Cast<UWeaponDataAsset>(currentWeaponInstance->GetItemData())));
			return;
		}
	}
	// 무기가 없거나 오류 상황이면 0으로 방송
	OnInventoryAmmoChanged.Broadcast(0);
}



