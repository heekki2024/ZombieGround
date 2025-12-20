// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equippable/BaseEquippable.h"
#include "Flashlight.generated.h"

UCLASS()
class ZOMBIEGROUND_API AFlashlight : public ABaseEquippable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlashlight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	// [추가] 물리 충돌 및 루트 역할을 할 캡슐 컴포넌트
	// UPROPERTY(VisibleAnywhere, Category = "Components")
	// class UCapsuleComponent* capsuleComponent;
	

	// 빛을 쏘는 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpotLightComponent* lightCone;
	
	// 현재 켜져있는지 상태 확인용
	bool bIsLightOn;
	
	// 켜고 끌 때 재생할 효과음 (에디터에서 할당)
	UPROPERTY(EditAnywhere, Category = "Effects")
	class USoundBase* toggleSound;
	
	UFUNCTION()
	void ToggleLight();
};
