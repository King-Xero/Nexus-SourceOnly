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
	
	ANexusAICharacter();

protected:

	virtual void BeginPlay() override;
	
	/**
	 * \brief Component used to give AI senses. (Sight etc.)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionComponent* PerceptionComponent;

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
};
