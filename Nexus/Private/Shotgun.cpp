// Toyan Green © 2020

#include "Shotgun.h"
#include "NexusCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"


void AShotgun::StartFiring()
{
	// Weapon should fire once, if the time elapsed since the last shot is greater than the fire delay.
	if (GetWorld()->GetTimeSeconds() >= LastFireTime + WeaponFireDelayTime)
	{
		Fire();
	}
}

void AShotgun::Fire()
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

			// Fire a line trace to act as each "pellet" in the shot
			for (int i = 1; i < NumberOfPelletsInShot; ++i)
			{
				LineTraceForDamageAndImpactEffects(WeaponOwner, BulletTracerTarget, SurfaceType);
			}

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
