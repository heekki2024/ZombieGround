// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/Weapon/WeaponActor/BaseWeaponActor.h"

#include "Character/Human/HumanCharacter.h"
#include "Inventory/InventoryComponent.h"
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
	
	// 1. 이 위젯을 소유한 플레이어 캐릭터를 가져옴
	AHumanCharacter* ownerCharacter = Cast<AHumanCharacter>(GetOwner());

	if (ownerCharacter)
	{
		// 3. (선택사항) 시작하자마자 현재 상태 한 번 갱신 (초기값 0/0 방지)
		ownerCharacter->BroadcastCurrentAmmoUpdate();
		ownerCharacter->BroadcastInventoryAmmoUpdate(); 

	}
}

// Called every frame
void ABaseWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABaseWeaponActor::LoadWeaponInstance(class UWeaponInstance* updatedInstance)
{
	weaponInstance = updatedInstance;
	
	// 2. [데이터 주도] 데이터 에셋에 있는 Mesh를 내 몸에 입힘
	if (weaponInstance->actorMesh)
	{
		weaponMesh->SetSkeletalMesh(weaponInstance->actorMesh);
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
	if (bIsRightClicking == true)
	{
		if (weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Primary || 
			weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Secondary)
		{
			// 1회 즉시 발사 시도 (발사 불가능하면 그냥 무시됨)
			Fire();
		}

		// FullAuto만 타이머 연사
		if (weaponInstance->defaultWeaponData->fireMode == EFireMode::FullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(
				FireTimerHandle,
				this,
				&ABaseWeaponActor::Fire,
				weaponInstance->defaultWeaponData->weaponStats.fireRate,      // 타이머는 거의 즉시 호출되도록 매우 짧게
				true
			);
		}
	}
}

void ABaseWeaponActor::OnLeftClickReleased()
{
	// FullAuto일 경우 타이머 중지
	if (weaponInstance->defaultWeaponData->fireMode == EFireMode::FullAuto)
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
	if (weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Primary || 
		weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = true;
	}
}

void ABaseWeaponActor::OnRightClickReleased()
{
	if (weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Primary || 
		weaponInstance->defaultWeaponData->weaponSlot == EWeaponSlot::Secondary)
	{
		bIsRightClicking = false;
	}
}

void ABaseWeaponActor::Fire()
{
	// 0. 데이터 유효성 검사 (안전장치)
	if (!weaponInstance || !weaponInstance->defaultWeaponData) return;
	
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
	if (!weaponMesh) return;
	FVector MuzzleLocation = weaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));

	// -------------------------------
	// 3) Projectile 생성
	// -------------------------------
	if (weaponInstance->defaultWeaponData->projectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = OwnerPawn;

		GetWorld()->SpawnActor<ABaseProjectile>(
			weaponInstance->defaultWeaponData->projectileClass,
			MuzzleLocation,
			CameraRotation,
			SpawnParams
		);
	}

	// -------------------------------
	// 4) 총기 자체 발사 애니메이션 재생
	// -------------------------------
	if (weaponInstance->defaultWeaponData->tempGunAnim)
	{
		weaponMesh->PlayAnimation(weaponInstance->defaultWeaponData->tempGunAnim, false);
	}
	
	// -------------------------------
	// 4) 캐릭터 총기 발사 애니메이션 재생
	// -------------------------------
	if (weaponInstance->defaultWeaponData->playerAnimData.FireMontage)
	{
		// 캐릭터 SkeletalMesh 가져오기
		USkeletalMeshComponent* CharacterMesh = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>();
		UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
		AnimInstance->Montage_Play(weaponInstance->defaultWeaponData->playerAnimData.FireMontage);
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

	if (weaponInstance->defaultWeaponData->weaponFX.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			weaponInstance->defaultWeaponData->weaponFX.FireSound,
			GetActorLocation() // 소리는 총 위치에서 나야 자연스러움 (ShootDirection은 방향임)
		);
	}
	
	if (AHumanCharacter* Human = Cast<AHumanCharacter>(OwnerPawn))
	{
		// 아까 만든 델리게이트 호출 -> UI가 즉시 29발로 갱신됨
		Human->BroadcastCurrentAmmoUpdate();
	}
	
	// 다음 발사 가능 시간 갱신
	NextFireTime = currentTime + weaponInstance->defaultWeaponData->weaponStats.fireRate;
}

void ABaseWeaponActor::Reload()
{
	if (!weaponInstance || !weaponInstance->defaultWeaponData) return;
	
	// UWeaponDataAsset* defaultWeaponData = weaponInstance->defaultWeaponData;
	
	//이미 탄창이 꽉 찼으면 리턴
	if (weaponInstance->currentAmmo >= weaponInstance->maxAmmo) return;
	
	//필요한 탄약 수 계산
	int32 AmmoNeeded = weaponInstance->maxAmmo - weaponInstance->currentAmmo;
	
	//오너의 인벤토리 컴포넌트 가져오기
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;
	
	//AHumanCharacter로 캐스팅하거나 Interface를 사용하는 것이 좋음
	AHumanCharacter* ownerCharacter = Cast<AHumanCharacter>(MyOwner);
	UInventoryComponent* inventoryComponent = ownerCharacter->inventoryComponent;
	
	//인벤토리에 탄약 소비 요청
	int32 AmmoConsumed = inventoryComponent->ConsumeItem(weaponInstance->defaultWeaponData->weaponType, AmmoNeeded);
	//무기 탄창 채우기
	if (AmmoConsumed > 0)
	{
		weaponInstance->currentAmmo += AmmoConsumed;
		if (AHumanCharacter* Human = Cast<AHumanCharacter>(GetOwner()))
		{
			// 아까 만든 델리게이트 호출 -> UI가 즉시 29발로 갱신됨
			ownerCharacter->BroadcastCurrentAmmoUpdate();
			ownerCharacter->BroadcastInventoryAmmoUpdate(); 
		}
		UE_LOG(LogTemp, Log, TEXT("Reloaded! Current Ammo: %d"), weaponInstance->currentAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Ammo in Inventory!"));
	}
}



