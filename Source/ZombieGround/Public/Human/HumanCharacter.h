// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "HumanCharacter.generated.h"


UCLASS()
class ZOMBIEGROUND_API AHumanCharacter : public ACharacter
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
	
	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	class UInventoryComponent* InventoryComponent;
	
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
	
	//Input 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpAction(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void OnRightClickPressed(const FInputActionValue& Value);
	void OnRightClickReleased(const FInputActionValue& Value);
	void OnLeftClickPressed(const FInputActionValue& Value);
	void OnLeftClickReleased(const FInputActionValue& Value);
	void OnNum1KeyPressed(const FInputActionValue& Value);
	void OnNum2KeyPressed(const FInputActionValue& Value);

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseWeapon> WeaponToSpawnClass;
	
	
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
	class ABasePickup* HighlightedPickup = nullptr;
	
	UFUNCTION()
	void SetActorOutline(ABasePickup* pickup, bool bEnable);
	
	
public:
	//Interaction 관련
	UPROPERTY()
	class ABaseWeapon* BaseWeapon;
	
	UFUNCTION()
	ABaseWeapon* SpawnWeapon(TSubclassOf<ABaseWeapon> weaponToSpawn);
	
	//E키로 습득한 무기 클래스를 1인칭에 인스턴스화
	UFUNCTION()
	void SwapWeapon(TSubclassOf<ABaseWeapon> weaponToSpawn);
	
	// UFUNCTION()
	// void DropCurrentWeapon();
	//
	
protected:
	// 현재 장착하고 있는 무기 인스턴스를 저장할 변수
	UPROPERTY(EditAnywhere)
	class ABaseWeapon* currentWeapon;
	
	// EWeaponName currentWeaponNameEnum;
	//
	
public:
	
	
	UFUNCTION(BlueprintCallable)
	ABaseWeapon* GetCurrentWeapon() const { return currentWeapon;};
	
	
	
	// UFUNCTION()
	// EWeaponName GetCurrentWeaponName() const { return currentWeaponNameEnum;};
};
