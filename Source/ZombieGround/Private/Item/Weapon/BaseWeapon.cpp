// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/BaseWeapon.h"

#include "Projectile/BaseProjectile.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 기존 RootComponent 또는 캐릭터 메쉬에 Attach
	gunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::OnLeftClickPressed()
{
	Super::OnLeftClickPressed();
	
	if (bIsRightClicking)
	{
		if (weaponDetails.weaponType == EWeaponType::AssaultRifle ||
			weaponDetails.weaponType == EWeaponType::LMG ||
			weaponDetails.weaponType == EWeaponType::SMG ||
			weaponDetails.weaponType == EWeaponType::SniperRifle ||
			weaponDetails.weaponType == EWeaponType::Shotgun ||
			weaponDetails.weaponType == EWeaponType::Pistol)
			{
				// 1회 즉시 발사 시도 (발사 불가능하면 그냥 무시됨)
				Fire();
			}

			// FullAuto만 타이머 연사
			if (weaponDetails.fireMode == EFireMode::FullAuto)
			{
				GetWorld()->GetTimerManager().SetTimer(
					FireTimerHandle,
					this,
					&ABaseWeapon::Fire,
					FireRate,      // 타이머는 거의 즉시 호출되도록 매우 짧게
					true
				);
			}
	}

}

void ABaseWeapon::OnLeftClickReleased()
{
	Super::OnLeftClickReleased();
	
	// FullAuto일 경우 타이머 중지
	if (weaponDetails.fireMode == EFireMode::FullAuto)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		}
	}
}

void ABaseWeapon::OnRightClickPressed()
{
	Super::OnRightClickPressed();
	// bIsAiming = true;
	if (weaponDetails.weaponType == EWeaponType::AssaultRifle ||
		weaponDetails.weaponType == EWeaponType::LMG ||
		weaponDetails.weaponType == EWeaponType::SMG ||
		weaponDetails.weaponType == EWeaponType::SniperRifle ||
		weaponDetails.weaponType == EWeaponType::Shotgun ||
		weaponDetails.weaponType == EWeaponType::Pistol)
	{
		bIsRightClicking = true;
	}
}

void ABaseWeapon::OnRightClickReleased()
{
	Super::OnRightClickReleased();
	
	if (weaponDetails.weaponType == EWeaponType::AssaultRifle ||
		weaponDetails.weaponType == EWeaponType::LMG ||
		weaponDetails.weaponType == EWeaponType::SMG ||
		weaponDetails.weaponType == EWeaponType::SniperRifle ||
		weaponDetails.weaponType == EWeaponType::Shotgun ||
		weaponDetails.weaponType == EWeaponType::Pistol)
	{
		bIsRightClicking = false;
	}
}

void ABaseWeapon::Fire()
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
	if (!gunMesh) return;
	FVector MuzzleLocation = gunMesh->GetSocketLocation(TEXT("MuzzleFlash"));

	// -------------------------------
	// 3) Projectile 생성
	// -------------------------------
	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = OwnerPawn;

		GetWorld()->SpawnActor<ABaseProjectile>(
			ProjectileClass,
			MuzzleLocation,
			CameraRotation,
			SpawnParams
		);
	}

	// -------------------------------
	// 4) 총기 자체 발사 애니메이션 재생
	// -------------------------------
	if (TempGunAction)
	{
		gunMesh->PlayAnimation(TempGunAction, false);
	}
	
	// -------------------------------
	// 4) 캐릭터 총기 발사 애니메이션 재생
	// -------------------------------
	if (weaponAnimSet.fire)
	{
		// 캐릭터 SkeletalMesh 가져오기
		USkeletalMeshComponent* CharacterMesh = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>();
		UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
		AnimInstance->Montage_Play(weaponAnimSet.fire);
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

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			ShootDirection
		);
	}
	
	// 다음 발사 가능 시간 갱신
	NextFireTime = CurrentTime + FireRate;
}



