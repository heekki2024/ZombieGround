// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "UQ3Character.generated.h"


UCLASS()
class ZOMBIEGROUND_API AUQ3Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUQ3Character(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	// --- [MouseLook.cs] 대응 설정 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, Category = "Mouse Look")
	float XSensitivity = 2.0f; // 마우스 좌우 감도

	UPROPERTY(EditAnywhere, Category = "Mouse Look")
	float YSensitivity = 2.0f; // 마우스 상하 감도

	UPROPERTY(EditAnywhere, Category = "Mouse Look")
	bool bUseSmoothing = false; // 부드러운 회전 사용 여부

	UPROPERTY(EditAnywhere, Category = "Mouse Look")
	float SmoothTime = 10.0f; // 스무딩 속도

	// --- [Q3PlayerDebug.cs] 대응 변수 ---
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugStats = true;
	
public:
	UPROPERTY(EditAnywhere, Category = "input")
	class UInputAction* IA_Turn;
	
	UPROPERTY(EditAnywhere, Category = "input")
	class UInputAction* IA_Lookup;
		
	UPROPERTY(EditAnywhere, Category = "input")
	class UInputAction* IA_MoveForward;
	
	UPROPERTY(EditAnywhere, Category = "input")
	class UInputAction* IA_MoveRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* IA_Jump;
private:
	// 스무딩을 위한 목표 회전값
	float TargetYaw;
	float TargetPitch;
	
	// 디버그용 변수
	float TopSpeed = 0.0f;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputMappingContext* IMC_UQ3Player;
	

	// 마우스 입력 처리 함수
	void LookUp(const FInputActionValue& Value);
    void Turn(const FInputActionValue& Value);
    void MoveForward(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
	
	// 점프 입력 처리 함수
	void Input_Jump(const FInputActionValue& Value);
	void Input_StopJump(const FInputActionValue& Value);
	
	virtual void CheckJumpInput(float DeltaTime) override;
};
