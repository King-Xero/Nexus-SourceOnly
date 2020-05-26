// Toyan Green © 2020


#include "GrenadeLauncher.h"
#include "Kismet/GameplayStatics.h"

void AGrenadeLauncher::StartFiring()
{
	// Weapon should fire once, if the time elapsed since the last shot is greater than the fire delay.
	if (GetWorld()->GetTimeSeconds() >= LastFireTime + WeaponFireDelayTime)
	{
		Fire();
	}
}

void AGrenadeLauncher::Fire()
{
	AActor* WeaponOwner = GetOwner();

	if (WeaponOwner && ProjectileClass)
	{
		FVector EyeLocation; // Start location for line trace.
		FRotator EyeRotation;
		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		const FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		// Set spawn collision handling override.
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Ensure we set the instigator on the spawned projectile
		ActorSpawnParams.Instigator = Cast<APawn>(GetOwner());

		// Spawn the projectile at the muzzle
		AGrenadeLauncherProjectile* Grenade = GetWorld()->SpawnActor<AGrenadeLauncherProjectile>(
			ProjectileClass, MuzzleLocation, EyeRotation, ActorSpawnParams);
		if (Grenade)
		{
			Grenade->SetDamageAmount(WeaponDamage);
			Grenade->SetDamageType(DamageType);
			Grenade->SetDamageRadius(DamageRadius);
		}

		// Spawn particle effect for muzzle flash.
		PlayMuzzleEffect();

		PlayCameraShake();

		// This needs to be set to prevent the firing rate getting bypassed with rapid firing input.
		LastFireTime = GetWorld()->GetTimeSeconds();
	}
}
