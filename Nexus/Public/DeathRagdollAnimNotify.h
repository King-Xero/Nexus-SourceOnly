// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DeathRagdollAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API UDeathRagdollAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
