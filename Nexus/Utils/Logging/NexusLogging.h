// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "LogLevel.h"
#include "LogOutput.h"

/**
 * \brief Custom logging wrapper for Unreal.
 */
class FNexusLogging
{
public:

	FNexusLogging() = delete;

	/**
	 * \brief Prints a message to all the log outputs.
	 * \param LoggingLevel Severity of message. (affects color of log @see ELogLevel)
	 * \param Message The message to display.
	 */
	static void Log(ELogLevel LoggingLevel, FString Message);
	/**
	 * \brief Prints a message to the specified log outputs with a specific color
	 * \param LoggingLevel Severity of message. (affects color of log @see ELogLevel)
	 * \param Message The message to display.
	 * \param LogOutput Where the message should be logged. (All, Output Log or Screen)
	 */
	static void Log(ELogLevel LoggingLevel, FString Message, ELogOutput LogOutput);
};
