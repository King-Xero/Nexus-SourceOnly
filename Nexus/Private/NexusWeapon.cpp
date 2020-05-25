// Toyan Green © 2020


#include "NexusWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "Nexus/Utils/ConsoleVariables.h"
#endif

// Sets default values
ANexusWeapon::ANexusWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialise the weapon component
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ANexusWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

/**
 * \brief Shoot the weapon
 */
void ANexusWeapon::Fire()
{
	AActor* WeaponOwner = GetOwner();
	
	if (WeaponOwner)
	{
		FVector EyeLocation; // Start location for line trace.
		FRotator EyeRotation;
		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		
		FVector TraceEnd = EyeLocation + (ShotDirection * WeaponRange); // End location for eye trace.

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(WeaponOwner); // Ignore collisions with the weapon owner.
		CollisionQueryParams.AddIgnoredActor(this); // Ignore collisions with the weapon itself.
		CollisionQueryParams.bTraceComplex = true; // Complex collision is more expensive, but we get exact location of where was hit.

		// Bullet tracer target parameter.
		FVector BulletTracerTarget = TraceEnd;
		
		FHitResult WeaponHitResult;		
		// Trace the world between the start and end locations. Returns true if blocking hit.
		if(GetWorld()->LineTraceSingleByChannel(WeaponHitResult, EyeLocation, TraceEnd, ECC_Visibility))
		{
			AActor* HitActor = WeaponHitResult.GetActor();

			// Apply damage to the hit actor.
			UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, ShotDirection, WeaponHitResult, WeaponOwner->GetInstigatorController(), this, DamageType);

			// Spawn particle effect for impact.
			if (ImpactVFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, WeaponHitResult.ImpactPoint, WeaponHitResult.Normal.Rotation());
			}

			// If the shot hit something, the bullet tracer target should be updated.
			BulletTracerTarget = WeaponHitResult.ImpactPoint;
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		bool bDrawDebug = CVarDebugWeaponDrawing.GetValueOnGameThread();
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
#endif

		// Spawn particle effect for muzzle flash.
		if (MuzzleVFX)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleVFX, MeshComponent, MuzzleSocketName);
		}

		// Spawn bullet tracer particle effect.
		if (TracerVFX)
		{
			FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

			UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerVFX, MuzzleLocation);

			if (TracerComponent)
			{
				TracerComponent->SetVectorParameter(TracerTargetParameterName, BulletTracerTarget);
			}
		}
		
	}
	
}

// Called every frame
void ANexusWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}