// Toyan Green © 2020

#include "ExplodingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "NexusCharacter.h"
#include "Components/AudioComponent.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "DrawDebugHelpers.h"
#include "Nexus/Utils/ConsoleVariables.h"
#endif

// Sets default values
AExplodingEnemy::AExplodingEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialise components
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	// Set so that the enemy doesn't cause a gap in the nav mesh.
	MeshComponent->SetCanEverAffectNavigation( false);
	// Set so that the enemy can be moved using forces.
	MeshComponent->SetSimulatePhysics(true);
	RootComponent = MeshComponent;

	// Initialise the health component
	EnemyHealthComponent = CreateDefaultSubobject<UNexusHealthComponent>(TEXT("HealthComponent"));

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->bImpulseVelChange = true;
	// If auto activate is not set to false, the component will run update code that constantly applies forces.
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->bIgnoreOwningActor = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// Configure collisions to only detect overlaps with pawns.
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetSphereRadius(ExplosionRadius);
	SphereComponent->SetupAttachment(RootComponent);

	MovementAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudioComponent"));
	MovementAudioComponent->SetupAttachment(RootComponent);
}

// Called every frame
void AExplodingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		// If the enemy is within range of the path point, get the next point.
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		// Move to the next target.

		// Calculate the direction the enemy should move in.
		FVector MovementForceDirection = NextPathPoint - GetActorLocation();
		MovementForceDirection.Normalize();

		MeshComponent->AddForce(MovementForceDirection * MovementForce, NAME_None, bVelocityChange);
	}

	// Set the volume of the movement sound effect, depending on the enemy's current velocity.
	MovementAudioComponent->SetVolumeMultiplier(FMath::GetMappedRangeValueClamped(TRange<float>(LowerVelocityRollingRange, UpperVelocityRollingRange), TRange<float>(LowerVolumeRollingRange, UpperVolumeRollingRange), GetVelocity().Size()));
}

void AExplodingEnemy::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bSelfDestruct)
	{
		ANexusCharacter* PlayerCharacter = Cast<ANexusCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			// If a player was overlapped, begin self-destruct.

			FStringFormatOrderedArguments LogArgs;
			LogArgs.Add(FStringFormatArg(GetName()));
			LogArgs.Add(FStringFormatArg(PlayerCharacter->GetName()));

			FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Enemy {0} detected player {1}. Starting self destruct."), LogArgs));

			// Play self destruct effect.
			PlaySelfDestructSFX();
			
			// Start self destruct timer.
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDestruct, this, &AExplodingEnemy::Explode, SelfDestructTime, false);
			bSelfDestruct = true;
		}
	}	
}

void AExplodingEnemy::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the exploded flag so that we can replicate the explosion.
	DOREPLIFETIME(AExplodingEnemy, bExploded);
}

// Called when the game starts or when spawned
void AExplodingEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Find the initial point to move towards.
	NextPathPoint = GetNextPathPoint();

	// Wire up health changed event.
	EnemyHealthComponent->OnHealthChanged.AddDynamic(this, &AExplodingEnemy::HealthChanged);

	MaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));

	// Variables would not set correctly in the constructor, so have to be set here.
	RadialForceComponent->Radius = ExplosionRadius;
	RadialForceComponent->ImpulseStrength = RadialImpulseStrength;

	// Set sound effect to be played.
	MovementAudioComponent->SetSound(MovementSFX);
	MovementAudioComponent->Play();
}

FVector AExplodingEnemy::GetNextPathPoint()
{
	// Get the first player in the world. **Will need to change for multiple players.**
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Get the path from the enemy's current position to the player.
	UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerCharacter);

	if (1 < NavigationPath->PathPoints.Num())
	{
		// Return the second point in the navigation path.
		return NavigationPath->PathPoints[1];
	}

	// If there is a problem getting a path point, return the enemy's current location.
	return GetActorLocation();
}

void AExplodingEnemy::HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	FStringFormatOrderedArguments LogArgs;
	LogArgs.Add(FStringFormatArg(HealthDelta));
	LogArgs.Add(FStringFormatArg(GetName()));

	FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("{0} damage inflicted to {1}."), LogArgs));

	if (MaterialInstance)
	{
		// Pulse the material.
		MaterialInstance->SetScalarParameterValue(MaterialInstanceDamageParameterName, GetWorld()->GetTimeSeconds());
	}

	// If the enemy's health is 0 or less and not currently exploded, the enemy should explode.
	if (0.0f >= Health && !bExploded)
	{
		FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Enemy {1} has exploded."), LogArgs));

		Explode();
	}
}

void AExplodingEnemy::Explode()
{
	// Clear the timer in case it was set.
	GetWorldTimerManager().ClearTimer(TimerHandle_SelfDestruct);
	
	// Apply damage to actors around the enemy.
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, EnemyDamageType, {}, this);

	// Emit radial force.
	RadialForceComponent->FireImpulse();

	OnRep_Explode();

	bExploded = true;

	Destroy();
}

void AExplodingEnemy::OnRep_Explode() const
{
	// Spawn particle effect for explosion.
	PlayExplosionVFX();

	// Spawn sound effect for explosion.
	PlayExplosionSFX();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const bool bDrawDebug = CVarDebugWeaponDrawing.GetValueOnGameThread();
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12.0f, FColor::Yellow, false, 1.0f, 0, 1.0f);
	}
#endif
}

void AExplodingEnemy::PlayExplosionVFX() const
{
	if (ExplosionVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), MeshComponent->GetComponentRotation());
	}
}

void AExplodingEnemy::PlaySelfDestructSFX() const
{
	if (SelfDestructSFX)
	{
		UGameplayStatics::SpawnSoundAttached(SelfDestructSFX, RootComponent);
	}
}

void AExplodingEnemy::PlayExplosionSFX() const
{
	if (ExplosionSFX)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSFX, GetActorLocation());
	}
}