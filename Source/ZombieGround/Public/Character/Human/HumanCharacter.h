// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Character/BaseCharacter.h"
#include "HumanCharacter.generated.h"

class AZombieCharacter;

// 1. 이벤트 정의 (int32 두 개를 방송: 현재탄, 최대탄)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoChangedDelegate, int32, currentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryAmmoChangedDelegate, int32, inventoryAmmo);
// [추가] 스태미나 변경 알림용 델리게이트 (UI와 연동)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);
UCLASS()
class ZOMBIEGROUND_API AHumanCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHumanCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, Category = "Zombie")
	TSubclassOf<AZombieCharacter> ZombieClassToSpawn;
	

protected:
	// ... 기존 변수들 ...

	// Ease-In 곡선의 휘어짐 정도 (기본값 2.0)
	// 1.0 = 직선(선형), 2.0 = 제곱(부드러움), 3.0~ = 급격한 변화
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float runEaseExp = 2.0f; 

	// 가속이 걸리는 시간 (초 단위) - 이 시간이 지나야 최고 속도 도달
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float runAccelerationTime = 2.0f;

private:
	// 0.0 ~ 1.0 사이의 값. 현재 가속도가 얼마나 붙었는지 저장
	float runAlpha = 0.0f;
	
	
	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	UCapsuleComponent* InteractionCapsule;
	
	// --- 마우스 옵션 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Mouse")
	float MouseSensitivity = 0.5f;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputMappingContext* IMC_HumanPlayer;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Move;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Look;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Jump;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Interact;
	
	//공격
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_MouseLeftClick;
	
	//Mode
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_MouseRightClick;
	
	//1번키
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Num1Key;

	//2번키
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Num2Key;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Reload;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* IA_Tab;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* IA_Run; // 에디터에서 할당할 Sprint 액션
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* IA_Toggle; // 에디터에서 할당할 Sprint 액션
	
	
	//Input 함수
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void JumpAction(const struct FInputActionValue& Value);
	void Interact(const struct FInputActionValue& Value);
	void OnRightClickPressed(const struct FInputActionValue& Value);
	void OnRightClickReleased(const struct FInputActionValue& Value);
	void OnLeftClickPressed(const struct FInputActionValue& Value);
	void OnLeftClickReleased(const struct FInputActionValue& Value);
	void OnNum1KeyPressed(const struct FInputActionValue& Value);
	void OnNum2KeyPressed(const struct FInputActionValue& Value);
	void Reload(const struct FInputActionValue& Value);
	void OnTabPressed(const struct FInputActionValue& Value);
	void Toggle(const struct FInputActionValue& Value);

public:
	UPROPERTY()
	APlayerController* PC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	class UInventoryComponent* inventoryComponent;
	
	// // 현재 장착하고 있는 무기 인스턴스를 저장할 변수
	// UPROPERTY(EditAnywhere)
	// class ABaseWeaponActor* currentWeaponActor;
	//
	//
	// UPROPERTY(EditAnywhere)	
	// class UBaseWeaponInstance* currentWeaponInstance;

	
	UPROPERTY()
	TSet<AActor*> OverlappingInteractables;

	UFUNCTION()
	void OnInteractableBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractableEndOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex);
	
public:
	UFUNCTION()
	AActor* GetCenterScreenInteractable();
	
	//이전에 하이라이트된 액터 저장용 변수
	UPROPERTY()
	class AActor* outLinedInteractable = nullptr;
	
	UFUNCTION()
	void SetInteractableOutline(AActor* interactable, bool bEnable);
	
	UFUNCTION()
	void UpdateInteractableHighlight();
	
public:
	//Interaction 관련
	UPROPERTY()
	class ABaseWeaponActor* BaseWeapon;
	
	UPROPERTY()
	class UHumanHUD* humanHud;
	

public:
	//무기 총 UI 
	// 2. 방송 송출기 (블루프린트 UI에서 이걸 구독함)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCurrentAmmoChangedDelegate OnCurrentAmmoChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCurrentAmmoChangedDelegate OnInventoryAmmoChanged;

	// 3. 방송을 내보내는 함수 (무기나 인벤토리가 호출할 예정)
	UFUNCTION(BlueprintCallable)
	void BroadcastCurrentAmmoUpdate();	
	
	UFUNCTION(BlueprintCallable)
	void BroadcastInventoryAmmoUpdate();
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 350.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float runSpeed = 600.f;
	// 현재 달리기 키를 누르고 있는지 확인하는 플래그
	bool bWantsToSprint = false;
	
	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void UpdateRunSpeed(float DeltaTime);
	
	
	//스태미나
	
public:
	// [추가] 델리게이트 인스턴스
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaChanged OnStaminaChanged;
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float CurrentStamina;

	// 초당 소모량
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaDrainRate = 10.0f; 

	// 초당 회복량
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenRate = 5.0f;

	// 달리기를 시작하기 위한 최소 스태미나 (0이 되자마자 바로 다시 달리는 것 방지)
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MinStaminaToRun = 10.0f;

	// 점프 시 즉시 소모되는 스태미나 양 (예: 30)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float JumpStaminaCost = 20.0f;
	
	// 스태미나 소모 후 회복이 시작되기까지 기다리는 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoveryDelay = 2.0f; 

private:
	// 마지막으로 스태미나를 소모한 게임 시간 (World Time)
	float LastStaminaUseTime = 0.0f;
	
	
	//달리기 카메라 조절
	// [추가] 기본 FOV (걷기 상태) - 1인칭은 보통 90 정도를 기본으로 많이 씁니다.
	UPROPERTY(EditAnywhere, Category = "Camera")
	float DefaultFOV = 80.0f;

	// [추가] 달리기 FOV (최대 속도 상태) - 달릴 때 100~110까지 늘리면 속도감이 확 살아납니다.
	UPROPERTY(EditAnywhere, Category = "Camera")
	float RunFOV = 90.0f;

	// [추가] 1인칭 카메라 컴포넌트
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FirstPersonCamera;
	
protected:
	// ... 기존 변수들 ...

	// [추가] 조준 FOV (줌 인) - 숫자가 작을수록 확대됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimingFOV = 70.0f; 

	// [추가] 조준 상태 플래그
	bool bIsAiming = false;
	
	// [추가] 조준 시 이동 속도 (250)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimWalkSpeed = 250.0f;
	
	

};
