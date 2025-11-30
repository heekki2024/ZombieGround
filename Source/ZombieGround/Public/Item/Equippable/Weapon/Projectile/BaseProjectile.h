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
	
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
};
