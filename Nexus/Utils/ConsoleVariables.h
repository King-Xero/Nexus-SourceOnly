// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"

// Only compile if debugging
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

// Console variables used to toggle debugging features.

static TAutoConsoleVariable<bool> CVarDebugWeaponDrawing(
	TEXT("Nexus.DebugWeapons"),
	false,
	TEXT("Enable or Disable debug tracers for weapons. ")
	TEXT("false = off. ")
	TEXT("true = Draw debug tracers."),
	ECVF_Cheat);

static TAutoConsoleVariable<bool> CVarDebugScreenLoggingDrawing(
	TEXT("Nexus.ScreenLogging"),
	false,
	TEXT("Enable or Disable drawing log messages in screen. ")
	TEXT("false = off. ")
	TEXT("true = Draw log messages."),
	ECVF_Cheat);

#endif
