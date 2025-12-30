// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeTaskBase.h"
#include "AI/NavigationSystemBase.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "Controllers/ZombieAIController.h"
#include "UObject/Object.h"
#include "World/SearchZone.h"
#include "SelectNextWeightedRoomTask.generated.h"

class UNavigationSystemV1;
/**
 * 
 */
// ----------------------------------------------------------------------
// 1. Task 데이터 (Output)
// ----------------------------------------------------------------------
USTRUCT()
struct FSelectNextWeightedRoomInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
    TObjectPtr<AZombieAIController> ZombieAIC = nullptr;

    UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
    TObjectPtr<AZombieCharacter> ZombieActor = nullptr;

    UPROPERTY(EditAnywhere, Category = "Output")
    FVector TargetLocation = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "Select Next Weighted Room (Room First Logic)", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FSelectNextWeightedRoomTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSelectNextWeightedRoomInstanceData;

    FSelectNextWeightedRoomTask()
    {
       bShouldCallTick = false;
    }

    virtual const UStruct* GetInstanceDataType() const override
    {
       return FInstanceDataType::StaticStruct();
    }
    
    // 내부 후보 구조체
    struct FRoomCandidate 
    { 
        int32 SectorIndex;   // 어느 섹터 소속인지
        int32 RoomIndex;     // 그 섹터 안에서 몇 번째 방인지
        FVector Location;    // 방 위치
        float Weight;        // 가중치
    };

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
    {
       FInstanceDataType& Data = Context.GetInstanceData(*this);
        
       if (!Data.ZombieAIC || !Data.ZombieActor) return EStateTreeRunStatus::Failed;
       if (Data.ZombieAIC->VisitedZoneHistory.IsEmpty()) return EStateTreeRunStatus::Failed;

       ASearchZone* CurrentBuilding = Data.ZombieAIC->VisitedZoneHistory.Last();
       if (!IsValid(CurrentBuilding) || CurrentBuilding->Sectors.IsEmpty()) return EStateTreeRunStatus::Failed;

       FSectorSearchState& SearchState = Data.ZombieAIC->SectorSearchStatus;
       
       FVector PawnLocation = Data.ZombieActor->GetActorLocation();
       FTransform ZoneTransform = CurrentBuilding->GetActorTransform();
       UWorld* World = Data.ZombieActor->GetWorld();
       UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

       // ==============================================================================
       // [Step 1] 현재 상태 확인: "새로운 섹터를 찾아야 하는가?"
       // ==============================================================================
       bool bFindNewSector = false;

       // 1. 아예 시작 전인 경우 (-1)
       if (SearchState.CurrentSectorIndex == -1)
       {
           bFindNewSector = true;
       }
       // 2. 현재 섹터의 목표를 달성했거나 방이 동난 경우
       else
       {
           int32 RoomsInCurrentSector = CurrentBuilding->Sectors[SearchState.CurrentSectorIndex].RoomPoints.Num();
           int32 VisitedInCurrent = SearchState.VisitedRoomIndicesInSector.Num();

           if (VisitedInCurrent >= SearchState.TargetRoomsInCurrentSector || VisitedInCurrent >= RoomsInCurrentSector)
           {
               // 현재 섹터 완료 처리
               SearchState.CompletedSectorIndices.AddUnique(SearchState.CurrentSectorIndex);
               SearchState.ResetCurrentSector();
               bFindNewSector = true;
           }
       }

       // ==============================================================================
       // [Step 2] 후보군 수집 (Candidates)
       // ==============================================================================
       TArray<FRoomCandidate> Candidates;
       float TotalWeight = 0.0f;

       if (bFindNewSector)
       {
           // --- [모드 A: 글로벌 검색] ---
           // 아직 완료하지 않은 '모든 섹터'의 '모든 방'을 후보로 등록합니다.
           // 여기서 가중치로 뽑힌 방이 속한 섹터가 다음 타겟이 됩니다.

           for (int32 SecIdx = 0; SecIdx < CurrentBuilding->Sectors.Num(); SecIdx++)
           {
               // 이미 완료한 섹터는 제외
               if (SearchState.CompletedSectorIndices.Contains(SecIdx)) continue;

               const FSearchSector& Sector = CurrentBuilding->Sectors[SecIdx];
               for (int32 RoomIdx = 0; RoomIdx < Sector.RoomPoints.Num(); RoomIdx++)
               {
                   // (참고: 새 섹터를 뚫을 때는 모든 방이 후보가 됩니다)
                   FVector RoomWorldLoc = ZoneTransform.TransformPosition(Sector.RoomPoints[RoomIdx]);
                   float Weight = CalculateWeight(PawnLocation, RoomWorldLoc, NavSys);
                   
                   Candidates.Add({ SecIdx, RoomIdx, RoomWorldLoc, Weight });
                   TotalWeight += Weight;
               }
           }
       }
       else
       {
           // --- [모드 B: 섹터 내 검색] ---
           // 현재 잠겨있는(Current) 섹터 안에서 안 가본 방만 후보로 등록합니다.

           int32 SecIdx = SearchState.CurrentSectorIndex;
           const FSearchSector& Sector = CurrentBuilding->Sectors[SecIdx];

           for (int32 RoomIdx = 0; RoomIdx < Sector.RoomPoints.Num(); RoomIdx++)
           {
               // 이미 방문한 방 제외
               if (SearchState.VisitedRoomIndicesInSector.Contains(RoomIdx)) continue;

               FVector RoomWorldLoc = ZoneTransform.TransformPosition(Sector.RoomPoints[RoomIdx]);
               float Weight = CalculateWeight(PawnLocation, RoomWorldLoc, NavSys);

               Candidates.Add({ SecIdx, RoomIdx, RoomWorldLoc, Weight });
               TotalWeight += Weight;
           }
       }

       // 후보가 없다면?
       if (Candidates.Num() == 0)
       {
           // 모드 A에서 후보가 없다 = 모든 섹터 완료 = 건물 수색 끝 -> Failed 반환 (다음 건물로)
           if (bFindNewSector) return EStateTreeRunStatus::Failed;
           
           // 모드 B에서 후보가 없다 = 섹터 내 남은 방 없음 -> 강제로 섹터 종료하고 재시도(Succeeded 반환하여 루프)
           SearchState.CompletedSectorIndices.AddUnique(SearchState.CurrentSectorIndex);
           SearchState.ResetCurrentSector();
           // 이번 틱은 쉬고 다음 틱에 모드 A로 진입하게 함 (혹은 재귀호출 가능하나 루프 권장)
           return EStateTreeRunStatus::Succeeded; 
       }

       // ==============================================================================
       // [Step 3] 룰렛 휠 선택
       // ==============================================================================
       float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
       const FRoomCandidate* Selected = nullptr;

       for (const auto& Cand : Candidates)
       {
           RandomValue -= Cand.Weight;
           if (RandomValue <= 0.0f) { Selected = &Cand; break; }
       }
       if (!Selected) Selected = &Candidates.Last();

       // ==============================================================================
       // [Step 4] 상태 업데이트 및 결과 반환
       // ==============================================================================
       
       // 만약 새로운 섹터를 뚫은 것이라면? (모드 A였던 경우)
       if (bFindNewSector)
       {
           SearchState.CurrentSectorIndex = Selected->SectorIndex;
           SearchState.VisitedRoomIndicesInSector.Empty(); // 초기화

           // [요청하신 기능] 섹터의 방 개수를 구한 후 랜덤한 개수를 골라 설정
           int32 TotalRoomsInThisSector = CurrentBuilding->Sectors[Selected->SectorIndex].RoomPoints.Num();
           if (TotalRoomsInThisSector > 0)
           {
               // 최소 1개 ~ 최대 전체 개수
               SearchState.TargetRoomsInCurrentSector = FMath::RandRange(1, TotalRoomsInThisSector);
           }
       }

       // 공통 처리: 선택된 방을 방문 처리
       SearchState.VisitedRoomIndicesInSector.Add(Selected->RoomIndex);
       Data.TargetLocation = Selected->Location;

       UE_LOG(LogTemp, Warning, TEXT("Zombie [%s] Target: Building [%s], Sector [%d], Room [%d]"), 
           *GetNameSafe(Data.ZombieActor), 
           *GetNameSafe(CurrentBuilding), 
           Selected->SectorIndex, 
           Selected->RoomIndex);

       return EStateTreeRunStatus::Succeeded;
    }

private:
    // 가중치 계산 함수 분리 (코드 중복 방지)
    float CalculateWeight(const FVector& Start, const FVector& End, UNavigationSystemV1* NavSys) const
    {
        double PathLength = 0.0;
        if (NavSys)
        {
            // 비용 절감을 위해 직선 거리가 너무 멀면(예: 30m) 정밀 계산 스킵 가능
            // 여기서는 정석대로 모두 계산
            ENavigationQueryResult::Type Result = NavSys->GetPathLength(Start, End, PathLength);
            if (Result != ENavigationQueryResult::Success) 
            {
                 // 경로 없음: 직선 거리로 대체하거나 0점 처리 (여기선 직선거리 페널티)
                 PathLength = FVector::Dist(Start, End) * 2.0f; 
            }
        }
        else
        {
            PathLength = FVector::Dist(Start, End);
        }

        // 거리가 가까울수록 가중치 높음
        return 100000.0f / ((float)PathLength + 100.0f);
    }
};