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
	int32 MAX_HP = 30;
	int32 currentHP = MAX_HP;

	UPROPERTY(EditAnywhere)
	float knockbackPower = 5;
	FVector knockbackPos;
	virtual void OnDamageProcess(FVector hitDirection);

	// 생성 시 재생될 전역 사운드 (요청하신 dieSound)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* DieSound;
	

};
