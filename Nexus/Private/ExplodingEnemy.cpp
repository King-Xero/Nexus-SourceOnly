// Toyan Green © 2020


#include "ExplodingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"

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
	
	// If the enemy's health is 0 or less and not currently dead, the enemy should die.
	if (0.0f >= Health && !bDead)
	{
		FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Enemy {1} has died.."), LogArgs));

		bDead = true;
	}
}
