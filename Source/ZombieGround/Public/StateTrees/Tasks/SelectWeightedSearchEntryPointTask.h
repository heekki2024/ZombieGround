// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeTaskBase.h"
#include "Character/Zombie/ZombieCharacter.h"
#include "Controllers/ZombieAIController.h"
#include "UObject/Object.h"
#include "World/SearchZone.h"
#include "SelectWeightedSearchEntryPointTask.generated.h"

/**
 * 
 */
// ----------------------------------------------------------------------
// 1. Task 데이터 (Output)
// ----------------------------------------------------------------------
USTRUCT()
struct FSelectWeightedSearchZoneInstanceData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
    TObjectPtr<AZombieAIController> ZombieAIC = nullptr;

    UPROPERTY(EditAnywhere, Category = "Context", meta = (Context))
    TObjectPtr<AZombieCharacter> ZombieActor = nullptr;
    
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<ASearchZone> SelectedZone = nullptr;

	UPROPERTY(EditAnywhere, Category = "Output")
	FVector TargetLocation = FVector::ZeroVector;
};

// ----------------------------------------------------------------------
// 2. 내부 계산용 후보 구조체
// ----------------------------------------------------------------------
struct FEntryPointCandidate
{
	ASearchZone* OwnerZone;
	FVector Location;
	float Weight;
};

USTRUCT(meta = (DisplayName = "Select Weighted Entry Point (History Exclusion)", Category = "Zombie AI"))
struct ZOMBIEGROUND_API FSelectWeightedSearchEntryPointTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSelectWeightedSearchZoneInstanceData;

    FSelectWeightedSearchEntryPointTask()
    {
        bShouldCallTick = false;
    }

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
    {
        FInstanceDataType& Data = Context.GetInstanceData(*this);
        
        if (!Data.ZombieAIC || !Data.ZombieActor) return EStateTreeRunStatus::Failed;
        if (Data.ZombieAIC->CachedSearchZones.IsEmpty()) return EStateTreeRunStatus::Failed;
        
        const FVector PawnLocation = Data.ZombieActor->GetActorLocation();
        UWorld* World = Data.ZombieActor->GetWorld();
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

        if (!NavSys) return EStateTreeRunStatus::Failed;

        TArray<FEntryPointCandidate> Candidates;
        TArray<FEntryPointCandidate> FallbackCandidates; 
        float TotalWeight = 0.0f;
        
        const float MaxSearchDistSq = FMath::Square(100000.0f); 

        // 1. 후보 검색
        for (ASearchZone* Zone : Data.ZombieAIC->CachedSearchZones)
        {
            if (!IsValid(Zone)) continue;

            // [수정됨] 단일 변수 비교가 아니라, History 배열에 포함되어 있는지 확인
            // VisitedZoneHistory에 들어있는 모든 존(최근 2개 등)은 제외 대상
            bool bIsRecentlyVisited = Data.ZombieAIC->VisitedZoneHistory.Contains(Zone);

            FTransform ZoneTransform = Zone->GetActorTransform();

            for (const FVector& LocalPoint : Zone->EntryPoints)
            {
                FVector WorldEntryLocation = ZoneTransform.TransformPosition(LocalPoint);

                // 거리 1차 필터
                float DistSq = FVector::DistSquared(PawnLocation, WorldEntryLocation);
                if (DistSq > MaxSearchDistSq) continue;

                // NavMesh 경로 계산
                double PathLength = 0.0;
                ENavigationQueryResult::Type Result = NavSys->GetPathLength(PawnLocation, WorldEntryLocation, PathLength);

                if (Result != ENavigationQueryResult::Success) continue;

                // 가중치 계산
                float Weight = 100000.0f / ((float)PathLength + 100.0f);

                FEntryPointCandidate NewCandidate;
                NewCandidate.OwnerZone = Zone;
                NewCandidate.Location = WorldEntryLocation;
                NewCandidate.Weight = Weight;

                // [수정됨] 최근 방문 목록에 없으면 정식 후보, 있으면 Fallback 후보
                if (!bIsRecentlyVisited)
                {
                    Candidates.Add(NewCandidate);
                    TotalWeight += Weight;
                }
                else
                {
                    FallbackCandidates.Add(NewCandidate);
                }
            }
        }

        // 2. 만약 최근 방문하지 않은 새로운 건물 후보가 하나도 없다면?
        if (Candidates.Num() == 0)
        {
            // 어쩔 수 없이 최근에 방문했던 곳들 중에서 다시 선택 (Fallback 사용)
            if (FallbackCandidates.Num() > 0)
            {
                Candidates = FallbackCandidates;
                // TotalWeight 다시 계산
                for(const auto& C : Candidates) TotalWeight += C.Weight;
            }
            else
            {
                return EStateTreeRunStatus::Failed; 
            }
        }

        // 3. 룰렛 휠 선택
        float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
        const FEntryPointCandidate* SelectedCandidate = nullptr;

        for (const FEntryPointCandidate& Candidate : Candidates)
        {
            RandomValue -= Candidate.Weight;
            if (RandomValue <= 0.0f)
            {
                SelectedCandidate = &Candidate;
                break;
            }
        }

        if (!SelectedCandidate && Candidates.Num() > 0)
        {
            SelectedCandidate = &Candidates.Last();
        }

        // 4. 결과 반환 및 [History 업데이트]
        if (SelectedCandidate)
        {
            Data.SelectedZone = SelectedCandidate->OwnerZone;
            Data.TargetLocation = SelectedCandidate->Location;

            // [수정됨] 방문 기록 업데이트 로직
            // 1) 배열에 현재 선택된 존 추가
            Data.ZombieAIC->VisitedZoneHistory.Add(SelectedCandidate->OwnerZone);

            // 2) 기억할 개수(MaxHistorySize)를 초과하면 가장 오래된 것(인덱스 0)부터 삭제
            // 컨트롤러에 MaxHistorySize 변수가 없다면 상수로 2를 사용
            const int32 MaxHistory = Data.ZombieAIC->MaxHistorySize; // 혹은 Data.ZombieAIC->MaxHistorySize;
            
            while (Data.ZombieAIC->VisitedZoneHistory.Num() > MaxHistory)
            {
                Data.ZombieAIC->VisitedZoneHistory.RemoveAt(0);
            }


            // [수정/추가된 부분] 여기가 빠져있을 확률이 매우 높습니다!
            // 새 건물이 정해졌으니, 이전 건물의 '섹터 수색 기록'을 싹 지워야 합니다.
            // =================================================================
            Data.ZombieAIC->StartNewBuildingSearch();
            
            return EStateTreeRunStatus::Succeeded;
        }

        return EStateTreeRunStatus::Failed;
    }
};