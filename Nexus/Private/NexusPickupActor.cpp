// Toyan Green © 2020

#include "NexusPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

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
}

// Called when the game starts or when spawned
void ANexusPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}