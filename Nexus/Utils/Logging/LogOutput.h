// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

/**
 * \brief Used to declare the output method for log messages.
 */
enum class ELogOutput : uint8
{
	ALL				UMETA(DisplayName = "All outputs"),
	OUTPUT_LOG		UMETA(DisplayName = "Output log"),
	SCREEN			UMETA(DisplayName = "Screen")
};