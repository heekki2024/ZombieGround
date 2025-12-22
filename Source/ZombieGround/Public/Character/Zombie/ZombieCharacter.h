// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "ZombieCharacter.generated.h"

UCLASS()
class ZOMBIEGROUND_API AZombieCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	UCapsuleComponent* InteractionCapsule;
	
	UPROPERTY()
	APlayerController* PC;
	
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_ZombiePlayer;
	
		// --- 마우스 옵션 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Mouse")
	float MouseSensitivity = 0.5f;

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
	
	
	//이전에 하이라이트된 액터 저장용 변수
	UPROPERTY()
	class AActor* outLinedInteractable = nullptr;
	
	
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
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimInstance* AnimInstance;
	// 에디터에서 할당할 공격 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* basicAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* DamageMontage;

	virtual void PlayBasicAttackMontage();
	virtual void PlayDamageMontage();
	
public:
	// 공격 판정 함수 (AnimNotify에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackHitCheck();

	// 공격 거리 및 범위 설정
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRadius = 50.0f;
	
	void SetInteractableOutline(AActor* interactable, bool bEnable);
	AActor* GetCenterScreenInteractable();
	
	
	UPROPERTY(EditAnywhere)
	float MAX_HP = 500.0;
	float currentHP = MAX_HP;

	UPROPERTY(EditAnywhere)
	float knockbackPower = 5;
	FVector knockbackPos;
	virtual void OnDamageProcess(float damage,const FHitResult& hitResult,FVector bulletDirection, float knockbackStrength, float stun, float stunTime);
	void OnDie();

	// 생성 시 재생될 전역 사운드 (요청하신 dieSound)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* DieSound;
	
	// 생성된 HUD 인스턴스
	UPROPERTY()
	class UZombieHud* zombieHUD;
	
	// [추가] 넉백 상태 관리 변수
	bool bIsKnockbackActive = false; // 지금 밀려나는 중인가?
	FVector KnockbackTargetPos;      // 어디까지 밀려날 것인가?
protected:
	// 넉백 처리를 담당하는 내부 함수
	// void UpdateKnockback(float DeltaTime);
	
private:
	// [추가] 스턴 상태가 끝나면 속도를 복구하는 타이머 핸들
	FTimerHandle StunTimerHandle;

	// [추가] 원래 이동 속도를 저장할 변수
	float DefaultWalkSpeed = 600.0f; 

	// [추가] 타이머가 끝나면 호출될 함수
	void RestoreWalkSpeed();
};
