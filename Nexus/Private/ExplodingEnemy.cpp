// Toyan Green © 2020


#include "ExplodingEnemy.h"

// Sets default values
AExplodingEnemy::AExplodingEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialise components
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	// Set so that the enemy doesn't cause a gap in the nav mesh
	MeshComponent->SetCanEverAffectNavigation( false);
	RootComponent = MeshComponent;
}

// Called every frame
void AExplodingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void AExplodingEnemy::BeginPlay()
{
	Super::BeginPlay();
}