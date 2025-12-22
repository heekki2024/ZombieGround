// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class ZOMBIEGROUND_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Root : Sphere Collision
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* CollisionComp;

	// Cascade Particle System
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UParticleSystemComponent* ParticleComp;

	// StaticMesh under Cascade Particle
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	// Radial Force under Cascade Particle
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class URadialForceComponent* RadialForceComp;

	// Projectile Movement
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* ImpactSound;

	// [추가] 사운드 감쇄 설정 (3D 사운드 범위 조절용)
	UPROPERTY(EditAnywhere, Category = "Effects")
	class USoundAttenuation* ImpactAttenuation;
	
public:
	// 이 총알이 가진 데미지 (스폰될 때 주입받음)
	float baseDmg = 1.0f;
	float headshotDmg = 1.0f;
	float knockbackStrength = 100.0f;
	float stun = 10;
	float stunTime = 0.3f;
	
	// [추가] 총알 생성 직후 호출해서 데미지를 세팅하는 함수
	void InitProjectile(float weaponBaseDmg, float weaponHeadshotDmg, float weaponKnockbackStrength, float weaponStun, float weaponStunTime);
	
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
};
