// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "NexusCharacter.h"
#include "NexusAICharacter.generated.h"

class UAIPerceptionComponent;

UCLASS()
class NEXUS_API ANexusAICharacter : public ANexusCharacter
{
	GENERATED_BODY()

public:
	ANexusAICharacter();

	virtual void StopShooting() override;

	/**
	 * \brief Get the score value for killing this enemy.
	 * \return Score value.
	 */
	float GetScoreValue() const;
	
protected:

	virtual void BeginPlay() override;
	
	/**
	 * \brief Component used to give AI senses. (Sight etc.)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionComponent* PerceptionComponent;

	/**
	 * \brief Visible mesh component for a easter egg.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EasterEggMeshComponent;

	/**
	 * \brief The amount of points scored when this enemy is killed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	float ScoreValue;
	
	/**
	 * \brief Meshes that can be used for this character.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<USkeletalMesh*> CharacterMeshes;

	/**
	 * \brief Materials that can be used for this character's mesh.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<UMaterial*> CharacterMaterials;

	/**
	 * \brief Meshes that can be used for this character's hair.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<UStaticMesh*> HairMeshes;

	/**
	 * \brief Meshes that can be used for this character's beard.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<UStaticMesh*> BeardMeshes;

	/**
	 * \brief Materials that can be used for this character's hair and beard.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<UMaterialInstance*> HairMaterials;

	/**
	 * \brief Meshes that can be used for this character's mask/helmet.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<UStaticMesh*> MaskMeshes;

	/**
	 * \brief Materials that can be used for this character's mask/helmet.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TArray<UMaterialInstance*> MaskMaterials;

	/**
	 * \brief Sound effect spawned at the character's location when they are hurt and the easter egg is active.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* EasterEggHurtSFX;
	
	/**
	 * \brief Sound effect spawned at the character's location when they die and the easter egg is active.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* EasterEggDeathSFX;

private:
	
	virtual void PlayHurtSFX() const override;
	virtual void PlayDeathSFX() const override;
};
