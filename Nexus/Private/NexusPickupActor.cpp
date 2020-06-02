// Toyan Green © 2020

#include "NexusPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "NexusPowerUpActor.h"

// Sets default values
ANexusPickupActor::ANexusPickupActor()
{
 	// Pick up does not need to update every frame.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(70.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	// Rotate the decal so it has the correct orientation by default.
	DecalComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(65.0f, 75.0f, 75.0f);
	DecalComponent->SetupAttachment(RootComponent);
}

void ANexusPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (SpawnedPowerUp)
	{
		SpawnedPowerUp->ActivatePowerUp();
		SpawnedPowerUp = nullptr;

		// Set timer to respawn the power up.
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnPowerUp, this, &ANexusPickupActor::SpawnPowerUp, PickUpCoolDown);
	}
}

// Called when the game starts or when spawned
void ANexusPickupActor::BeginPlay()
{
	Super::BeginPlay();

	// Pick up should start with power up spawned.
	SpawnPowerUp();
}

void ANexusPickupActor::SpawnPowerUp()
{
	if (PowerUpClass && !SpawnedPowerUp)
	{
		FActorSpawnParameters SpawnParameters;
		// Ensure that the power up will always spawn.
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn transform needs to be calculated to that the power up does not spawn in the floor.
		FTransform SpawnTransform = GetTransform();
		FVector SpawnTranslation = SpawnTransform.GetTranslation();
		SpawnTranslation.Z += SpawnHeight;
		SpawnTransform.SetTranslation(SpawnTranslation);

		// Spawn an instance of the power up actor.
		SpawnedPowerUp = GetWorld()->SpawnActor<ANexusPowerUpActor>(PowerUpClass, SpawnTransform, SpawnParameters);
	}
}