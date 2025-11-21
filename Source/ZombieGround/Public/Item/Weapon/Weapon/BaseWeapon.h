// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"


// 1. 블루프린트에서 변수로 사용하려면 BlueprintType 필수
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	// 2. UMETA(DisplayName)은 에디터(드롭다운 메뉴)에서 보일 이름입니다.
	Unarmed         UMETA(DisplayName = "Unarmed"),
	Pistol          UMETA(DisplayName = "Pistol"),
	SMG             UMETA(DisplayName = "SMG"),
	AssaultRifle    UMETA(DisplayName = "Assault Rifle"),
    
	// 3. 최대값 (보통 반복문이나 범위 체크용으로 둠, 에디터엔 숨김)
	MAX             UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponName : uint8
{
	// 2. UMETA(DisplayName)은 에디터(드롭다운 메뉴)에서 보일 이름입니다.
	Unarmed         UMETA(DisplayName = "Unarmed"),
	Pistol          UMETA(DisplayName = "Pistol_A"),
	
	// 3. 최대값 (보통 반복문이나 범위 체크용으로 둠, 에디터엔 숨김)
	MAX             UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EFireMode : uint8
{
	// 2. UMETA(DisplayName)은 에디터(드롭다운 메뉴)에서 보일 이름입니다.
	SemiAuto         UMETA(DisplayName = "SemiAuto"),
	FullAuto          UMETA(DisplayName = "FullAuto"),
	
	// 3. 최대값 (보통 반복문이나 범위 체크용으로 둠, 에디터엔 숨김)
	MAX             UMETA(Hidden)
};

UCLASS()
class ZOMBIEGROUND_API ABaseWeapon : public AActor
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
	float Damage;
	
	UPROPERTY(EditAnywhere)
	float Range;
	
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
	UPROPERTY(EditAnywhere)
	EWeaponName WeaponName;
	
	UPROPERTY(EditAnywhere)
	class TSubclassOf<class ABasePickup> PickupClass;
	
	UPROPERTY(EditAnywhere)
	FName SocketName;
};
 