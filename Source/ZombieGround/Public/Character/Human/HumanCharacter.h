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
	// class UWeaponInstance* currentWeaponInstance;

	
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
};
