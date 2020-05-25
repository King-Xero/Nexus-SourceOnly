// Toyan Green © 2020

#pragma once

// Only compile if debugging
#include "CoreMinimal.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

static TAutoConsoleVariable<bool> CVarDebugWeaponDrawing(
	TEXT("Nexus.DebugWeapons"),
	false,
	TEXT("Enable or Disable debug tracers for weapons. ")
	TEXT("false = off. ")
	TEXT("true = Draw Debug Tracers."),
	ECVF_Cheat);

#endif
