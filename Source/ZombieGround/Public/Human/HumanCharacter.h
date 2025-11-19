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
	
	//Input 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpAction(const FInputActionValue& Value);

};
