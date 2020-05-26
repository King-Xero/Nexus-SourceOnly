// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

/**
 * \brief Used to declare the severity log message.
 */
enum class ELogLevel : uint8
{
	TRACE			UMETA(DisplayName = "Trace"),
	DEBUG			UMETA(DisplayName = "Debug"),
	INFO			UMETA(DisplayName = "Info"),
	WARNING			UMETA(DisplayName = "Warning"),
	ERROR			UMETA(DisplayName = "Error")
};