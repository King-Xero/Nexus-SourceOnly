#include "NexusLogging.h"
#include "Engine/Engine.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "Nexus/Utils/ConsoleVariables.h"
#endif

void FNexusLogging::Log(ELogLevel LoggingLevel, FString Message)
{
	Log(LoggingLevel, Message, ELogOutput::ALL);
}

void FNexusLogging::Log(ELogLevel LoggingLevel, FString Message, ELogOutput LogOutput)
{
	
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const bool bDrawScreenLogs = CVarDebugScreenLoggingDrawing.GetValueOnGameThread();
	if (bDrawScreenLogs)
	{
		// Only print when screen is selected and the GEngine object is available.
		if ((ELogOutput::ALL == LogOutput || ELogOutput::SCREEN == LogOutput) && GEngine)
		{
			// Default color
			FColor LogColor = FColor::Magenta;
			// Flip the color based on the type
			switch (LoggingLevel)
			{
			case ELogLevel::TRACE:
				LogColor = FColor::Green;
				break;
			case ELogLevel::DEBUG:
				LogColor = FColor::Magenta;
				break;
			case ELogLevel::INFO:
				LogColor = FColor::White;
				break;
			case ELogLevel::WARNING:
				LogColor = FColor::Yellow;
				break;
			case ELogLevel::ERROR:
				LogColor = FColor::Red;
				break;
			default:
				break;
			}
			
			// Print the message on screen.
			GEngine->AddOnScreenDebugMessage(-1, 4.5f, LogColor, Message);
		}
	}
#endif
	

	if (ELogOutput::ALL == LogOutput || ELogOutput::OUTPUT_LOG == LogOutput)
	{
		// Flip the message type based on error level.
		switch (LoggingLevel)
		{
		case ELogLevel::TRACE:
			UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::DEBUG:
			UE_LOG(LogTemp, Verbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::INFO:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		case ELogLevel::WARNING:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
			break;
		case ELogLevel::ERROR:
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
			break;
		default:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		}
	}
}