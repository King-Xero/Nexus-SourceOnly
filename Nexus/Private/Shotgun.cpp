// Toyan Green © 2020

#include "Shotgun.h"
#include "NexusCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Nexus/Utils/NexusTypeDefinitions.h"
#include "Kismet/GameplayStatics.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "DrawDebugHelpers.h"
#include "Nexus/Utils/ConsoleVariables.h"
#endif

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
		
			// Start location for line trace.
			FVector EyeLocation;
			FRotator EyeRotation;
			// Bullet tracer target parameter.
			FVector BulletTracerTarget;
			// The type of surface that was hit. Used to add damage multiplier and play different effects.
			EPhysicalSurface SurfaceType = SurfaceType_Default;
		
			WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FVector ShotDirection = EyeRotation.Vector();

			for (int i = 1; i < NumberOfPelletsInShot; ++i)
			{
				// Add "bullet spread" to shot direction. Shots are/can be more accurate while aiming down sights.
				const float HalfAngleRad = OwningCharacter && OwningCharacter->IsAimingDownSights() ? FMath::DegreesToRadians(ADSBulletSpreadAngle)
					                           : FMath::DegreesToRadians(BulletSpreadAngle);
				ShotDirection = FMath::VRandCone(ShotDirection, HalfAngleRad);

				FVector TraceEnd = EyeLocation + (ShotDirection * WeaponRange); // End location for eye trace.

				/*TraceEnd.X += FMath::RandRange(-HorizontalSpreadRange, HorizontalSpreadRange);
			TraceEnd.Z += FMath::RandRange(-VerticalSpreadRange, VerticalSpreadRange);*/

				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(WeaponOwner); // Ignore collisions with the weapon owner.
				CollisionQueryParams.AddIgnoredActor(this); // Ignore collisions with the weapon itself.
				CollisionQueryParams.bTraceComplex = true;
				// Complex collision is more expensive, but we get exact location of where was hit.
				CollisionQueryParams.bReturnPhysicalMaterial = true;
				// Ensure that the surface material is returned to check what body part was hit.
			
				BulletTracerTarget = TraceEnd;

				SurfaceType = SurfaceType_Default;

				FHitResult WeaponHitResult;
				// Trace the world between the start and end locations. Returns true if blocking hit.
				if (GetWorld()->LineTraceSingleByChannel(WeaponHitResult, EyeLocation, TraceEnd, COLLISION_TRACE_WEAPON, CollisionQueryParams))
				{
					AActor* HitActor = WeaponHitResult.GetActor();

					// Get the surface type that was hit.
					SurfaceType = UPhysicalMaterial::DetermineSurfaceType(WeaponHitResult.PhysMaterial.Get());

					// Calculate the amount of damage to inflict.
					float DamageToInflict = WeaponDamage * GetDamageMultiplier(SurfaceType);

					// Apply damage to the hit actor.
					UGameplayStatics::ApplyPointDamage(HitActor, DamageToInflict, ShotDirection, WeaponHitResult,
					                                   WeaponOwner->GetInstigatorController(), this, DamageType);

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
