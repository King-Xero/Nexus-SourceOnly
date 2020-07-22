// Toyan Green © 2020


#include "NexusWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Nexus/Utils/NexusTypeDefinitions.h"
#include "Net/UnrealNetwork.h"
#include "Nexus/Utils/Logging/NexusLogging.h"
#include "NexusCharacter.h"
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

	// Replicate ammo variables for weapon owner.
	DOREPLIFETIME_CONDITION(ANexusWeapon, CurrentAmmoInClip, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANexusWeapon, CurrentTotalAmmo, COND_OwnerOnly);

	// Replicate the weapon owner. Required for animation replication.
	DOREPLIFETIME(ANexusWeapon, OwningCharacter);

	DOREPLIFETIME(ANexusWeapon, bReloading);
}

void ANexusWeapon::StartFiring()
{
	// Need to calculate a first delay to that the firing rate cannot be bypassed with rapid input.
	// The first delay has to be at least 0, as passing a negative value has reserved functionality.
	const float FirstDelay = FMath::Max(LastFireTime + WeaponFireDelayTime - GetWorld()->TimeSeconds, 0.0f);
	
	// Start looping timer to start automatic fire.
	GetWorldTimerManager().SetTimer(TimerHandle_WeaponFireDelay, this, &ANexusWeapon::Fire,  WeaponFireDelayTime, true, FirstDelay);
}

void ANexusWeapon::StopFiring()
{
	// Stop looping timer to stop automatic fire.
	GetWorldTimerManager().ClearTimer(TimerHandle_WeaponFireDelay);
}

void ANexusWeapon::StartReloading()
{
	if (CanReloadWeapon())
	{
		// Play reload animation.
		if (OwningCharacter && ReloadAnimMontage)
		{
			const float ReloadMontagePlaybackRate = ReloadAnimMontage->GetPlayLength() / WeaponReloadDelayTime;
			
			if (ROLE_Authority > GetLocalRole())
			{
				// Play the animation via the server authority.
				ServerPlayAnimationMontage(ReloadAnimMontage, ReloadMontagePlaybackRate);
			}
			else
			{
				// Play the animation.
				MulticastPlayAnimationMontage(ReloadAnimMontage, ReloadMontagePlaybackRate);
			}
		}
		
		// Start timer to reload the weapon.
		GetWorldTimerManager().SetTimer(TimerHandle_WeaponReloadDelay, this, &ANexusWeapon::Reload, WeaponReloadDelayTime);

		bReloading = true;
		FNexusLogging::Log(ELogLevel::INFO, "Weapon has started reloading");
	}	
}

void ANexusWeapon::StopReloading()
{
	// Stop timer to stop reloading the weapon.
	GetWorldTimerManager().ClearTimer(TimerHandle_WeaponReloadDelay);

	bReloading = false;
	FNexusLogging::Log(ELogLevel::INFO, "Weapon reloading was stopped.");	
}

void ANexusWeapon::SetOwnerAndAttachToCharacter(ANexusCharacter* NewOwningCharacter)
{
	if (OwningCharacter != NewOwningCharacter)
	{
		OwningCharacter = NewOwningCharacter;
		// Set owner for use in fire function.
		SetOwner(OwningCharacter);
		// Attach the weapon to the character's hand.
		AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}	
}

// Called when the game starts or when spawned
void ANexusWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Calculate the time between weapon shot. Rate of fire is round per minute.
	WeaponFireDelayTime = 60.0f / WeaponRateOfFire;
	// Initial ammo in clip should be somewhere between the max ammo the clip can hold, and the amount of ammo this weapon spawns with.
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartingAmmo);
	// The total available ammo should be the smallest amount between the spawn amount, and the maximum that can be carried.
	CurrentTotalAmmo = FMath::Min(StartingAmmo, MaxAmmo);
}

bool ANexusWeapon::CanFireWeapon() const
{
	const bool bAmmoInClip = 0 < CurrentAmmoInClip;
	
	return bAmmoInClip && !bReloading;
}

/**
 * \brief Shoot the weapon
 */
void ANexusWeapon::Fire()
{
	AActor* WeaponOwner = GetOwner();
	
	if (WeaponOwner && CanFireWeapon())
	{
		// Fire should only be called via the server authority.
		if (ROLE_Authority > GetLocalRole())
		{
			ServerFire();
		}
		
		// Use ammo.
		DepleteAmmo();
		
		FVector EyeLocation; // Start location for line trace.
		FRotator EyeRotation;
		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Add "bullet spread" to shot direction. Shots are/can be more accurate while aiming down sights.
		const float HalfAngleRad = OwningCharacter && OwningCharacter->IsAimingDownSights() ? FMath::DegreesToRadians(ADSBulletSpreadAngle) : FMath::DegreesToRadians(BulletSpreadAngle);
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

bool ANexusWeapon::CanReloadWeapon()
{
	// Check if ammo has been depleted from the clip.
	const bool bRoomInClip = CurrentAmmoInClip < MaxAmmoPerClip;
	// Check if there is ammo spare that is not in the clip.
	const bool bAmmoInReserve = CurrentTotalAmmo - CurrentAmmoInClip > 0;

	return bRoomInClip && bAmmoInReserve;
}

void ANexusWeapon::Reload()
{
	// Reload should only be called via the server authority.
	if (ROLE_Authority > GetLocalRole())
	{
		ServerReload();
	}
	
	// Calculate how much room is in the clip.
	const int32 BulletsDepletedFromClip = MaxAmmoPerClip - CurrentAmmoInClip;

	// Calculate how much ammo is spare that is not in the clip.
	const int32 BulletsInReserve = CurrentTotalAmmo - CurrentAmmoInClip;

	// The amount of bullets to reload should be the smallest amount between available bullets, and room in clip.
	const int32 BulletsToReload = FMath::Min(BulletsDepletedFromClip, BulletsInReserve);

	if (0 < BulletsToReload)
	{
		// Add bullets to clip.
		CurrentAmmoInClip += BulletsToReload;
	}

	FNexusLogging::Log(ELogLevel::INFO, "Weapon has finished reloading");
	bReloading = false;
}

void ANexusWeapon::ServerReload_Implementation()
{
	// The reload code will execute via the server authority.
	Reload();
}

bool ANexusWeapon::ServerReload_Validate()
{
	return true;
}

void ANexusWeapon::ServerPlayAnimationMontage_Implementation(UAnimMontage* AnimMontage, float PlaybackRate)
{
	// To get something to execute on all clients (and the server), you need to use a NetMulticast function, that is called from the server.
	// So to play an animation on all clients, we must call (from a client/server) this function that will execute on the server. Now executing
	// on the server, we call the below function, which will be executed on all connected clients.

	MulticastPlayAnimationMontage(AnimMontage, PlaybackRate);
}

void ANexusWeapon::MulticastPlayAnimationMontage_Implementation(UAnimMontage* AnimMontage, float PlaybackRate)
{
	// Although this function is called on all clients, OwningCharacter must also be replicated for the animation to be played.
	OwningCharacter->PlayAnimMontage(AnimMontage, PlaybackRate);
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

	// Play the fired sound effect.
	PlayFiredSFX();
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

void ANexusWeapon::PlayFiredSFX() const
{
	if (FiredSFX)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, FiredSFX, GetActorLocation());
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

void ANexusWeapon::DepleteAmmo()
{
	CurrentAmmoInClip = FMath::Max(0, --CurrentAmmoInClip);
	CurrentTotalAmmo = FMath::Max(0, --CurrentTotalAmmo);

	FStringFormatOrderedArguments LogArgs;
	LogArgs.Add(FStringFormatArg(CurrentAmmoInClip));
	LogArgs.Add(FStringFormatArg(CurrentTotalAmmo));

	FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Ammo in clip: {0}. Total ammo: {1}."), LogArgs));
}
