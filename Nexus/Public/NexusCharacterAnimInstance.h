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
	bool bJumpEnabled;
	
	/**
	 * \brief Used to track if the character is in the air.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bInAir;
	
	/**
	 * \brief Used to track if the character is crouching.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bCrouching;
	
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
	bool bDead;

	/**
	 * \brief Used to track if the character is aiming down sights.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bAimingDownSights;

	/**
	 * \brief The pitch rotation angle of the character's aim.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AimPitch;

	/**
	 * \brief The yaw rotation angle of the character's aim.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AimYaw;

private:
	
	/**
	 * \brief Pointer to the pawn this anim instance is used by
	 */
	APawn* Owner;
};
