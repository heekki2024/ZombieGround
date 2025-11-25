// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "BaseWeapon.generated.h"

// 1. 블루프린트에서 변수로 사용하려면 BlueprintType 필수
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	// 2. UMETA(DisplayName)은 에디터(드롭다운 메뉴)에서 보일 이름입니다.
	Unarmed         UMETA(DisplayName = "Unarmed"),
	Pistol          UMETA(DisplayName = "Pistol"),
	AssaultRifle    UMETA(DisplayName = "Assault Rifle"),
	MachineGun      UMETA(DisplayName = "Machinegun"),
	SMG             UMETA(DisplayName = "SMG"),
	SniperRifle		UMETA(DisplayName = "SniperRifle"),
	Shotgun			UMETA(DisplayName = "Shotgun"),
	
    
	// 3. 최대값 (보통 반복문이나 범위 체크용으로 둠, 에디터엔 숨김)
	MAX             UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponName : uint8
{
	// 2. UMETA(DisplayName)은 에디터(드롭다운 메뉴)에서 보일 이름입니다.
	Unarmed         UMETA(DisplayName = "Unarmed"),
	Pistol_A          UMETA(DisplayName = "Pistol_A"),
	Assault_Rifle_A		UMETA(DisplayName = "Assault_Rifle_A"),
	// 3. 최대값 (보통 반복문이나 범위 체크용으로 둠, 에디터엔 숨김)
	MAX             UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	// 2. UMETA(DisplayName)은 에디터(드롭다운 메뉴)에서 보일 이름입니다.
	SemiAuto         UMETA(DisplayName = "SemiAuto"),
	FullAuto          UMETA(DisplayName = "FullAuto"),
	ManualAction		UMETA(DisplayName = "ManualAction"),
	
	// 3. 최대값 (보통 반복문이나 범위 체크용으로 둠, 에디터엔 숨김)
	MAX             UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FWeaponAnimSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* aimDownSight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* lowReady;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* equip;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* unEquip;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* reload;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* fire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBlendSpace* AimOffset;
};

USTRUCT(BlueprintType)
struct FWeaponDetails
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere)
	float damage;

	UPROPERTY(EditAnywhere)
	float range;

	UPROPERTY(EditAnywhere)
	EWeaponType weaponType;

	UPROPERTY(EditAnywhere)
	EWeaponName weaponName;
	
	UPROPERTY(EditAnywhere)
	FName socketName;
};

UCLASS()
class ZOMBIEGROUND_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	
public:
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* gunMesh;
	
	UPROPERTY(EditAnywhere)
	class TSubclassOf<class ABasePickup> pickupClass;
	
	UPROPERTY(EditAnywhere)
	FWeaponDetails weaponDetails;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnimSet playerAnim;
	
};
