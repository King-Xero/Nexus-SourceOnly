// Toyan Green © 2020


#include "GrenadeLauncherProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "Nexus/Utils/ConsoleVariables.h"
#endif

// Sets default values
AGrenadeLauncherProjectile::AGrenadeLauncherProjectile()
{
	// Create and configure a collision sphere.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	// Wire up an event for when this component hits something.
	CollisionComponent->OnComponentHit.AddDynamic(this, &AGrenadeLauncherProjectile::OnHit);

	// Ensure players can't walk on it
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComponent;

	// Use a ProjectileMovementComponent to govern this projectile's movement.
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default.
	InitialLifeSpan = 3.0f;
}

void AGrenadeLauncherProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (nullptr != OtherActor && this != OtherActor)
	{
		// Only add impulse if we hit a physics object.
		if (nullptr != OtherComp && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsImpulseStrength, GetActorLocation());
		}
		else if (OtherActor->IsA<APawn>())
		{
			// If the projectile hits a character, it should explode on impact.
			Explode();
		}
	}
}

void AGrenadeLauncherProjectile::LifeSpanExpired()
{
	Explode();
}

void AGrenadeLauncherProjectile::SetDamageAmount(float Damage)
{
	DamageAmount = Damage;
}

void AGrenadeLauncherProjectile::SetDamageRadius(float Radius)
{
	DamageRadius = Radius;
}

void AGrenadeLauncherProjectile::SetDamageType(TSubclassOf<UDamageType> Type)
{
	DamageType = Type;
}

void AGrenadeLauncherProjectile::Explode()
{
	AActor* ProjectileOwner = GetInstigator();

	// Apply damage to actors around the projectile
	UGameplayStatics::ApplyRadialDamage(GetWorld(), DamageAmount, CollisionComponent->GetComponentLocation(), DamageRadius, DamageType,
		{}, this, ProjectileOwner->GetInstigatorController());

	// Spawn particle effect for explosion.
	PlayExplosionEffect();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const bool bDrawDebug = CVarDebugWeaponDrawing.GetValueOnGameThread();
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), CollisionComponent->GetComponentLocation(), DamageRadius, 12.0f, FColor::Yellow, false, 1.0f, 0, 1.0f);
	}
#endif
	
	Destroy();
}

void AGrenadeLauncherProjectile::PlayExplosionEffect() const
{
	if (ExplosionVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, CollisionComponent->GetComponentLocation(), CollisionComponent->GetComponentRotation());
	}
}
