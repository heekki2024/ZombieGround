// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"

#include "Character/Human/HumanCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Weapon/Projectile/BaseProjectile.h"
#include "Item/Instance/Weapon/BaseWeaponInstance.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ABaseWeaponActor::ABaseWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 루트 컴포넌트 없이 스켈레탈 메쉬를 루트로 씁니다.
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = mesh;

	// 무기는 캐릭터가 손에 들고 다니므로 물리 충돌은 보통 끕니다.
	mesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> tempCS
	(TEXT("/Game/Characters/Human/Blueprints/miscellaneous/BP_FireCameraShake.BP_FireCameraShake_C"));
	if (tempCS.Succeeded())
	{
		fireCameraShake = tempCS.Class;
	}
}

// Called when the game starts or when spawned
void ABaseWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	

	
	// 1. 이 위젯을 소유한 플레이어 캐릭터를 가져옴
	ownerCharacter = Cast<AHumanCharacter>(GetOwner());
	inventoryComponent = ownerCharacter->inventoryComponent;

	if (ownerCharacter)
	{
		// 3. (선택사항) 시작하자마자 현재 상태 한 번 갱신 (초기값 0/0 방지)
		ownerCharacter->BroadcastCurrentAmmoUpdate();
		ownerCharacter->BroadcastInventoryAmmoUpdate(); 

	}
	
}

void ABaseWeaponActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
    
	// 1. 돌아가고 있던 타이머 강제 종료
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(EquipTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(UnequipTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(AimTransitionTimerHandle);
	}

	// 2. WeaponInstance의 상태 해제
	if (IsValid(weaponInstance))
	{
		weaponInstance->bIsReloading = false;
		weaponInstance->bIsEquipping = false;
		weaponInstance->bIsUnequipping = false;
	}

	// 3. 캐릭터 애니메이션 정리 [삭제 또는 주석 처리]
	// 이유: 무기 교체 시(Swap), 이 코드가 새 무기의 Equip 애니메이션까지 꺼버림.
	/*
	if (IsValid(ownerCharacter))
	{
	   USkeletalMeshComponent* CharacterMesh = ownerCharacter->FindComponentByClass<USkeletalMeshComponent>();
	   
	   if (IsValid(CharacterMesh))
	   {
		  UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
		  if (IsValid(AnimInstance))
		  {
			 // [범인] 이 줄이 새 무기의 Equip 모션을 즉시 중단시킵니다.
			 // AnimInstance->Montage_Stop(0.0f, nullptr); 
		  }
	   }
	}
	*/
    
	UE_LOG(LogTemp, Log, TEXT("WeaponActor Destroyed: Timers Cleared."));
}

// Called every frame
void ABaseWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// [추가] 매 프레임 총기 열기를 식힘
	if (weaponInstance)
	{
		weaponInstance->UpdateSpread(DeltaTime);
	}
}



void ABaseWeaponActor::LoadWeaponInstance(class UBaseWeaponInstance* updatedInstance)
{
	weaponInstance = updatedInstance;
	
	// 2. [데이터 주도] 데이터 에셋에 있는 Mesh를 내 몸에 입힘
	if (weaponInstance->actorMesh)
	{
		mesh->SetSkeletalMesh(weaponInstance->actorMesh);
	}
	// 3. 부착물 생성 및 부착
	UpdateAttachments();
}

void ABaseWeaponActor::UpdateAttachments()
{
	// // 기존 부착물 싹 정리 (다시 그리기 위해)
	// for (AActor* Att : SpawnedAttachments)
	// {
	// 	if (Att) Att->Destroy();
	// }
	// SpawnedAttachments.Empty();
	//
	// // Instance에 저장된 부착물 리스트를 순회
	// // (WeaponInstance에 TArray<UAttachmentData*> Attachments가 있다고 가정)
	// /*
	// for (UAttachmentData* AttData : MyInstance->Attachments)
	// {
	// 	if (AttData && AttData->AttachmentActorClass)
	// 	{
	// 		// 부착물 액터 스폰
	// 		FActorSpawnParameters SpawnParams;
	// 		SpawnParams.Owner = this;
	// 		
	// 		AActor* NewAtt = GetWorld()->SpawnActor<AActor>(AttData->AttachmentActorClass, SpawnParams);
	// 		
	// 		// 총기의 특정 소켓에 부착 (예: ScopeSocket)
	// 		if (NewAtt)
	// 		{
	// 			NewAtt->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttData->SocketName);
	// 			SpawnedAttachments.Add(NewAtt);
	// 		}
	// 	}
	// }
	// */
}


void ABaseWeaponActor::OnLeftClickPressed()
{
	// [추가] 입력 상태 추적
	bIsLeftClickHeld = true;

	if(weaponInstance->bIsReloading == true) return;
	
	// 장전 중이거나, 꺼내는 중이거나, 넣는 중이면 발사 불가
	// [추가] ADS 전환 중(bIsAimTransitioning)에도 발사 불가
	if (weaponInstance->bIsReloading || weaponInstance->bIsEquipping || weaponInstance->bIsUnequipping || bIsAimTransitioning) 
		return;
	
	if (bIsRightClicking == true)
	{
		if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary || 
			weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
		{
			// 1회 즉시 발사 시도 (발사 불가능하면 그냥 무시됨)
			Fire();
		}

		// FullAuto만 타이머 연사
		if (weaponInstance->GetItemData<UWeaponDataAsset>()->fireMode == EFireMode::FullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(
				FireTimerHandle,
				this,
				&ABaseWeaponActor::Fire,
				weaponInstance->GetItemData<UWeaponDataAsset>()->weaponStats.fireRate,      // 타이머는 거의 즉시 호출되도록 매우 짧게
				true
			);
		}
	}
}

void ABaseWeaponActor::OnLeftClickReleased()
{
	// [추가] 입력 상태 해제
	bIsLeftClickHeld = false;

	// FullAuto일 경우 타이머 중지
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->fireMode == EFireMode::FullAuto)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		}
	}
}

void ABaseWeaponActor::OnRightClickPressed()
{
	// [추가] 장착/해제 중에는 조준 불가
	if (weaponInstance->bIsEquipping || weaponInstance->bIsUnequipping) return;

	// bIsAiming = true;
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary || 
		weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = true;
		
		// [추가] FOV 직접 제어
		if (ownerCharacter)
		{
			ownerCharacter->TargetFOV = ownerCharacter->AimingFOV;
		}

		// [추가] ADS 전환 중 발사 제한
		UWeaponDataAsset* Data = weaponInstance->GetItemData<UWeaponDataAsset>();
		if (Data && Data->weaponStats.LowReadyToAdsDuration > 0.0f)
		{
			bIsAimTransitioning = true;
			GetWorld()->GetTimerManager().ClearTimer(AimTransitionTimerHandle); // 안전장치
			GetWorld()->GetTimerManager().SetTimer(AimTransitionTimerHandle, this, &ABaseWeaponActor::FinishAimTransition, Data->weaponStats.LowReadyToAdsDuration, false);
		}
	}
}

void ABaseWeaponActor::OnRightClickReleased()
{
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary || 
		weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = false;
		
		// [추가] FOV 복구
		if (ownerCharacter)
		{
			ownerCharacter->TargetFOV = ownerCharacter->DefaultFOV;
		}

		// [추가] ADS 해제 중 발사 제한
		UWeaponDataAsset* Data = weaponInstance->GetItemData<UWeaponDataAsset>();
		if (Data && Data->weaponStats.AdsToLowReadyDuration > 0.0f)
		{
			bIsAimTransitioning = true;
			GetWorld()->GetTimerManager().ClearTimer(AimTransitionTimerHandle); // 안전장치
			GetWorld()->GetTimerManager().SetTimer(AimTransitionTimerHandle, this, &ABaseWeaponActor::FinishAimTransition, Data->weaponStats.AdsToLowReadyDuration, false);
		}
	}
}

// void ABaseWeaponActor::Fire()
// {
// 	// 0. 데이터 유효성 검사 (안전장치)
// 	if (!weaponInstance || !weaponInstance->GetItemData<UWeaponDataAsset>()) return;
// 	
// 	// 현재 시간
// 	double currentTime = GetWorld()->GetTimeSeconds();
//
// 	// 다음 발사까지 기다려야 함
// 	if (currentTime < NextFireTime)
// 		return;
//
// 	// ---------------------------------------------------------
// 	// [추가됨 1] 탄약 확인 (Ammo Check)
// 	// 총알이 없으면 발사 로직을 실행하지 않고 종료
// 	// ---------------------------------------------------------
// 	if (weaponInstance->currentAmmo <= 0)
// 	{
// 		// (선택사항) 빈 총 소리 재생 (찰칵!)
// 		// if (weaponInstance->defaultWeaponData->weaponFX.DryFireSound)
// 		// {
// 		//    UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation());
// 		// }
//         
// 		// 클릭 소리가 너무 자주 나지 않게 하려면 여기서도 NextFireTime 갱신 필요
// 		// NextFireTime = currentTime + 0.2f; 
// 		return; 
// 	}
// 	
// 	// ---- 실제 발사 로직 시작----
// 	weaponInstance->currentAmmo--;
// 	
// 	APawn* OwnerPawn = Cast<APawn>(GetOwner());
// 	if (!OwnerPawn) return;
//
// 	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
// 	if (!PC) return;
//
// 	FVector CameraLocation;
// 	FRotator CameraRotation;
// 	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
//
// 	FVector ShootDirection = CameraRotation.Vector();
//
// 	// -------------------------------
// 	// 2) 총구 위치 가져오기 (Muzzle 소켓)
// 	// -------------------------------
// 	if (!mesh) return;
// 	FVector MuzzleLocation = mesh->GetSocketLocation(TEXT("MuzzleFlash"));
//
// 	// -------------------------------
// 	// 3) Projectile 생성
// 	// -------------------------------
// 	if (weaponInstance->GetItemData<UWeaponDataAsset>()->projectileClass)
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.Owner = this;
// 		SpawnParams.Instigator = OwnerPawn;
//
// 		GetWorld()->SpawnActor<ABaseProjectile>(
// 			weaponInstance->GetItemData<UWeaponDataAsset>()->projectileClass,
// 			MuzzleLocation,
// 			CameraRotation,
// 			SpawnParams
// 		);
// 	}
//
// 	// -------------------------------
// 	// 4) 총기 자체 발사 애니메이션 재생
// 	// -------------------------------
// 	if (weaponInstance->GetItemData<UWeaponDataAsset>()->tempGunAnim)
// 	{
// 		mesh->PlayAnimation(weaponInstance->GetItemData<UWeaponDataAsset>()->tempGunAnim, false);
// 	}
// 	
// 	// -------------------------------
// 	// 4) 캐릭터 총기 발사 애니메이션 재생
// 	// -------------------------------
// 	if (weaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData.FireMontage)
// 	{
// 		// 캐릭터 SkeletalMesh 가져오기
// 		USkeletalMeshComponent* CharacterMesh = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>();
// 		UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
// 		AnimInstance->Montage_Play(weaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData.FireMontage);
// 	}
// 	
// 	
//
// 	// -------------------------------
// 	// 5) 총구 이펙트 & 사운드 (선택)
// 	// -------------------------------
// 	// if (MuzzleFlash)
// 	// {
// 	// 	UGameplayStatics::SpawnEmitterAttached(
// 	// 		MuzzleFlash,
// 	// 		gunMesh,
// 	// 		TEXT("MuzzleFlash")
// 	// 	);
// 	// }
//
// 	if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponFX.FireSound)
// 	{
// 		UGameplayStatics::PlaySoundAtLocation(
// 			this,
// 			weaponInstance->GetItemData<UWeaponDataAsset>()->weaponFX.FireSound,
// 			GetActorLocation() // 소리는 총 위치에서 나야 자연스러움 (ShootDirection은 방향임)
// 		);
// 	}
// 	
// 	if (AHumanCharacter* Human = Cast<AHumanCharacter>(OwnerPawn))
// 	{
// 		// 아까 만든 델리게이트 호출 -> UI가 즉시 29발로 갱신됨
// 		Human->BroadcastCurrentAmmoUpdate();
// 	}
// 	
// 	// 다음 발사 가능 시간 갱신
// 	NextFireTime = currentTime + weaponInstance->GetItemData<UWeaponDataAsset>()->weaponStats.fireRate;
// 	
// 	//카메라 셰이크 재생
// 	auto controller = GetWorld()->GetFirstPlayerController();
// 	controller->PlayerCameraManager->StartCameraShake(fireCameraShake);
// 	
// }

void ABaseWeaponActor::Fire()
{
	// 0. 데이터 유효성 검사
    if (!weaponInstance || !weaponInstance->GetItemData<UWeaponDataAsset>()) return;
    
    double currentTime = GetWorld()->GetTimeSeconds();
    if (currentTime < NextFireTime) return;

    // 탄약 확인
    if (weaponInstance->currentAmmo <= 0)
    {
       // 클릭 소리 등 처리...
       return; 
    }
    
    // =========================================================
    // [수정] 1. Lyra 로직 적용: 발사했으니 열기(Heat) 증가
    // =========================================================
    weaponInstance->AddSpread();


    // 실제 발사 로직
    weaponInstance->currentAmmo--;
    
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
    if (!PC) return;

    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // =========================================================
    // [수정] 2. 탄퍼짐(Spread) 적용하여 발사 방향 비틀기
    // =========================================================
    FVector ShootDirection = CameraRotation.Vector();
    
    // 인스턴스에서 현재 탄퍼짐 각도 가져오기
    float CurrentSpread = weaponInstance->GetCurrentSpread();
    
    // 탄퍼짐이 있다면 방향을 랜덤하게 휨 (VRandCone 사용)
    if (CurrentSpread > 0.0f)
    {
        // HalfAngleInRadians: 절반 각도를 라디안으로 변환해야 함
        ShootDirection = FMath::VRandCone(ShootDirection, FMath::DegreesToRadians(CurrentSpread * 0.5f));
    }
    
    // 휘어진 방향을 회전값으로 변환
    FRotator FinalMuzzleRotation = ShootDirection.Rotation();


    // 총구 위치
    if (!mesh) return;
    FVector MuzzleLocation = mesh->GetSocketLocation(TEXT("MuzzleFlash"));

    // =========================================================
    // [수정] 3. Projectile 생성 시 '비틀어진 회전값' 사용
    // =========================================================
    if (weaponInstance->GetItemData<UWeaponDataAsset>()->projectileClass)
    {
       FActorSpawnParameters SpawnParams;
       SpawnParams.Owner = this;
       SpawnParams.Instigator = OwnerPawn;

    	// 1. 총알 스폰
    	ABaseProjectile* NewBullet = GetWorld()->SpawnActor<ABaseProjectile>(
          weaponInstance->GetItemData<UWeaponDataAsset>()->projectileClass,
          MuzzleLocation,
          FinalMuzzleRotation, // <--- CameraRotation 대신 FinalMuzzleRotation 사용
          SpawnParams
       );
    	
    	// 2. [핵심] 데이터 에셋의 데미지 정보를 총알에 주입!
    	if (NewBullet)
    	{
    		UWeaponDataAsset* weaponDA = weaponInstance->GetItemData<UWeaponDataAsset>();
           
    		// InitProjectile 호출
    		NewBullet->InitProjectile(weaponDA->weaponStats.BaseDmg, weaponDA->weaponStats.HeadshotDmg, weaponDA->weaponStats.KnockbackStrength, weaponDA->weaponStats.Stun,  weaponDA->weaponStats.StunTime);
    	}
    }

    // ... (애니메이션, 사운드 재생 코드는 기존 유지) ...
    if (weaponInstance->GetItemData<UWeaponDataAsset>()->tempGunAnim)
    {
       mesh->PlayAnimation(weaponInstance->GetItemData<UWeaponDataAsset>()->tempGunAnim, false);
    }
    if (weaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData.FireMontage)
    {
       USkeletalMeshComponent* CharacterMesh = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>();
       UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
       AnimInstance->Montage_Play(weaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData.FireMontage);
    }
    if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponFX.FireSound)
    {
       UGameplayStatics::PlaySoundAtLocation(
          this,
          weaponInstance->GetItemData<UWeaponDataAsset>()->weaponFX.FireSound,
          GetActorLocation()
       );
    }
    if (AHumanCharacter* Human = Cast<AHumanCharacter>(OwnerPawn))
    {
       Human->BroadcastCurrentAmmoUpdate();
    }
    
    // 다음 발사 시간 갱신
    NextFireTime = currentTime + weaponInstance->GetItemData<UWeaponDataAsset>()->weaponStats.fireRate;
    
    // 카메라 셰이크 (기존 유지 - 시각적 흔들림)
    if(PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraShake(fireCameraShake);
    }

    // =========================================================
    // [추가] 4. 물리적 반동 (Recoil) 추가 - 에임이 실제로 위로 튐
    // =========================================================
	if (PC)
	{
		// 데이터 에셋 가져오기
		UWeaponDataAsset* Data = weaponInstance->GetItemData<UWeaponDataAsset>();
        
		if (Data)
		{
			// 데이터 에셋에 설정된 Min ~ Max 사이의 랜덤한 값을 추출
			float VerticalRecoil = FMath::RandRange(Data->weaponStats.RecoilPitchMin, Data->weaponStats.RecoilPitchMax);
			float HorizontalRecoil = FMath::RandRange(Data->weaponStats.RecoilYawMin, Data->weaponStats.RecoilYawMax);
            
			// 컨트롤러에 입력 적용
			PC->AddPitchInput(VerticalRecoil);
			PC->AddYawInput(HorizontalRecoil);
		}
	}
}

void ABaseWeaponActor::StartEquip()
{
	// 0. 데이터 유효성 검사
	if (!weaponInstance || !ownerCharacter) return;

	UWeaponDataAsset* WeaponDA = weaponInstance->GetItemData<UWeaponDataAsset>();
	if (!WeaponDA) return;

	// 기존 작업 취소
	GetWorld()->GetTimerManager().ClearTimer(UnequipTimerHandle);

	// 1. 상태 설정 (발사 못하게 막음)
	weaponInstance->bIsEquipping = true;
	weaponInstance->bIsUnequipping = false;

	float equipDuration = WeaponDA->weaponStats.equipDuration;
	
	UAnimMontage* equipMontage = WeaponDA->playerAnimData.EquipMontage;
	float MontageLength = equipMontage->GetPlayLength();

	// B. 3초 안에 끝내기 위한 배속 계산
	// 공식: (원본 길이 4.5초) / (목표 시간 3.0초) = 1.5배속
	float PlayRate = 1.0f;
	if (equipDuration > 0.0f)
	{
		PlayRate = MontageLength / equipDuration;
	}
	
	// // 안전장치: 재생 속도가 0 이하면 1.0으로 보정 (무한 대기 방지)
	// if (PlayRate <= 0.0f) PlayRate = 1.0f;

	USkeletalMeshComponent* CharMesh = ownerCharacter->GetMesh(); // 혹은 FindComponentByClass

	// [핵심 변경] 몽타주 직접 재생
	CharMesh->GetAnimInstance()->Montage_Play(equipMontage, PlayRate);
	
	// 3. 타이머 설정 (계산된 Duration만큼 대기 후 FinishEquip 호출)
	GetWorld()->GetTimerManager().SetTimer(EquipTimerHandle, this, &ABaseWeaponActor::FinishEquip, equipDuration, false);

	UE_LOG(LogTemp, Log, TEXT("Weapon Equip Started... Duration: %f"), equipDuration);
}

void ABaseWeaponActor::StartUnequip()
{
	// 0. 데이터 유효성 검사
	if (!weaponInstance || !ownerCharacter) 
	{
		// 데이터가 없으면 애니메이션 없이 바로 종료 처리
		FinishUnequip();
		return;
	}

	UWeaponDataAsset* WeaponDA = weaponInstance->GetItemData<UWeaponDataAsset>();
	if (!WeaponDA) 
	{
		FinishUnequip();
		return;
	}

	// 기존 Equip 작업 취소
	GetWorld()->GetTimerManager().ClearTimer(EquipTimerHandle);
    
	// 1. 상태 설정 (발사 불가, 장전 취소 등)
	weaponInstance->bIsUnequipping = true;
	weaponInstance->bIsEquipping = false; // Equip 중단
	weaponInstance->bIsReloading = false; // 장전 중이었다면 취소
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle); // 장전 타이머 킬

	float unequipDuration = WeaponDA->weaponStats.unequipDuration;
	
	UAnimMontage* unequipMontage = WeaponDA->playerAnimData.UnEquipMontage;
	float MontageLength = unequipMontage->GetPlayLength();

	// B. 3초 안에 끝내기 위한 배속 계산
	// 공식: (원본 길이 4.5초) / (목표 시간 3.0초) = 1.5배속
	float PlayRate = 1.0f;
	if (unequipDuration > 0.0f)
	{
		PlayRate = MontageLength / unequipDuration;
	}
	
	USkeletalMeshComponent* CharMesh = ownerCharacter->GetMesh(); // 혹은 FindComponentByClass

	// [핵심 변경] 몽타주 직접 재생
	CharMesh->GetAnimInstance()->Montage_Play(unequipMontage, PlayRate);
	

	// 3. 타이머 설정 (계산된 Duration만큼 대기 후 FinishUnequip 호출)
	GetWorld()->GetTimerManager().SetTimer(UnequipTimerHandle, this, &ABaseWeaponActor::FinishUnequip, unequipDuration, false);
    
	UE_LOG(LogTemp, Log, TEXT("Weapon Unequip Started... Duration: %f"), unequipDuration);
}

void ABaseWeaponActor::FinishEquip()
{
	if (weaponInstance)
	{
		weaponInstance->bIsEquipping = false; // 이제 발사 가능!
	}
    
	// [추가] 장착 완료 시점에 플레이어가 우클릭을 유지하고 있다면 즉시 조준 모드 진입
	if (ownerCharacter && ownerCharacter->bIsAiming)
	{
		OnRightClickPressed();
	}

	// 캐릭터에게 "장착 끝났다"고 알림 (필요시 UI 갱신 등)
	if (OnEquipFinished.IsBound())
	{
		OnEquipFinished.Broadcast();
	}
	UE_LOG(LogTemp, Log, TEXT("Weapon Equip Finished. Ready to Fire."));
}

void ABaseWeaponActor::FinishUnequip()
{
	if (weaponInstance)
	{
		weaponInstance->bIsUnequipping = false;
	}

	// 4. 캐릭터에게 "나 이제 사라진다"고 알림 (다음 무기 스폰을 위해)
	if (OnUnequipFinished.IsBound())
	{
		OnUnequipFinished.Broadcast();
	}

	// [변경 2] 할 일 다 했으니 스스로 파괴 (Self Destroy)
	Destroy();
}


void ABaseWeaponActor::TryReload()
{
	if (!weaponInstance || !weaponInstance->GetItemData<UWeaponDataAsset>()) return;
	
	//이미 탄창이 꽉 찼으면 리턴
	if (weaponInstance->currentAmmo >= weaponInstance->maxAmmo) return;
	
	// [수정] 인벤토리 탄약 OR 내부 예비 탄약 둘 중 하나라도 있어야 장전 시도
	bool bHasInventoryAmmo = inventoryComponent->GetItemQuantity(weaponInstance->GetItemData<UWeaponDataAsset>()) > 0;
	bool bHasInternalAmmo = weaponInstance->InternalReserveAmmo > 0;

	if (!bHasInventoryAmmo && !bHasInternalAmmo)
	{
		// 둘 다 없으면 장전 불가 (빈 소리 재생 등 가능)
		return;
	}
	
	UWeaponDataAsset* WeaponDA = weaponInstance->GetItemData<UWeaponDataAsset>();
	float reloadDuration = WeaponDA->weaponStats.reloadDuration;
	weaponInstance->bIsReloading = true;
	// 2. 애니메이션 재생 및 속도 자동 조절
	UAnimMontage* reloadMontage = WeaponDA->playerAnimData.ReloadMontage;
	float MontageLength = reloadMontage->GetPlayLength();
	
	// B. 3초 안에 끝내기 위한 배속 계산
	// 공식: (원본 길이 4.5초) / (목표 시간 3.0초) = 1.5배속
	float PlayRate = 1.0f;
	if (reloadDuration > 0.0f)
	{
		PlayRate = MontageLength / reloadDuration;
	}
	// C. 계산된 배속으로 재생 (두 번째 인자에 PlayRate 전달)
	USkeletalMeshComponent* CharacterMesh = ownerCharacter->FindComponentByClass<USkeletalMeshComponent>();
	CharacterMesh->GetAnimInstance()->Montage_Play(reloadMontage, PlayRate);
            
	UE_LOG(LogTemp, Log, TEXT("Reload Anim Speed: x%.2f (Original: %.2fs -> Target: %.2fs)"), PlayRate, MontageLength, reloadDuration);
	
	UE_LOG(LogTemp, Log, TEXT("Reload Started... (Wait 3s)"));
	// 3. 타이머 설정 (정확히 목표 시간 뒤에 완료)
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ABaseWeaponActor::FinishReload, reloadDuration, false);
	// (총기 자체 애니메이션 - 탄창 빠지는 모션 등)
	// if (SomeGunReloadAnim) { ... }
	// 3. 타이머 설정 (3초 뒤에 FinishReload 호출)
	// 3.0f 대신 데이터 에셋에 있는 ReloadTime 변수를 쓰면 더 좋음
}

void ABaseWeaponActor::FinishReload()
{
	// 장전 상태 해제
	if (weaponInstance)
	{
		weaponInstance->bIsReloading = false;
	}
	else
	{
		return;
	}
	
	//필요한 탄약 수 계산
	int32 AmmoNeeded = weaponInstance->maxAmmo - weaponInstance->currentAmmo;
	int32 TotalAmmoConsumed = 0;

	// [추가] 1. 내부 예비 탄약(보이지 않는 탄약) 먼저 소모
	if (weaponInstance->InternalReserveAmmo > 0)
	{
		int32 TakeFromInternal = FMath::Min(weaponInstance->InternalReserveAmmo, AmmoNeeded);
		weaponInstance->InternalReserveAmmo -= TakeFromInternal;
		TotalAmmoConsumed += TakeFromInternal;
		AmmoNeeded -= TakeFromInternal; // 남은 필요량 갱신

		UE_LOG(LogTemp, Log, TEXT("Reloaded from Internal Ammo. Used: %d, Remaining: %d"), TakeFromInternal, weaponInstance->InternalReserveAmmo);
	}

	// [수정] 2. 아직 부족하면 인벤토리에서 소모
	if (AmmoNeeded > 0)
	{
		// tryReload 중 탄약을 버렸을경우 체크. (단, 내부 탄약이 있었다면 인벤토리에 없어도 장전은 성공해야 함)
		// 따라서 인벤토리에 탄약이 있는지 체크하는 건 '내부 탄약도 없고 인벤토리 탄약도 없을 때'만 의미가 있음.
		
		inventoryComponent = ownerCharacter->inventoryComponent;
		if (inventoryComponent)
		{
			// 인벤토리에 탄약 소비 요청
			int32 InventoryConsumed = inventoryComponent->ConsumeItem(AmmoNeeded);
			TotalAmmoConsumed += InventoryConsumed;
		}
	}

	// 3. 최종적으로 무기 탄창 채우기
	if (TotalAmmoConsumed > 0)
	{
		weaponInstance->currentAmmo += TotalAmmoConsumed;
		
		// 델리게이트 호출 -> UI 갱신
		ownerCharacter->BroadcastCurrentAmmoUpdate();
		ownerCharacter->BroadcastInventoryAmmoUpdate();
		
		UE_LOG(LogTemp, Log, TEXT("Reloaded Complete! Current Ammo: %d"), weaponInstance->currentAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Reload Failed: No Ammo Available (Internal or Inventory)"));
	}
}

void ABaseWeaponActor::FinishAimTransition()
{
	bIsAimTransitioning = false;
	
	UE_LOG(LogTemp, Log, TEXT("FinishAimTransition Called. bIsLeftClickHeld: %d"), bIsLeftClickHeld);

	// [추가] 전환이 끝났을 때 좌클릭을 누르고 있다면 즉시 발사 로직 실행
	if (bIsLeftClickHeld)
	{
		OnLeftClickPressed();
	}
}



