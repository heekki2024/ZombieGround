// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickup.generated.h"


UCLASS()
class ZOMBIEGROUND_API ABasePickup : public AActor
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
	// 시각적 표현 (무기 메쉬 or 상자 메쉬)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* meshComp;
    
	// // 플레이어 감지용 (혹은 인터페이스 사용 시 제거 가능)
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// class USphereComponent* InteractionZone;

public:
	// 이 픽업이 어떤 아이템인지 정의 (에디터에서 할당)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	class UBaseItemDataAsset* itemDataAsset;
    
	// (선택) 이미 쓰던 총을 버렸을 때, 남은 탄약을 기억하기 위한 인스턴스 보관
	UPROPERTY()
	class UBaseItemInstance* itemInstance;

	// // 상호작용 시 호출 (인벤토리에 Instance 생성 후 자폭)
	// virtual void OnInteract(class ACharacter* Interactor);
};
