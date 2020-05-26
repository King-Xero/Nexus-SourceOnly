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

	/**
	 * \brief Used to track if the character jumped.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool JumpEnabled;
	
	/**
	 * \brief Used to track if the character is in the air.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsInAir;
	
	/**
	 * \brief Used to track if the character is crouching.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsCrouching;
	
	/**
	 * \brief The current speed of the character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed;
	
	/**
	 * \brief The current direction of the character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction;

	/**
	 * \brief Used to track if the character is dead.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsDead;

private:
	
	/**
	 * \brief Pointer to the pawn this anim instance is used by
	 */
	APawn* Owner;
};