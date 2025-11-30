// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DataAsset/BaseItemDataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */

// --- Enums ---

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary,    // 이름 간소화 (PrimaryWeapon -> Primary)
	Secondary,
	Melee,
	Throwable,
	Max
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	Knife,
	Pistol,
	AssaultRifle,
	LMG,
	SMG,
	SniperRifle,
	Shotgun,
	MAX             UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto,
	FullAuto,
	ManualAction, // 볼트액션, 펌프액션 등
	MAX             UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float baseDamage = 10.0f; // 기본값 설정 추천

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fireRate = 0.1f;    // 연사 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float range = 5000.0f;    // 사거리

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 maxAmmo = 30;  // 탄창 용량

};

// --- Structs (관련된 데이터끼리 묶기) ---

// 1. 애니메이션 관련 데이터 묶음
USTRUCT(BlueprintType)
struct FPlayerAnimData
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* AimDownSight; // 줌 상태 포즈
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* LowReady; // 평상시 포즈 (Idle)
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* Equip; // 꺼내기
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* UnEquip; // 집어넣기
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadMontage;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* FireMontage; // 발사

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBlendSpace* AimOffset; // 위/아래 조준
};


// 2. 이펙트 및 사운드 데이터 묶음
USTRUCT(BlueprintType)
struct FWeaponFXData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* MuzzleFlash; // 총구 화염

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* FireSound; // 발사 소리

	// 탄피 배출 이펙트 등이 추가될 수 있음
};


UCLASS()
class ZOMBIEGROUND_API UWeaponDataAsset : public UBaseItemDataAsset
{
	GENERATED_BODY()
	
public:
	// =============================================================
	// 1. 기본 설정 (Setup)
	// =============================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EWeaponSlot weaponSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EWeaponType weaponType;

	// 발사 모드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EFireMode fireMode;

	// 실제 스폰될 무기 액터 클래스 (블루프린트)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<class ABaseWeaponActor> weaponActorClass;
    
	// [Equip 상태] 손에 들었을 때 보여줄 메쉬 (애니메이션 필요)
	// 장전, 발사 애니메이션을 위해 SkeletalMesh 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	class USkeletalMesh* weaponActorMesh;
	
	
	// 발사체 클래스 (총알) - 하나로 통일!
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<AActor> projectileClass;
	
	// =============================================================
	// 2. 비주얼 (Visuals)
	// =============================================================

	// 장착 시 사용할 스켈레탈 메쉬
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	USkeletalMesh* weaponMesh;

	// // 총구 소켓 이름 (이펙트/총알 나가는 위치)
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	// FName muzzleSocketName = FName("MuzzleFlash");

	// 위에서 만든 FX 구조체 사용 -> 에디터에서 깔끔하게 접힘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FWeaponFXData weaponFX;
	
	// =============================================================
	// 3. 성능 (Stats)
	// =============================================================
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	FWeaponStats weaponStats;
	
	
	// =============================================================
	// 4. 애니메이션 (Animations)
	// =============================================================

	// 위에서 만든 Anim 구조체 사용 -> 변수 하나로 퉁침!
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FPlayerAnimData playerAnimData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimSequence* TempGunAnim;
	
	UPROPERTY()
	class UBaseWeaponActor* weaponActorToSpawn;
	
	UPROPERTY()
	class UBaseWeaponPickup* weaponPickupToSpawn;
	
	// 소켓 이름 정의 
	const FName rHandRifleSocketName = FName("RifleSocket");
	const FName rHandPistolSocketName = FName("PistolSocket");
};

