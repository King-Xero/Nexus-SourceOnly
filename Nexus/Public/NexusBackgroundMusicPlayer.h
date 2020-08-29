// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusBackgroundMusicPlayer.generated.h"

class USoundCue;

UCLASS()
class NEXUS_API ANexusBackgroundMusicPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusBackgroundMusicPlayer();

	/**
	 * \brief Play a random track from MusicTrackList.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music Player")
	void PlayRandomTrackFromList();

	/**
	 * \brief Use this music player to play a given track.
	 * \param TrackToPlay 
	 */
	UFUNCTION(BlueprintCallable, Category = "Music Player")
	void PlayTrack(USoundCue* TrackToPlay);

	/**
	 * \brief Stop any currently playing music.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music Player")
	void StopMusic();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Used to play audio from the power.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent;

	/**
	 * \brief Fade in duration (in seconds) when playing music.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Music Player")
	float FadeInTime;

	/**
	 * \brief Fade out duration (in seconds) when stopping music.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Music Player")
	float FadeOutTime;
	
	/**
	 * \brief Music tracks to be played.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Music Player")
	TArray<USoundCue*> MusicTrackList;
};
