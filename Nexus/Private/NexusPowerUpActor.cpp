// Toyan Green © 2020


#include "NexusPowerUpActor.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"

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

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	
	TicksProcessed = 0;

	// Needs to be set to replicate explosion across all clients.
	SetReplicates(true);
}

void ANexusPowerUpActor::ActivatePowerUp(AActor* ActivatingActor)
{
	PowerUpActivatingActor = ActivatingActor;
	
	// Set a timer to apply the power up effect.
	if (PowerUpInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUp, this, &ANexusPowerUpActor::PowerUpTick, PowerUpInterval, true, 0.0f);
	}
	else
	{
		// Apply the effect once.
		PowerUpTick();
	}

	// Set flag to replicate activation.
	bPowerUpActivated = true;
	
	// Activate power up locally.
	OnRep_PowerUpActivated();	
}

void ANexusPowerUpActor::DeactivatePowerUp()
{
	// The power up has expired, so deactivate the effect.
	OnDeactivated();

	// Set flag to replicate deactivation.
	bPowerUpActivated = false;

	// Deactivate power up locally.
	OnRep_PowerUpActivated();
	
	// Clear the timer so we stop processing the power up.
	GetWorldTimerManager().ClearTimer(TimerHandle_PowerUp);
}

void ANexusPowerUpActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the activated flag so that we can replicate the power up activation.
	DOREPLIFETIME(ANexusPowerUpActor, bPowerUpActivated);
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

	// Set timer to make the power up "float".
	GetWorldTimerManager().SetTimer(TimerHandle_Float, this, &ANexusPowerUpActor::Float, FloatInterval, true);
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
		DeactivatePowerUp();
	}
	else
	{
		// Apply the power up effect.
		ApplyPowerUpEffect();
	}
}

void ANexusPowerUpActor::Float()
{
	// Get the current location.
	FVector NewLocation = GetActorLocation();
	// Get the current time.
	const float ActorTime = GetGameTimeSinceCreation();
	// Calculate the height change using the current time and update interval
	const float DeltaHeight = (FMath::Sin(ActorTime + FloatInterval) - FMath::Sin(ActorTime));
	// Update new location using scale factor.
	NewLocation.Z += DeltaHeight * FloatHeightChangeScale;
	// Set new location.
	SetActorLocation(NewLocation);
}

void ANexusPowerUpActor::OnRep_PowerUpActivated()
{
	OnPowerUpStateChanged(bPowerUpActivated);
}

void ANexusPowerUpActor::OnPowerUpStateChanged(bool bActive)
{
	// Run this functionality if the power up was activated
	if (bActive)
	{
		// Mesh and light should be hidden off when the power up is collected/activated.
		MeshComponent->SetVisibility(false);
		LightComponent->SetVisibility(false);
	}
	else
	{
		// Power up should be destroyed when deactivated.
		Destroy();
	}
}
