// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NexusCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API UNexusCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UNexusCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool JumpEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsInAir;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsCrouching;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction;

private:
	
	/**
	 * \brief Pointer to the pawn this anim instance is used by
	 */
	APawn* Owner;
};
