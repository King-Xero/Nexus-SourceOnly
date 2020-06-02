// Toyan Green © 2020


#include "NexusPowerUpActor.h"
#include "Components/PointLightComponent.h"

// Sets default values
ANexusPowerUpActor::ANexusPowerUpActor()
{
 	// Power up does not need to tick.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComponent;

	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComponent"));
	// Configure default light properties.
	LightComponent->SetCastShadows(false);
	LightComponent->SetAttenuationRadius(100.0f);
	LightComponent->SetupAttachment(RootComponent);
	
	TicksProcessed = 0;
}

void ANexusPowerUpActor::ActivatePowerUp()
{
	// Set a timer to apply the power up effect.
	if (PowerUpInterval > 1)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUp, this, &ANexusPowerUpActor::PowerUpTick, PowerUpInterval, true, 0.0f);
	}
	else
	{
		// Apply the effect once.
		PowerUpTick();
	}

	// Mesh and light should be hidden off when the power up is collected/activated.
	MeshComponent->SetVisibility(false);
	LightComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void ANexusPowerUpActor::BeginPlay()
{
	Super::BeginPlay();

	PowerUpMaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	if (PowerUpMaterialInstance)
	{
		// Set the color of the power up, configure the mesh material flashing.
		PowerUpMaterialInstance->SetVectorParameterValue(MaterialInstanceColorParameterName, PowerUpColor);
		PowerUpMaterialInstance->SetScalarParameterValue(MaterialInstanceTimeMultiplierParameterName, PowerUpFlashTimerMultiplier);
		PowerUpMaterialInstance->SetScalarParameterValue(MaterialInstanceMinimumEmissiveParameterName, PowerUpFlashMinimumEmissive);
	}
		
	LightMaterialInstance = UMaterialInstanceDynamic::Create(LightComponent->GetMaterial(0), LightComponent);
	if (LightMaterialInstance)
	{
		// Configure the light flashing.
		LightMaterialInstance->SetScalarParameterValue(MaterialInstanceTimeMultiplierParameterName, PowerUpFlashTimerMultiplier);
		LightMaterialInstance->SetScalarParameterValue(MaterialInstanceMinimumEmissiveParameterName, PowerUpFlashMinimumEmissive);
		LightComponent->SetMaterial(0, LightMaterialInstance);
	}
	// Set the light color.
	LightComponent->SetLightColor(PowerUpColor);
}

void ANexusPowerUpActor::PowerUpTick()
{
	if (0 == TicksProcessed)
	{
		// This should only ever get called once on the first activation.
		OnActivated();
	}
	
	if (++TicksProcessed > TotalNumberOfTicks)
	{
		// The power up has expired, so deactivate the effect.
		OnDeactivated();

		// Clear the timer so we stop processing the power up.
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUp);
	}
	else
	{
		// Apply the power up effect.
		ApplyPowerUpEffect();
	}
}