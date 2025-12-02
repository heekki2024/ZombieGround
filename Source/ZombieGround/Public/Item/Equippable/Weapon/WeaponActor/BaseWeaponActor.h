// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equippable/BaseEquippable.h"
#include "BaseWeaponActor.generated.h"

// 1. 블루프린트에서 변수로 사용하려면 BlueprintType 필수


UCLASS()
class ZOMBIEGROUND_API ABaseWeaponActor : public ABaseEquippable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	
	
	// =================================================================
	// 1. Components
	// =================================================================
    
	// 총기 본체 (애니메이션 재생을 위해 스켈레탈 메쉬 사용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* weaponMesh;

	// =================================================================
	// 2. State (데이터 연결)
	// =================================================================

	// 이 무기의 현재 상태 (탄약, 부착물 목록 등) - 인벤토리에서 넘어옴
	UPROPERTY(BlueprintReadOnly, Category = "State")
	class UWeaponInstance* weaponInstance;



	// 현재 생성된 부착물 액터들을 관리하는 배열 (파괴 시 같이 없애기 위해)
	UPROPERTY()
	TArray<AActor*> SpawnedAttachments;
	

// 	//총 관련
protected:
	
	UPROPERTY()
	FTimerHandle FireTimerHandle;

	double NextFireTime = 0.0;  // 다음 발사 가능 시간
	
	// 부착물 시각화 업데이트
	void UpdateAttachments();

	//칼 타입만
	
public:
	
	bool bIsRightClicking = false;

	
	// =================================================================
	// 3. Interface (외부에서 호출)
	// =================================================================

	// [핵심] 인벤토리 데이터(Instance)를 기반으로 무기 초기화 (메쉬 변경, 부착물 장착)
	virtual void LoadWeaponInstance(class UWeaponInstance* updatedInstance);
	
	UPROPERTY(EditAnywhere)
	class TSubclassOf<class ABasePickup> pickupClass;
	
	// UFUNCTION(BlueprintCallable)
	// bool GetbIsRightClicking() const {return bIsRightClicking;}
	//
	
	
	virtual void OnLeftClickPressed() override;
	virtual void OnLeftClickReleased() override;
	virtual void OnRightClickPressed() override;
	virtual void OnRightClickReleased() override;
	
	
	UFUNCTION()
	void Fire();
	
	UFUNCTION()
	void Reload();
};
