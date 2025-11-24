// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Human/Interface/InteractInterface.h"
#include "BasePickup.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABasePickup : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// 전방 선언 (헤더 파일 로딩 속도 향상)
	// 에디터(블루프린트)에서 볼 수 있도록 VisibleAnywhere 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* gunMesh;
	
protected:
	// TSubclassOf를 사용하면 에디터에서 BaseWeapon을 상속받은 블루프린트만 선택할 수 있게 필터링됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseWeapon> WeaponToSpawn;
	
	
	
	// 3. 인터페이스 함수 오버라이드 선언
	// _Implementation을 붙여야 합니다 (BlueprintNativeEvent 규칙)
	// UFUNCTION()
	// virtual void OnInteract_Implementation(class AHumanCharacter* characterPickingUp,class ABasePickup* interactedPickup);
	//
public:
	UFUNCTION()
	TSubclassOf<class ABaseWeapon> GetWeaponToSpawn(){return WeaponToSpawn;};
};
