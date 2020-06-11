// Toyan Green © 2020


#include "NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Nexus/Utils/NexusTypeDefinitions.h"
#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANexusCharacter::ANexusCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialise the camera components
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// Initialise the health component
	CharacterHealthComponent = CreateDefaultSubobject<UNexusHealthComponent>(TEXT("HealthComponent"));
	
	// Disable weapon collisions on the capsule component so that the mesh collisions work as expected.
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_OBJECT_PROJECTILE, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_TRACE_WEAPON, ECR_Ignore);

	// This must be set to allow the character to crouch.
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

// Called every frame
void ANexusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetAimDownSight(DeltaTime);
}

// Called to bind functionality to input
void ANexusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(MoveForwardBindingName, this, &ANexusCharacter::MoveForward);
	PlayerInputComponent->BindAxis(MoveRightBindingName, this, &ANexusCharacter::MoveRight);

	PlayerInputComponent->BindAxis(LookUpBindingName, this, &ANexusCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TurnBindingName, this, &ANexusCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction(CrouchBindingName, IE_Pressed, this, &ANexusCharacter::StartCrouch);
	PlayerInputComponent->BindAction(CrouchBindingName, IE_Released, this, &ANexusCharacter::EndCrouch);

	PlayerInputComponent->BindAction(JumpBindingName, IE_Pressed, this, &ANexusCharacter::Jump);

	PlayerInputComponent->BindAction(AimingBindingName, IE_Pressed, this, &ANexusCharacter::StartADS);
	PlayerInputComponent->BindAction(AimingBindingName, IE_Released, this, &ANexusCharacter::EndADS);

	PlayerInputComponent->BindAction(ShootBindingName, IE_Pressed, this, &ANexusCharacter::StartShooting);
	PlayerInputComponent->BindAction(ShootBindingName, IE_Released, this, &ANexusCharacter::StopShooting);
}

FVector ANexusCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		// View for character should be that of the 3rd person camera.
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

bool ANexusCharacter::IsDead() const
{
	return bDead;
}

void ANexusCharacter::StartShooting()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

void ANexusCharacter::StopShooting()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void ANexusCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Because weapon is only spawned on the server, we have to ensure that it is replicated, so the clients can use it.
	DOREPLIFETIME(ANexusCharacter, CurrentWeapon);

	// Replicate the dead flag so that we can replicate the death animation.
	DOREPLIFETIME(ANexusCharacter, bDead);
}

// Called when the game starts or when spawned
void ANexusCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Cache FOV so that we can reset when we stop ADS.
	DefaultFOV = CameraComponent->FieldOfView;

	if (ROLE_Authority == GetLocalRole())
	{
		// Set spawn collision handling override.
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn the default character weapon
		CurrentWeapon = GetWorld()->SpawnActor<ANexusWeapon>(SpawnWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, ActorSpawnParams);

		if (CurrentWeapon)
		{
			// Set owner for use in weapon fire function.
			CurrentWeapon->SetOwner(this);
			// Attach the weapon to the character's hand.
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}
	}	

	// Wire up health changed event.
	CharacterHealthComponent->OnHealthChanged.AddDynamic(this, &ANexusCharacter::HealthChanged);
}

/**
 * \brief Moves the character forward or backward, depending on axis value.
 * \param fAxisValue Value of the input axis.
 */
void ANexusCharacter::MoveForward(float fAxisValue)
{
	AddMovementInput(GetActorForwardVector(), fAxisValue);
}

/**
 * \brief Moves the character left or right, depending on axis value.
 * \param fAxisValue Value of the input axis.
 */
void ANexusCharacter::MoveRight(float fAxisValue)
{
	AddMovementInput(GetActorRightVector(), fAxisValue);
}

/**
 * \brief Make the character crouch from standing
 */
void ANexusCharacter::StartCrouch()
{
	Crouch();
}

/**
 * \brief Make the character stand from crouching
 */
void ANexusCharacter::EndCrouch()
{
	UnCrouch();
}

void ANexusCharacter::StartADS()
{
	// "Zoom" the camera in.
	bAimDownSight = true;
}

void ANexusCharacter::EndADS()
{
	// "Zoom" the camera out.
	bAimDownSight = false;
}

void ANexusCharacter::HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// If the character's health is 0 or less and not currently dead, the character should die.
	if (0.0f >= Health && !bDead)
	{
		FNexusLogging::Log(ELogLevel::DEBUG, "Character has died.");
		
		bDead = true;

		// Disable all collisions on capsule component.
		UCapsuleComponent* cCapsuleCollider = GetCapsuleComponent();
		cCapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		cCapsuleCollider->SetCollisionResponseToAllChannels(ECR_Ignore);

		// Immediately stop all movement, and prevent the component from updating.
		UPawnMovementComponent* cMovementComponent = GetMovementComponent();
		cMovementComponent->StopMovementImmediately();
		cMovementComponent->SetComponentTickEnabled(false);

		// Disable further movement and camera control.
		DetachFromControllerPendingDestroy();

		// Setting the lifespan will cause the character to be destroyed after the time value passed.
		SetLifeSpan(DeathLifeSpan);
	}
}

void ANexusCharacter::SetAimDownSight(float DeltaTime)
{
	// Interpolate the aim down sight for a smooth aim in/out.
	
	const float TargetFOV = bAimDownSight ? AimingFOV : DefaultFOV;

	const float InterpolationFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ADSInterpolationSpeed);

	CameraComponent->SetFieldOfView(InterpolationFOV);
}