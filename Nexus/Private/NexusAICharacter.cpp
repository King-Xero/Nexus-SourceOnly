// Toyan Green © 2020


#include "NexusAICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "NexusGameState.h"
#include "Kismet/GameplayStatics.h"

ANexusAICharacter::ANexusAICharacter()
	: ANexusCharacter()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	EasterEggMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EasterEggMeshComponent"));
	EasterEggMeshComponent->SetupAttachment(GetMesh(), HeadSocketName);
	EasterEggMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EasterEggMeshComponent->SetVisibility(false);
}

void ANexusAICharacter::StopShooting()
{
	Super::StopShooting();
	
	if (CurrentWeapon->GetAmmoInClip() <= 0)
	{
		StartReloading();
	}
}

float ANexusAICharacter::GetScoreValue() const
{
	return ScoreValue;
}

void ANexusAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// Randomize character appearance.

	// Character mesh
	const int RandomCharacterMeshIndex = FMath::RandRange(0, CharacterMeshes.Num() - 1);
	USkeletalMesh* CharacterMesh = CharacterMeshes[RandomCharacterMeshIndex];
	if (CharacterMesh)
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh);
		
		const int RandomCharacterMaterialIndex = FMath::RandRange(0, CharacterMaterials.Num() - 1);

		UMaterial* CharacterMaterial = CharacterMaterials[RandomCharacterMaterialIndex];

		if (CharacterMaterial)
		{
			GetMesh()->SetMaterial(0, CharacterMaterial);
		}
	}

	// Hair and Beard
	if (HairMaterials.Num())
	{
		const int RandomHairMaterialIndex = FMath::RandRange(0, HairMaterials.Num() - 1);
		UMaterialInstance* HairMaterial = HairMaterials[RandomHairMaterialIndex];

		// If hair meshes are assigned then hair can be randomized.
		if (HairMeshes.Num())
		{
			const int RandomCharacterHairMeshIndex = FMath::RandRange(0, HairMeshes.Num() - 1);
			UStaticMesh* HairMesh = HairMeshes[RandomCharacterHairMeshIndex];
			if (HairMesh)
			{
				HairMeshComponent->SetStaticMesh(HairMesh);
				if (HairMaterial)
				{
					HairMeshComponent->SetMaterial(0, HairMaterial);
				}
			}
		}
		// If beard meshes are assigned then facial hair can be randomized.
		if (BeardMeshes.Num())
		{
			const int RandomCharacterBeardMeshIndex = FMath::RandRange(0, BeardMeshes.Num() - 1);
			UStaticMesh* BeardMesh = BeardMeshes[RandomCharacterBeardMeshIndex];
			if (BeardMesh)
			{
				BeardMeshComponent->SetStaticMesh(BeardMesh);
				if (HairMaterial)
				{
					BeardMeshComponent->SetMaterial(0, HairMaterial);
				}
			}
		}
	}

	// Mask/helmet mesh
	if (MaskMeshes.Num())
	{
		const int RandomMaskMeshIndex = FMath::RandRange(0, MaskMeshes.Num() - 1);
		UStaticMesh* MaskMesh = MaskMeshes[RandomCharacterMeshIndex];
		if (MaskMesh)
		{
			MaskMeshComponent->SetStaticMesh(MaskMesh);

			const int RandomMaskMaterialIndex = FMath::RandRange(0, MaskMeshes.Num() - 1);

			UMaterialInstance* MaskMaterial = MaskMaterials[RandomMaskMaterialIndex];

			if (MaskMaterial)
			{
				GetMesh()->SetMaterial(0, MaskMaterial);
			}
		}
	}

	ANexusGameState* GameState = GetWorld()->GetGameState<ANexusGameState>();
	if (GameState)
	{
		EasterEggMeshComponent->SetVisibility(GameState->IsEasterEggActive());
	}
}

void ANexusAICharacter::PlayHurtSFX() const
{
	bool bEasterEggActive = false;
	
	ANexusGameState* GameState = GetWorld()->GetGameState<ANexusGameState>();
	if(GameState)
	{
		bEasterEggActive = GameState->IsEasterEggActive();
	}

	if (bEasterEggActive && EasterEggHurtSFX)
	{
		UGameplayStatics::SpawnSoundAttached(EasterEggHurtSFX, GetMesh());
	}
	else
	{
		Super::PlayHurtSFX();
	}
}

void ANexusAICharacter::PlayDeathSFX() const
{
	bool bEasterEggActive = false;

	ANexusGameState* GameState = GetWorld()->GetGameState<ANexusGameState>();
	if (GameState)
	{
		bEasterEggActive = GameState->IsEasterEggActive();
	}

	if (bEasterEggActive && EasterEggDeathSFX)
	{
		UGameplayStatics::SpawnSoundAttached(EasterEggDeathSFX, GetMesh());
	}
	else
	{
		Super::PlayDeathSFX();
	}
}
