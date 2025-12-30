// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SearchZone.generated.h"

// 섹터 구조체 정의
USTRUCT(BlueprintType)
struct FSearchSector
{
	GENERATED_BODY()

	// 이 섹터에 포함된 방들의 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget = true))
	TArray<FVector> RoomPoints;
};

UCLASS()
class ZOMBIEGROUND_API ASearchZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASearchZone();

	virtual void BeginPlay() override;
	
public:
	// 건물의 진입점들 (에디터에서 드래그 가능)
	UPROPERTY(EditInstanceOnly, Category = "Search Logic", meta = (MakeEditWidget = true))
	TArray<FVector> EntryPoints;

	// // 건물 내부의 수색할 방 위치들 (에디터에서 드래그 가능)
	// UPROPERTY(EditInstanceOnly, Category = "Search Logic", meta = (MakeEditWidget = true))
	// TArray<FVector> RoomPoints;
	
	// [변경] 방들을 섹터 단위로 묶어서 관리
	// 에디터에서 Element를 추가하면 Sector 0, Sector 1... 이 됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSearchSector> Sectors;
	
	/**
	 * 관찰자(좀비) 위치에서 '실제 이동 경로(NavMesh)'가 가장 짧은 진입점까지의 거리를 반환합니다.
	 * @param ObserverLocation 좀비의 현재 위치
	 * @param WorldContext 월드 컨텍스트 (GetWorld용)
	 * @return 경로 길이 (cm). 경로가 없으면 FLT_MAX 반환.
	 */
	float GetNavPathDistanceToClosestEntryPoint(const FVector& ObserverLocation, UWorld* WorldContext) const;

	/**
	 * 관찰자(좀비) 위치에서 가장 가까운 진입점의 '월드 좌표'를 반환합니다.
	 * (직선 거리 기준, 이동 타겟 설정용)
	 */
	UFUNCTION(BlueprintPure, Category = "Search Logic")
	FVector GetClosestEntryPointLocation(const FVector& ObserverLocation) const;

	/**
	 * 등록된 방 중 무작위로 하나를 골라 월드 좌표를 반환합니다.
	 */
	// UFUNCTION(BlueprintPure, Category = "Search Logic")
	// FVector GetRandomRoomLocation() const;

	/**
	 * 로컬 위젯 좌표를 월드 좌표로 변환하는 내부 헬퍼
	 */
	FVector TransformLocalToWorld(const FVector& LocalPoint) const;
	
	// [추가] 에디터에서만 사용할 빌보드 컴포넌트 선언
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	class UBillboardComponent* SpriteComponent;
#endif
};
