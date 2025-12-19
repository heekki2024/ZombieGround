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
	
	// 1. 돌아가고 있던 장전 타이머 강제 종료
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}

	// 2. WeaponInstance의 장전 중 상태 해제
	// IsValid를 사용하여 GC된 객체에 접근하는 것을 방지
	if (IsValid(weaponInstance))
	{
		weaponInstance->bIsReloading = false;
	}

	// 3. 캐릭터 애니메이션 정리
	if (IsValid(ownerCharacter))
	{
		USkeletalMeshComponent* CharacterMesh = ownerCharacter->FindComponentByClass<USkeletalMeshComponent>();
		
		if (IsValid(CharacterMesh))
		{
			UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				AnimInstance->Montage_Stop(0.0f, nullptr);
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("WeaponActor Destroyed: Reload Cancelled & Cleaned up."));
}

// Called every frame
void ABaseWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if(weaponInstance->bIsReloading == true) return;
	
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
	// bIsAiming = true;
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary || 
		weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = true;
	}
}

void ABaseWeaponActor::OnRightClickReleased()
{
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Primary || 
		weaponInstance->GetItemData<UWeaponDataAsset>()->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = false;
	}
}

void ABaseWeaponActor::Fire()
{
	// 0. 데이터 유효성 검사 (안전장치)
	if (!weaponInstance || !weaponInstance->GetItemData<UWeaponDataAsset>()) return;
	
	// 현재 시간
	double currentTime = GetWorld()->GetTimeSeconds();

	// 다음 발사까지 기다려야 함
	if (currentTime < NextFireTime)
		return;

	// ---------------------------------------------------------
	// [추가됨 1] 탄약 확인 (Ammo Check)
	// 총알이 없으면 발사 로직을 실행하지 않고 종료
	// ---------------------------------------------------------
	if (weaponInstance->currentAmmo <= 0)
	{
		// (선택사항) 빈 총 소리 재생 (찰칵!)
		// if (weaponInstance->defaultWeaponData->weaponFX.DryFireSound)
		// {
		//    UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation());
		// }
        
		// 클릭 소리가 너무 자주 나지 않게 하려면 여기서도 NextFireTime 갱신 필요
		// NextFireTime = currentTime + 0.2f; 
		return; 
	}
	
	// ---- 실제 발사 로직 시작----
	weaponInstance->currentAmmo--;
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector ShootDirection = CameraRotation.Vector();

	// -------------------------------
	// 2) 총구 위치 가져오기 (Muzzle 소켓)
	// -------------------------------
	if (!mesh) return;
	FVector MuzzleLocation = mesh->GetSocketLocation(TEXT("MuzzleFlash"));

	// -------------------------------
	// 3) Projectile 생성
	// -------------------------------
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->projectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = OwnerPawn;

		GetWorld()->SpawnActor<ABaseProjectile>(
			weaponInstance->GetItemData<UWeaponDataAsset>()->projectileClass,
			MuzzleLocation,
			CameraRotation,
			SpawnParams
		);
	}

	// -------------------------------
	// 4) 총기 자체 발사 애니메이션 재생
	// -------------------------------
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->tempGunAnim)
	{
		mesh->PlayAnimation(weaponInstance->GetItemData<UWeaponDataAsset>()->tempGunAnim, false);
	}
	
	// -------------------------------
	// 4) 캐릭터 총기 발사 애니메이션 재생
	// -------------------------------
	if (weaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData.FireMontage)
	{
		// 캐릭터 SkeletalMesh 가져오기
		USkeletalMeshComponent* CharacterMesh = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>();
		UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
		AnimInstance->Montage_Play(weaponInstance->GetItemData<UWeaponDataAsset>()->playerAnimData.FireMontage);
	}
	
	

	// -------------------------------
	// 5) 총구 이펙트 & 사운드 (선택)
	// -------------------------------
	// if (MuzzleFlash)
	// {
	// 	UGameplayStatics::SpawnEmitterAttached(
	// 		MuzzleFlash,
	// 		gunMesh,
	// 		TEXT("MuzzleFlash")
	// 	);
	// }

	if (weaponInstance->GetItemData<UWeaponDataAsset>()->weaponFX.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			weaponInstance->GetItemData<UWeaponDataAsset>()->weaponFX.FireSound,
			GetActorLocation() // 소리는 총 위치에서 나야 자연스러움 (ShootDirection은 방향임)
		);
	}
	
	if (AHumanCharacter* Human = Cast<AHumanCharacter>(OwnerPawn))
	{
		// 아까 만든 델리게이트 호출 -> UI가 즉시 29발로 갱신됨
		Human->BroadcastCurrentAmmoUpdate();
	}
	
	// 다음 발사 가능 시간 갱신
	NextFireTime = currentTime + weaponInstance->GetItemData<UWeaponDataAsset>()->weaponStats.fireRate;
	
	//카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(fireCameraShake);
	
}

// void ABaseWeaponActor::Reload()
//
// {
//
// 	if (!weaponInstance || !weaponInstance->GetItemData<UWeaponDataAsset>()) return;
//
//
// 	// UWeaponDataAsset* defaultWeaponData = weaponInstance->defaultWeaponData;
//
//
// 	//이미 탄창이 꽉 찼으면 리턴
//
// 	if (weaponInstance->currentAmmo >= weaponInstance->maxAmmo) return;
//
//
// 	//필요한 탄약 수 계산
//
// 	int32 AmmoNeeded = weaponInstance->maxAmmo - weaponInstance->currentAmmo;
//
//
// 	//오너의 인벤토리 컴포넌트 가져오기
//
// 	AActor* MyOwner = GetOwner();
//
// 	if (!MyOwner) return;
//
//
// 	//AHumanCharacter로 캐스팅하거나 Interface를 사용하는 것이 좋음
//
// 	AHumanCharacter* ownerCharacter = Cast<AHumanCharacter>(MyOwner);
//
// 	inventoryComponent = ownerCharacter->inventoryComponent;
//
//
// 	//인벤토리에 탄약 소비 요청
//
// 	int32 AmmoConsumed = inventoryComponent->ConsumeItem(AmmoNeeded);
//
// 	//무기 탄창 채우기
//
// 	if (AmmoConsumed > 0)
//
// 	{
//
// 		weaponInstance->currentAmmo += AmmoConsumed;
//
// 		if (AHumanCharacter* Human = Cast<AHumanCharacter>(GetOwner()))
//
// 		{
//
// 			// 아까 만든 델리게이트 호출 -> UI가 즉시 29발로 갱신됨
//
// 			ownerCharacter->BroadcastCurrentAmmoUpdate();
//
// 			ownerCharacter->BroadcastInventoryAmmoUpdate();
//
// 		}
//
// 		UE_LOG(LogTemp, Log, TEXT("Reloaded! Current Ammo: %d"), weaponInstance->currentAmmo);
//
// 	}
//
// 	else
//
// 	{
//
// 		UE_LOG(LogTemp, Warning, TEXT("No Ammo in Inventory!"));
//
// 	}
//
// }
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



