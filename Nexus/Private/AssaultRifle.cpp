// Toyan Green © 2020

#include "AssaultRifle.h"
#include "NexusCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

void AAssaultRifle::StartFiring()
{
	// Need to calculate a first delay to that the firing rate cannot be bypassed with rapid input.
	// The first delay has to be at least 0, as passing a negative value has reserved functionality.
	const float FirstDelay = FMath::Max(LastFireTime + WeaponFireDelayTime - GetWorld()->TimeSeconds, 0.0f);

	// Start looping timer to start automatic fire.
	GetWorldTimerManager().SetTimer(TimerHandle_WeaponFireDelay, this, &AAssaultRifle::Fire, WeaponFireDelayTime, true, FirstDelay);
}

void AAssaultRifle::Fire()
{
	AActor* WeaponOwner = GetOwner();

	if (WeaponOwner && CanFireWeapon())
	{
		if (!HasAmmoInClip())
		{
			// There is no ammo loaded in the clip, so the weapon should dry fire.
			PlayDryFiredSFX();
			StopFiring();
		}
		else
		{
			SetWeaponState(EWeaponState::Firing);

			// Fire should only be called via the server authority.
			if (ROLE_Authority > GetLocalRole())
			{
				ServerFire();
			}

			// Use ammo.
			DepleteAmmo();

			// Bullet tracer target parameter.
			FVector BulletTracerTarget;
			// The type of surface that was hit. Used to add damage multiplier and play different effects.
			EPhysicalSurface SurfaceType = SurfaceType_Default;

			// Fire a line trace to act as the "bullet".
			LineTraceForDamageAndImpactEffects(WeaponOwner, BulletTracerTarget, SurfaceType);

			// Play weapon effects locally.
			PlayWeaponFiredEffects(BulletTracerTarget);

			PlayFiredSFX();

			PlayFiredAnimation();

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
}