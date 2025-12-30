// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

class UStateTreeAIComponent;

// 섹터 수색 상태 관리 구조체
USTRUCT(BlueprintType)
struct FSectorSearchState
{
	GENERATED_BODY()

	// 1. 이미 완료한 섹터 인덱스 목록
	TArray<int32> CompletedSectorIndices;

	// 2. 현재 수색 중인 섹터 인덱스 (-1이면 선택된 섹터 없음)
	int32 CurrentSectorIndex = -1;

	// 3. 현재 섹터에서의 목표 (방문할 방 개수)
	int32 TargetRoomsInCurrentSector = 0;

	// 4. 현재 섹터에서 이미 방문한 방 인덱스들
	TArray<int32> VisitedRoomIndicesInSector;

	void ResetAll()
	{
		CompletedSectorIndices.Empty();
		ResetCurrentSector();
	}

	void ResetCurrentSector()
	{
		CurrentSectorIndex = -1;
		TargetRoomsInCurrentSector = 0;
		VisitedRoomIndicesInSector.Empty();
	}
};
UCLASS()
class ZOMBIEGROUND_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	/** StateTree Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStateTreeAIComponent* StateTreeAI;

public:
	// Sets default values for this actor's properties
	AZombieAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

public:
	// 맵에 존재하는 모든 수색 구역 (최적화를 위해 미리 저장)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Search")
	TArray<TObjectPtr<class ASearchZone>> CachedSearchZones;
	
	// [추가] 모든 구역의 '진입점 월드 좌표'를 통합해서 저장 (빠른 이동용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Cache")
	TArray<FVector> CachedWorldEntryPoints;
	
	// [변경] 최근 방문한 구역들의 리스트 (History)
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, Category = "AI | State")
	TArray<TObjectPtr<class ASearchZone>> VisitedZoneHistory;

	// 몇 개까지 기억할지 설정 (기본값 2: 이전, 이전의 이전까지 제외)
	int32 MaxHistorySize = 3;

	// // 방금 수색을 마친 구역 (바로 다시 들어가는 것 방지용)
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Search")
	// TObjectPtr<class ASearchZone> LastVisitedZone;

public:
	// [변경] 이름 변경: RoomSearchStatus -> SectorSearchStatus
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "AI | State")
	FSectorSearchState SectorSearchStatus;

	// [기존 함수 수정] 건물이 바뀌면 모든 섹터 기록 초기화
	void StartNewBuildingSearch()
	{
		SectorSearchStatus.ResetAll();
	}
};
