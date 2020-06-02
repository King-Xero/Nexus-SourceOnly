// Toyan Green © 2020

#include "ExplodingBarrel.h"
#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "DrawDebugHelpers.h"
#include "Nexus/Utils/ConsoleVariables.h"
#endif

// Sets default values
AExplodingBarrel::AExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialise components
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	// Simulate physics must be true for the barrel to move when it receives an impulse.
	MeshComponent->SetSimulatePhysics(true);
	RootComponent = MeshComponent;
	
	BarrelHealthComponent = CreateDefaultSubobject<UNexusHealthComponent>(TEXT("HealthComponent"));

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->bImpulseVelChange = true;
	// If auto activate is not set to false, the component will run update code that constantly applies forces.
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->bIgnoreOwningActor = true;

	// Needs to be set to replicate explosion across all clients.
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called every frame
void AExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplodingBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the exploded flag so that we can replicate the explosion.
	DOREPLIFETIME(AExplodingBarrel, bExploded);
}

// Called when the game starts or when spawned
void AExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();

	// Wire up health changed event.
	BarrelHealthComponent->OnHealthChanged.AddDynamic(this, &AExplodingBarrel::HealthChanged);

	// Variables would not set correctly in the constructor, so have to be set here.
	RadialForceComponent->Radius = ExplosionRadius;
	RadialForceComponent->ImpulseStrength = RadialImpulseStrength;
}

void AExplodingBarrel::HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// If the barrel's health is 0 or less and not currently exploded, the barrel should explode.
	if (0.0f >= Health && !bExploded)
	{
		FNexusLogging::Log(ELogLevel::DEBUG, "Barrel has exploded.");

		// Set barrel instigator to damage instigator, so we know who caused the barrel to explode when inflicting damage from the barrel.
		if (InstigatedBy)
		{
			SetInstigator(InstigatedBy->GetPawn());
		}		

		Explode();

		// Setting the lifespan will cause the character to be destroyed after the time value passed.
		SetLifeSpan(ExplosionLifeSpan);
	}
}

void AExplodingBarrel::Explode()
{
	// Play explosion effects locally.
	OnRep_Explode();
	
	// Must be set before applying radial damage or we can get caught in an infinite loop.
	bExploded = true;
	
	AActor* ExplosionInstigator = GetInstigator();

	AController* InstigatorController = ExplosionInstigator ? ExplosionInstigator->GetInstigatorController() : nullptr;
	
	// Apply damage to actors around the barrel.
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, BarrelDamageType, {}, this, InstigatorController);
	
	// Launch barrel upwards.
	MeshComponent->AddImpulse(FVector::UpVector * VerticalImpulseStrength, NAME_None, true);

	// Emit radial force.
	RadialForceComponent->FireImpulse();
}

void AExplodingBarrel::OnRep_Explode() const
{
	// Spawn particle effect for explosion.
	PlayExplosionEffect();

	// Change the material on the barrel.
	SetExplodedMaterial();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const bool bDrawDebug = CVarDebugWeaponDrawing.GetValueOnGameThread();
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12.0f, FColor::Yellow, false, 1.0f, 0, 1.0f);
	}
#endif
}

void AExplodingBarrel::PlayExplosionEffect() const
{
	if (ExplosionVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), MeshComponent->GetComponentRotation());
	}
}

void AExplodingBarrel::SetExplodedMaterial() const
{
	if (ExplodedMaterial)
	{
		MeshComponent->SetMaterial(0, ExplodedMaterial);
	}
}
