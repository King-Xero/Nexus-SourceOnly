// Toyan Green © 2020


#include "NexusWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Nexus/Utils/NexusTypeDefinitions.h"
#include "Net/UnrealNetwork.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "DrawDebugHelpers.h"
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

	// Needs to be set to that the weapon spawns on all clients.
	SetReplicates(true);

	// Set how frequently the actor is updated on the network. (Higher frequency = less delay).
	MinNetUpdateFrequency = 35.0f;
	NetUpdateFrequency = 70.0f;
}

// Called every frame
void ANexusWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANexusWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate hit info on all clients except the owner, so the other clients can play the replicated weapon effects.
	DOREPLIFETIME_CONDITION(ANexusWeapon, HitScanInfo, COND_SkipOwner);
}

void ANexusWeapon::StartFiring()
{
	// Need to calculate a first delay to that the firing rate cannot be bypassed with rapid input.
	// The first delay has to be at least 0, as passing a negative value has reserved functionality.
	const float FirstDelay = FMath::Max(LastFireTime + WeaponFireDelayTime - GetWorld()->TimeSeconds, 0.0f);
	
	// Start looping timer to start automatic fire
	GetWorldTimerManager().SetTimer(TimerHandle_WeaponFireDelay, this, &ANexusWeapon::Fire,  WeaponFireDelayTime, true, FirstDelay);
}

void ANexusWeapon::StopFiring()
{
	// Stop looping timer to stop automatic fire
	GetWorldTimerManager().ClearTimer(TimerHandle_WeaponFireDelay);
}

// Called when the game starts or when spawned
void ANexusWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Calculate the time between weapon shot. Rate of fire is round per minute.
	WeaponFireDelayTime = 60.0f / WeaponRateOfFire;
}

/**
 * \brief Shoot the weapon
 */
void ANexusWeapon::Fire()
{
	// Fire should only be called via the server authority.
	if (ROLE_Authority > GetLocalRole())
	{
		ServerFire();
	}
	
	AActor* WeaponOwner = GetOwner();
	
	if (WeaponOwner)
	{
		FVector EyeLocation; // Start location for line trace.
		FRotator EyeRotation;
		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Add "bullet spread" to shot direction.
		const float HalfAngleRad = FMath::DegreesToRadians(BulletSpreadAngle);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfAngleRad);
		
		FVector TraceEnd = EyeLocation + (ShotDirection * WeaponRange); // End location for eye trace.

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(WeaponOwner); // Ignore collisions with the weapon owner.
		CollisionQueryParams.AddIgnoredActor(this); // Ignore collisions with the weapon itself.
		CollisionQueryParams.bTraceComplex = true; // Complex collision is more expensive, but we get exact location of where was hit.
		CollisionQueryParams.bReturnPhysicalMaterial = true; // Ensure that the surface material is returned to check what body part was hit.

		// Bullet tracer target parameter.
		FVector BulletTracerTarget = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;
		
		FHitResult WeaponHitResult;		
		// Trace the world between the start and end locations. Returns true if blocking hit.
		if(GetWorld()->LineTraceSingleByChannel(WeaponHitResult, EyeLocation, TraceEnd, COLLISION_TRACE_WEAPON, CollisionQueryParams))
		{
			AActor* HitActor = WeaponHitResult.GetActor();

			// Get the surface type that was hit.
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(WeaponHitResult.PhysMaterial.Get());

			// Calculate the amount of damage to inflict.
			float DamageToInflict = WeaponDamage * GetDamageMultiplier(SurfaceType);
			
			// Apply damage to the hit actor.
			UGameplayStatics::ApplyPointDamage(HitActor, DamageToInflict, ShotDirection, WeaponHitResult, WeaponOwner->GetInstigatorController(), this, DamageType);

			// Play weapon impact effects locally.
			PlayWeaponImpactEffects(SurfaceType, WeaponHitResult.ImpactPoint);
			
			// If the shot hit something, the bullet tracer target should be updated.
			BulletTracerTarget = WeaponHitResult.ImpactPoint;
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const bool bDrawDebug = CVarDebugWeaponDrawing.GetValueOnGameThread();
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
#endif

		// Play weapon effects locally.
		PlayWeaponFiredEffects(BulletTracerTarget);

		// The server authority should replicate the hit scan information, so clients can replicate the weapon effects.
		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanInfo.TraceTargetLocation = BulletTracerTarget;
			HitScanInfo.HitSurfaceType = SurfaceType;
		}

		// This needs to be set to prevent the firing rate getting bypassed with rapid firing input.
		LastFireTime = GetWorld()->GetTimeSeconds();
	}	
}

void ANexusWeapon::ServerFire_Implementation()
{
	// The shoot code will execute via the server authority.
	Fire();
}

bool ANexusWeapon::ServerFire_Validate()
{
	return true;
}

void ANexusWeapon::OnRep_HitScanInfo() const
{
	// This method is called when HitScanInfo is replicated.

	// When the information is replicated to the other clients, they should use it to play the weapon effects.
	PlayWeaponFiredEffects(HitScanInfo.TraceTargetLocation);

	PlayWeaponImpactEffects(HitScanInfo.HitSurfaceType, HitScanInfo.TraceTargetLocation);
}


void ANexusWeapon::PlayWeaponImpactEffects(EPhysicalSurface SurfaceType, FVector Target) const
{
	// Spawn particle effect for weapon impact.
	PlayImpactEffect(SurfaceType, Target);
}

void ANexusWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Target) const
{
	// Set the impact vfx to be played depending on the surface type that is hit.
	UParticleSystem* SurfaceImpactVFX = nullptr;

	switch (SurfaceType)
	{
		case SURFACE_CHARACTER_HEAD:
		case SURFACE_CHARACTER_BODY:
		case SURFACE_CHARACTER_LIMBS:
			SurfaceImpactVFX = CharacterImpactVFX;
			break;
		default:
			SurfaceImpactVFX = DefaultImpactVFX;
			break;
	}

	// Spawn particle effect for impact.
	if (SurfaceImpactVFX)
	{
		const FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		// Calculate the direction the shot was fired from so that we use the correct rotation.
		FVector ShotDirection = Target - MuzzleLocation;
		ShotDirection.Normalize();
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SurfaceImpactVFX, Target, ShotDirection.Rotation());
	}
}

void ANexusWeapon::PlayWeaponFiredEffects(FVector BulletTracerTarget) const
{
	// Spawn particle effect for muzzle flash.
	PlayMuzzleEffect();

	// Spawn bullet tracer particle effect.
	PlayBulletTracerEffect(BulletTracerTarget);

	// Shake the camera.
	PlayCameraShake();
}

void ANexusWeapon::PlayMuzzleEffect() const
{
	// Spawn particle effect for muzzle flash.
	if (MuzzleVFX)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleVFX, MeshComponent, MuzzleSocketName);
	}
}

void ANexusWeapon::PlayBulletTracerEffect(FVector BulletTracerTarget) const
{
	if (TracerVFX)
	{
		const FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerVFX, MuzzleLocation);

		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(TracerTargetParameterName, BulletTracerTarget);
		}
	}
}

void ANexusWeapon::PlayCameraShake() const
{
	if (WeaponCameraShake)
	{
		// Get the owners controller and play camera shake.
		APawn* WeaponOwner = Cast<APawn>(GetOwner());
		if (WeaponOwner)
		{
			APlayerController* WeaponOwnerController = Cast<APlayerController>(WeaponOwner->GetController());
			if (WeaponOwnerController)
			{
				WeaponOwnerController->ClientPlayCameraShake(WeaponCameraShake);
			}
		}
	}	
}

float ANexusWeapon::GetDamageMultiplier(EPhysicalSurface SurfaceType) const
{
	// Return the damage multiplier for the surface type that was hit.
	switch (SurfaceType)
	{
		case SURFACE_CHARACTER_HEAD:
			return HeadShotDamageMultiplier;
		case SURFACE_CHARACTER_LIMBS:
			return LimbDamageMultiplier;
		default:
			// Default damage multiplier is 1.
			return 1.0f;
	}
}
