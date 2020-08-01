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
#include "Kismet/GameplayStatics.h"
#include "ExplosiveDamageType.h"
#include "BulletDamageType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

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

	CharacterCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CharacterCaptureComponent"));
	CharacterCaptureComponent->SetupAttachment(GetMesh(), HeadSocketName);
	
	// Initialise the health component
	CharacterHealthComponent = CreateDefaultSubobject<UNexusHealthComponent>(TEXT("HealthComponent"));
	
	// Disable weapon collisions on the capsule component so that the mesh collisions work as expected.
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_OBJECT_PROJECTILE, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_TRACE_WEAPON, ECR_Ignore);

	// This must be set to true to detect the physical material of the floor the character is walking on.
	GetCapsuleComponent()->bReturnMaterialOnMove = true;

	// This must be set to allow the character to crouch.
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

// Called every frame
void ANexusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetAimDownSight(DeltaTime);

	SetAimAngles(DeltaTime);
}

// Called to bind functionality to input
void ANexusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(MoveForwardBindingName, this, &ANexusCharacter::MoveForward);
	PlayerInputComponent->BindAxis(MoveRightBindingName, this, &ANexusCharacter::MoveRight);

	PlayerInputComponent->BindAxis(LookUpBindingName, this, &ANexusCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TurnBindingName, this, &ANexusCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction(CrouchBindingName, IE_Pressed, this, &ANexusCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction(JumpBindingName, IE_Pressed, this, &ANexusCharacter::Jump);

	PlayerInputComponent->BindAction(AimingBindingName, IE_Pressed, this, &ANexusCharacter::StartADS);
	PlayerInputComponent->BindAction(AimingBindingName, IE_Released, this, &ANexusCharacter::EndADS);

	PlayerInputComponent->BindAction(ShootBindingName, IE_Pressed, this, &ANexusCharacter::StartShooting);
	PlayerInputComponent->BindAction(ShootBindingName, IE_Released, this, &ANexusCharacter::StopShooting);

	PlayerInputComponent->BindAction(ReloadBindingName, IE_Released, this, &ANexusCharacter::StartReloading);
	
	PlayerInputComponent->BindAction(SwapWeaponBindingName, IE_Released, this, &ANexusCharacter::InitiateWeaponSwap);
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

void ANexusCharacter::Jump()
{
	if (CanCrouch())
	{
		// If the character can crouch, then it is not crouched, meaning it can (and should) jump.
		Super::Jump();
	}
	else
	{
		// If the character cannot crouch, it is crouched, meaning it should stand up.
		EndCrouch();
	}
}

bool ANexusCharacter::IsDead() const
{
	return bDead;
}

bool ANexusCharacter::IsAimingDownSights() const
{
	return bAimDownSight;
}

float ANexusCharacter::GetAimPitch() const
{
	return AimPitchAngle;
}

float ANexusCharacter::GetAimYaw() const
{
	return AimYawAngle;
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

void ANexusCharacter::StartReloading()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReloading();
	}
}

void ANexusCharacter::DeathRagdollCharacter() const
{
	// Enable full ragdoll.
	GetMesh()->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();

	// Set mesh to update joints from animation, and blend physics with animation.
	// This results in a hybrid ragdoll effect. Rather than completely collapsing the ragdoll is partially driven by the any playing animation.
	GetMesh()->SetAllBodiesPhysicsBlendWeight(RagdollPhysicsBlendWeight);
	GetMesh()->bUpdateJointsFromAnimation = true;

	GetMesh()->bBlendPhysics = true;

	// Weapon ragdoll.
	if (CurrentWeapon)
	{
		USkeletalMeshComponent* WeaponMeshComponent = Cast<USkeletalMeshComponent>(CurrentWeapon->GetRootComponent());
		if (WeaponMeshComponent)
		{
			WeaponMeshComponent->SetCollisionProfileName(FName(TEXT("Ragdoll")));
			WeaponMeshComponent->SetSimulatePhysics(true);
			WeaponMeshComponent->bBlendPhysics = true;
			WeaponMeshComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		}		
	}
}

void ANexusCharacter::InitiateWeaponSwap()
{
	if (CurrentWeapon && OffHandWeapon && CurrentWeapon->CanWeaponBeSwapped() && WeaponSwapAnimMontage)
	{
		// Play reload animation.
		const float SwapMontagePlaybackRate = WeaponSwapAnimMontage->GetPlayLength() / WeaponSwapDuration;

		if (ROLE_Authority > GetLocalRole())
		{
			// Play the animation via the server authority.
			ServerPlayAnimationMontage(WeaponSwapAnimMontage, SwapMontagePlaybackRate);
		}
		else
		{
			// Play the animation.
			MulticastPlayAnimationMontage(WeaponSwapAnimMontage, SwapMontagePlaybackRate);
		}

		// Reloading should be cancelled and firing stopped when the weapon is swapped.
		CurrentWeapon->StopReloading();
		CurrentWeapon->StopFiring();
		
		CurrentWeapon->SetWeaponState(EWeaponState::Swapping);
	}
}

void ANexusCharacter::SwapWeapon()
{
	if (CurrentWeapon && OffHandWeapon)
	{
		ANexusWeapon* WeaponToEquip = OffHandWeapon;

		// Stash the current weapon
		OffHandWeapon = CurrentWeapon;
		OffHandWeapon->SetWeaponState(EWeaponState::Offhand);
		AttachWeaponToSocket(OffHandWeapon, OffHandWeapon1SocketName);

		// Equip the offhand weapon.
		CurrentWeapon = WeaponToEquip;
		CurrentWeapon->SetWeaponState(EWeaponState::Swapping);
		AttachWeaponToSocket(CurrentWeapon, EquippedWeaponSocketName);
	}	
}

void ANexusCharacter::PrepareWeaponAfterSwap() const
{
	if (CurrentWeapon)
	{
		// Weapon should be in idle state when its swapped in.
		CurrentWeapon->SetWeaponState(EWeaponState::Idle);
	}
}

void ANexusCharacter::FillAmmo()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->RestoreAmmo(CurrentWeapon->GetMaxAmmoCapacity());
	}

	if (OffHandWeapon)
	{
		OffHandWeapon->RestoreAmmo(OffHandWeapon->GetMaxAmmoCapacity());
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

	// Cache max walk speed so we can reset when we stop ADS.
	DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	SpawnAndAttachStartingWeapons();
	
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

void ANexusCharacter::ToggleCrouch()
{
	if (CanCrouch())
	{
		StartCrouch();
	}
	else
	{
		EndCrouch();
	}
}

/**
 * \brief Make the character crouch from standing
 */
void ANexusCharacter::StartCrouch()
{
	// If the player can't jump (is in the air) then they shouldn't be able to crouch.
	if(CanJump())
	{
		PlayCrouchSFX();
		Crouch();
	}	
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

	OnADSUpdated.Broadcast(this, bAimDownSight);

	// Movement speed is limited while ADS.
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
}

void ANexusCharacter::EndADS()
{
	// "Zoom" the camera out.
	bAimDownSight = false;

	OnADSUpdated.Broadcast(this, bAimDownSight);

	// Restore movement speed when stopping ADS.
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
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
		
		// Explosive damage resulting in death should cause immediate ragdoll.
		if (DamageType->IsA(UExplosiveDamageType::StaticClass()))
		{
			DeathRagdollCharacter();
		}
		else
		{
			// Play death sfx
			PlayDeathSFX();

			// Play death animation. Death animation will trigger ragdoll via DeathRagdollAnimNotify.
			PlayDeathAnimation();
		}
		
		// Immediately stop all movement, and prevent the component from updating.
		UPawnMovementComponent* cMovementComponent = GetMovementComponent();
		cMovementComponent->StopMovementImmediately();
		cMovementComponent->SetComponentTickEnabled(false);

		// Disable further movement and camera control.
		/*DetachFromControllerPendingDestroy();*/

		// Setting the lifespan will cause the character/weapons to be destroyed after the time value passed.
		SetLifeSpan(DeathLifeSpan);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetLifeSpan(DeathLifeSpan);
		}		
	}
	else if (!bDead)
	{
		if (DamageType->IsA(UBulletDamageType::StaticClass()))
		{
			PlayHitAnimation();
		}
	}
}

void ANexusCharacter::ServerPlayAnimationMontage_Implementation(UAnimMontage* AnimMontage, float PlaybackRate)
{
	// To get something to execute on all clients (and the server), you need to use a NetMulticast function, that is called from the server.
	// So to play an animation on all clients, we must call (from a client/server) this function that will execute on the server. Now executing
	// on the server, we call the below function, which will be executed on all connected clients.

	MulticastPlayAnimationMontage(AnimMontage, PlaybackRate);
}

void ANexusCharacter::MulticastPlayAnimationMontage_Implementation(UAnimMontage* AnimMontage, float PlaybackRate)
{
	// Although this function is called on all clients, OwningCharacter must also be replicated for the animation to be played.
	PlayAnimMontage(AnimMontage, PlaybackRate);
}

void ANexusCharacter::SetAimDownSight(float DeltaTime)
{
	// Interpolate the aim down sight for a smooth aim in/out.
	
	const float TargetFOV = bAimDownSight ? AimingFOV : DefaultFOV;

	const float InterpolationFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ADSInterpolationSpeed);

	CameraComponent->SetFieldOfView(InterpolationFOV);
}

void ANexusCharacter::SetAimAngles(float DeltaTime)
{
	// Calculate aim angles to drive aim animation offsets.
	
	const FRotator ControlRotation = GetControlRotation();
	const FRotator ActorRotation = GetActorRotation();

	const FRotator DeltaRotation = ControlRotation - ActorRotation;

	const FRotator CurrentAimRotation(AimPitchAngle, AimYawAngle, 0);

	const FRotator NewAimRotation = FMath::RInterpTo(CurrentAimRotation, DeltaRotation, DeltaTime, 15);

	AimPitchAngle = FMath::ClampAngle(NewAimRotation.Pitch, -90, 90);
	AimYawAngle = FMath::ClampAngle(NewAimRotation.Yaw, -90, 90);
}

void ANexusCharacter::PlayCrouchSFX() const
{
	if (CrouchSFX)
	{
		UGameplayStatics::SpawnSoundAttached(CrouchSFX, GetMesh());
	}
}

void ANexusCharacter::PlayDeathSFX() const
{
	if (DeathSFX)
	{
		UGameplayStatics::SpawnSoundAttached(DeathSFX, GetMesh());
	}
}

void ANexusCharacter::PlayDeathAnimation() const
{
	// Play a random death animation from the available animations.
	const int RandomIndex = FMath::RandRange(0, DeathAnimations.Num() - 1);

	UAnimSequence* DeathAnimation = DeathAnimations[RandomIndex];

	if (DeathAnimation)
	{
		GetMesh()->PlayAnimation(DeathAnimation, false);
	}	
}

void ANexusCharacter::PlayHitAnimation()
{
	// Play a random death animation from the available animations.
	const int RandomIndex = FMath::RandRange(0, HitReactionAnimationMontages.Num() - 1);

	UAnimMontage* HitAnimation = HitReactionAnimationMontages[RandomIndex];

	if (HitAnimation)
	{
		if (ROLE_Authority > GetLocalRole())
		{
			// Play the animation via the server authority.
			ServerPlayAnimationMontage(HitAnimation);
		}
		else
		{
			// Play the animation.
			MulticastPlayAnimationMontage(HitAnimation);
		}
	}
}

void ANexusCharacter::SpawnAndAttachStartingWeapons()
{
	if (ROLE_Authority == GetLocalRole())
	{
		if (SpawnPrimaryWeaponClass)
		{
			SpawnAndAttachWeapon(CurrentWeapon, SpawnPrimaryWeaponClass, EquippedWeaponSocketName);
			// Equipped weapon should start in idle state.
			CurrentWeapon->SetWeaponState(EWeaponState::Idle);
		}
		if (SpawnSecondaryWeaponClass)
		{
			SpawnAndAttachWeapon(OffHandWeapon, SpawnSecondaryWeaponClass, OffHandWeapon1SocketName);
			// Offhand weapon should start in offhand state.
			OffHandWeapon->SetWeaponState(EWeaponState::Offhand);
		}
	}
}

void ANexusCharacter::SpawnAndAttachWeapon(ANexusWeapon*& WeaponPointer, const TSubclassOf<ANexusWeapon>& WeaponClass, const FName& SocketName)
{
	// Set spawn collision handling override.
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Create an instance of the weapon class.
	WeaponPointer = GetWorld()->SpawnActor<ANexusWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, ActorSpawnParams);

	if (WeaponPointer)
	{
		WeaponPointer->SetOwningCharacter(this);

		AttachWeaponToSocket(WeaponPointer, SocketName);
	}
}

void ANexusCharacter::AttachWeaponToSocket(ANexusWeapon*& Weapon, const FName& SocketName)
{
	// Attach the weapon to the character's socket.
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}
