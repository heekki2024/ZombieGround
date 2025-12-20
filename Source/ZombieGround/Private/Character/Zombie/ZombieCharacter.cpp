// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Zombie/ZombieCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Human/HumanCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactable/InteractInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"


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
	


	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	
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

	if (DieSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DieSound);
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
	
	// 1. 현재 컨트롤러가 플레이어 컨트롤러인지 확인
	if ((PC = Cast<APlayerController>(GetController())))
	{
		// 2. Enhanced Input 로컬 플레이어 서브시스템 가져오기
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// 3. 기존(인간 등)의 모든 매핑 컨텍스트 제거 (충돌 방지)
			Subsystem->ClearAllMappings();

			// 4. 좀비용 매핑 컨텍스트 추가
			if (IMC_ZombiePlayer)
			{
				Subsystem->AddMappingContext(IMC_ZombiePlayer, 0);
			}
		}
	}
	
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
	PlayBasicAttackMontage();
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

void AZombieCharacter::PlayBasicAttackMontage()
{
	// 1. 몽타주 에셋이 할당되어 있는지 확인
	if (basicAttackMontage)
	{
		// 2. 현재 몽타주가 이미 재생 중이 아니라면 실행 (스팸 방지 로직이 필요하다면 사용)
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(basicAttackMontage))
		{
			// 3. ACharacter 클래스 내장 함수로 몽타주 재생
			PlayAnimMontage(basicAttackMontage);
            
			// 만약 특정 섹션부터 시작하거나 속도를 조절하려면 아래와 같이 사용:
			// PlayAnimMontage(AttackMontage, 1.0f, FName("StartSection"));
		}
        
		// 스팸 방지 없이 무조건 재생하려면 단순히 아래 한 줄만 써도 됩니다.
		// PlayAnimMontage(AttackMontage);
	}
}

void AZombieCharacter::PlayDamageMontage()
{
	// 1. 몽타주 에셋이 할당되어 있는지 확인
	if (DamageMontage)
	{
		// 2. 현재 몽타주가 이미 재생 중이 아니라면 실행 (스팸 방지 로직이 필요하다면 사용)
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(DamageMontage))
		{
			//피격 애니메이션 재생
			int index = FMath::RandRange(0, 2);
			// FName sectionName(FString::Printf(TEXT("Damage%d"), index));
			// 1. int를 FName으로 변환 (예: index가 1이면 "Hit1"이 됨)
			// *FString::Printf는 포맷팅된 문자열을 만듭니다.
			FName SectionName = FName(*FString::Printf(TEXT("Damage%d"), index));

			// 2. 세 번째 인자에 섹션 이름 전달
			PlayAnimMontage(DamageMontage, 1.0f, SectionName);
            
			// 만약 특정 섹션부터 시작하거나 속도를 조절하려면 아래와 같이 사용:
			// PlayAnimMontage(AttackMontage, 1.0f, FName("StartSection"));
		}
	}
}

void AZombieCharacter::AttackHitCheck()
{
	// 1. 트레이스 시작점과 끝점 계산
	// 캐릭터의 위치에서 정면으로 AttackRange만큼 뻗어나감
	FVector Start = GetActorLocation(); 
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + (ForwardVector * AttackRange);

	// 2. 충돌 감지 매개변수 설정
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자기 자신은 무시

	// 3. 스피어 트레이스 실행 (SweepSingleByChannel)
	// 여기서는 파온(Pawn)만 감지하도록 설정했습니다. 필요에 따라 채널 변경 가능 (예: ECC_GameTraceChannel1)
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	// 4. 디버그 드로잉 (개발 중에 눈으로 확인하기 위해 필수)
	// 빨간색: 충돌 없음 / 초록색: 충돌 함
	FVector Center = Start + (ForwardVector * (AttackRange * 0.5f));
	float HalfHeight = AttackRange * 0.5f;
	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
    
	// DrawDebugCapsule 사용 예시 (혹은 UKismetSystemLibrary::DrawDebugSphereTraceSingle 사용 가능)
	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(ForwardVector).ToQuat(), DrawColor, false, 1.0f);


	// 5. 충돌 처리 로직
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
        
		// 여기에 데미지 전달 로직 추가 (예: ApplyDamage)
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), 10.0f, GetController(), this, UDamageType::StaticClass());
	}
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

void AZombieCharacter::OnDamageProcess(FVector hitDirection)
{
	//체력
	currentHP--;
	if (currentHP > 0)
	{
		//살아있음
		hitDirection.Z = 0;
		FVector force = hitDirection * knockbackPower;
		knockbackPos  = GetActorLocation() + force;
		// SetActorLocation(knockbackPos, true);
		
		float percent = GetWorld()->DeltaTimeSeconds * 10;
		FVector P = FMath::Lerp(GetActorLocation(), knockbackPos,percent);
		
		// 원충돌 거의 도착했다는걸 보장하기 위해.
		float dist = FVector::Dist(P, GetActorLocation());
		if (dist < 5)
		{
			P = GetActorLocation();
		}
		else
		{
			SetActorLocation(P, true);
		}
	}
	else
	{
		// OnDie();
	}
	

}


// void AZombieCharacter::OnDie()
// {
// 	SetActorEnableCollision(false);
// 	SetActorLocation(GetActorLocation() + (-GetActorUpVector() * 100 * GetWorld()->GetWorld()->DeltaTimeSeconds));
// 	if (GetActorLocation().Z < -80)
// 		Destroy();
// }

