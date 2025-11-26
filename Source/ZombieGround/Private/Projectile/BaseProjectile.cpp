// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/BaseProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 1) Sphere Collision (ROOT)
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->InitSphereRadius(5.f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);

	// 2) Particle Component (Cascade)
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->SetupAttachment(CollisionComp);  

	// 3) StaticMesh Component (Child of Particle)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(ParticleComp);

	// 4) RadialForce (Child of Particle)
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(ParticleComp);
	RadialForceComp->Radius = 75.f;
	RadialForceComp->ImpulseStrength = 8000.f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;   // 필요할 때만 발동시키도록

	// 5) Projectile Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 4000.f;
	ProjectileMovement->MaxSpeed = 4000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // 총알은 대부분 중력 없음
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (RadialForceComp)
	{
		RadialForceComp->FireImpulse();
	}
	if (ImpactParticle)  // UPROPERTY로 선언했다고 가정
	{
		// Normal → Rotation
		FRotator Rotation = FRotationMatrix::MakeFromX(Hit.Normal).Rotator();

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticle,
			Hit.ImpactPoint,
			Rotation
		);
	}
	
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			Hit.ImpactPoint
		);
	}
	Destroy();

}

