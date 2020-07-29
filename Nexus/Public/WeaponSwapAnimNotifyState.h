// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WeaponSwapAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API UWeaponSwapAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
