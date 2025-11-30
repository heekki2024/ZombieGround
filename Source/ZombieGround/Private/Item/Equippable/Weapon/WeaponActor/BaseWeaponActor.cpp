// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"

#include "Item/DataAsset/Weapon/WeaponDataAsset.h"
#include "Item/Equippable/Weapon/Projectile/BaseProjectile.h"
#include "Item/Instance/Weapon/WeaponInstance.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseWeaponActor::ABaseWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 루트 컴포넌트 없이 스켈레탈 메쉬를 루트로 씁니다.
	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = weaponMesh;

	// 무기는 캐릭터가 손에 들고 다니므로 물리 충돌은 보통 끕니다.
	weaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

}

// Called when the game starts or when spawned
void ABaseWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABaseWeaponActor::InitializeWeapon(class UWeaponInstance* InInstance)
{
	if (!InInstance) return;
	
	weaponInstance = InInstance;
	weaponDataAsset = Cast<UWeaponDataAsset>(InInstance);
	
	if (!weaponDataAsset) return;

	// 2. [데이터 주도] 데이터 에셋에 있는 Mesh를 내 몸에 입힘
	if (weaponDataAsset->weaponActorMesh)
	{
		weaponMesh->SetSkeletalMesh(weaponDataAsset->weaponActorMesh);
	}
	
	// 3. 부착물 생성 및 부착
	UpdateAccessories();
}

void ABaseWeaponActor::UpdateAccessories()
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
	if (bIsRightClicking == true)
	{
		if (weaponDataAsset->weaponSlot == EWeaponSlot::Primary || 
			weaponDataAsset->weaponSlot == EWeaponSlot::Secondary)
		{
			// 1회 즉시 발사 시도 (발사 불가능하면 그냥 무시됨)
			Fire();
		}

		// FullAuto만 타이머 연사
		if (weaponDataAsset->fireMode == EFireMode::FullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(
				FireTimerHandle,
				this,
				&ABaseWeaponActor::Fire,
				weaponDataAsset->weaponStats.FireRate,      // 타이머는 거의 즉시 호출되도록 매우 짧게
				true
			);
		}
	}
}

void ABaseWeaponActor::OnLeftClickReleased()
{
	// FullAuto일 경우 타이머 중지
	if (weaponDataAsset->fireMode == EFireMode::FullAuto)
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
	if (weaponDataAsset->weaponSlot == EWeaponSlot::Primary || 
		weaponDataAsset->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = true;
	}
}

void ABaseWeaponActor::OnRightClickReleased()
{
	if (weaponDataAsset->weaponSlot == EWeaponSlot::Primary || 
		weaponDataAsset->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = false;
	}
}

void ABaseWeaponActor::Fire()
{
	// 현재 시간
	double CurrentTime = GetWorld()->GetTimeSeconds();

	// 다음 발사까지 기다려야 함
	if (CurrentTime < NextFireTime)
		return;

	// ---- 실제 발사 ----
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
	if (!weaponMesh) return;
	FVector MuzzleLocation = weaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));

	// -------------------------------
	// 3) Projectile 생성
	// -------------------------------
	if (weaponDataAsset->projectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = OwnerPawn;

		GetWorld()->SpawnActor<ABaseProjectile>(
			weaponDataAsset->projectileClass,
			MuzzleLocation,
			CameraRotation,
			SpawnParams
		);
	}

	// -------------------------------
	// 4) 총기 자체 발사 애니메이션 재생
	// -------------------------------
	if (weaponDataAsset->TempGunAnim)
	{
		weaponMesh->PlayAnimation(weaponDataAsset->TempGunAnim, false);
	}
	
	// -------------------------------
	// 4) 캐릭터 총기 발사 애니메이션 재생
	// -------------------------------
	if (weaponDataAsset->playerAnimData.FireMontage)
	{
		// 캐릭터 SkeletalMesh 가져오기
		USkeletalMeshComponent* CharacterMesh = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>();
		UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
		AnimInstance->Montage_Play(weaponDataAsset->playerAnimData.FireMontage);
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

	if (weaponDataAsset->weaponFX.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			weaponDataAsset->weaponFX.FireSound,
			ShootDirection
		);
	}
	
	// 다음 발사 가능 시간 갱신
	NextFireTime = CurrentTime + weaponDataAsset->weaponStats.FireRate;
}

