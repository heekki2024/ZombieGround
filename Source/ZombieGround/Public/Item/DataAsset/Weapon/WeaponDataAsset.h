// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DataAsset/BaseDataAsset.h"
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

	// [추가] 총기의 기본 데미지 (Body Shot 기준)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseDmg = 30.0f; 

	// [추가] 헤드샷 배율 (예: 1.5배, 2.0배 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float HeadshotDmg = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float KnockbackStrength = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Stun = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float StunTime = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fireRate = 0.1f;    // 연사 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float range = 5000.0f;    // 사거리

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 maxAmmo = 30;  // 탄창 용량
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float reloadDuration = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ADSWalkSpeed = 350.f;
	
	// [추가] 장착/해제 속도 (기본 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float equipDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float unequipDuration = 1.0f;

	// [추가] ADS 전환 시간 (LowReady -> ADS)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float LowReadyToAdsDuration = 0.2f;

	// [추가] ADS 해제 시간 (ADS -> LowReady)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AdsToLowReadyDuration = 0.2f;
	
	// [Lyra 스타일] 탄퍼짐 및 열기 관리 데이터
    UPROPERTY(EditDefaultsOnly, Category = "Lyra Logic")
    class UCurveFloat* HeatToSpreadCurve; // 열기에 따른 탄퍼짐 각도 그래프
    
	// [수정] Lyra 스타일 커브 (추가)
	// "현재 열기(X)에 따라 이번에 추가될 열기량(Y)을 결정하는 그래프"
	UPROPERTY(EditDefaultsOnly, Category = "Lyra Logic")
	class UCurveFloat* HeatToHeatPerShotCurve;
	
    UPROPERTY(EditDefaultsOnly, Category = "Lyra Logic")
    class UCurveFloat* HeatToCooldownCurve; // 열기가 식는 속도 그래프
    
    UPROPERTY(EditDefaultsOnly, Category = "Lyra Logic")
    float HeatPerShot = 1.0f; // 한 발 쏠 때 오르는 열기
    
    UPROPERTY(EditDefaultsOnly, Category = "Lyra Logic")
    float CoolingDelay = 0.5f; // 사격 중지 후 열기가 식기 시작하는 시간
	
	// 수직 반동 최소값 (음수여야 위로 올라갑니다. 예: -0.5)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	float RecoilPitchMin = -0.5f;

	// 수직 반동 최대값 (예: -1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	float RecoilPitchMax = -1.0f;

	// 수평 반동 최소값 (왼쪽으로 튀는 정도. 예: -0.5)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	float RecoilYawMin = -0.2f;

	// 수평 반동 최대값 (오른쪽으로 튀는 정도. 예: 0.5)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	float RecoilYawMax = 0.2f;
	
	// UPROPERTY(EditAnywhere, Category = "Ammo")
	// int32 magazineCapacity; // 탄창 용량 (예: 30발)
	
	// UPROPERTY(EditAnywhere, Category = "Ammo")
	// class UAmmoDataAsset* AmmoItemData; // 이 무기가 사용하는 탄약 아이템 데이터 (예: 5.56mm Ammo DataAsset)
};

// --- Structs (관련된 데이터끼리 묶기) ---

// 1. 애니메이션 관련 데이터 묶음
USTRUCT(BlueprintType)
struct FPlayerAnimData
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBlendSpace* AO_ADS;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* AO_ADS_BasePose; // 위/아래 조준
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* LowReady; // 평상시 포즈 (Idle)
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* EquipMontage; // 꺼내기
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* UnEquipMontage; // 집어넣기
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadMontage;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* FireMontage; // 발사


	
	
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
class ZOMBIEGROUND_API UWeaponDataAsset : public UBaseDataAsset
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
	
	// 발사체 클래스 (총알) - 하나로 통일!
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<AActor> projectileClass;
	
	// =============================================================
	// 2. 비주얼 (Visuals)
	// =============================================================
	

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
	UAnimSequence* tempGunAnim;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	// TSubclassOf<class ABaseWeaponPickup> pickupClass;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	// TSubclassOf<class ABaseWeaponActor> actorClass;
	//
	
	
	// 소켓 이름 정의 
	const FName rHandRifleSocketName = FName("Rifle_Socket");
	const FName rHandPistolSocketName = FName("Pistol_Socket");
};

