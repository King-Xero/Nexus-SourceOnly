// Toyan Green © 2020


#include "ExplodingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

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
