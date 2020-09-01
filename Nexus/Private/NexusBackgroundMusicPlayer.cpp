// Toyan Green © 2020


#include "NexusBackgroundMusicPlayer.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ANexusBackgroundMusicPlayer::ANexusBackgroundMusicPlayer()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

void ANexusBackgroundMusicPlayer::PlayRandomTrackFromList()
{
	if (MusicTrackList.Num())
	{
		const int RandomMusicTrackIndex = FMath::RandRange(0, MusicTrackList.Num() - 1);
		USoundCue* TrackToPlay = MusicTrackList[RandomMusicTrackIndex];
		if (TrackToPlay)
		{
			PlayTrack(TrackToPlay);
		}
	}

}

void ANexusBackgroundMusicPlayer::PlayTrack(USoundCue* TrackToPlay)
{
	AudioComponent->SetSound(TrackToPlay);
	AudioComponent->FadeIn(FadeInTime, TrackToPlay->GetVolumeMultiplier());
}

void ANexusBackgroundMusicPlayer::StopMusic()
{
	AudioComponent->FadeOut(FadeOutTime, 0.0f);
}

void ANexusBackgroundMusicPlayer::FadeToLowVolume()
{
	AudioComponent->AdjustVolume(FadeOutTime, LowFadeVolume);
}

// Called when the game starts or when spawned
void ANexusBackgroundMusicPlayer::BeginPlay()
{
	Super::BeginPlay();	
}
