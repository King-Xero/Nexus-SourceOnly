// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;

UCLASS()
class NEXUS_API ANexusPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusPickupActor();

	/**
	 * \brief Event when this actor overlaps another actor, for example a player walking into a trigger.
	 * \param OtherActor The actor that was overlapped.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Collision sphere used to detect overlapping actors.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;

	/**
	 * \brief Collision sphere used to detect overlapping actors.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* DecalComponent;

};
