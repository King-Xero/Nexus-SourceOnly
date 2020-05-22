// Toyan Green © 2020


#include "NexusWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

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
		
		FHitResult WeaponHitResult;		
		// Trace the world between the start and end locations. Returns true if blocking hit.
		if(GetWorld()->LineTraceSingleByChannel(WeaponHitResult, EyeLocation, TraceEnd, ECC_Visibility))
		{
			AActor* HitActor = WeaponHitResult.GetActor();

			// Apply damage to the hit actor
			UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, ShotDirection, WeaponHitResult, WeaponOwner->GetInstigatorController(), this, DamageType);
		}
		
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
	}
	
}

// Called every frame
void ANexusWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}