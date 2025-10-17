// Copyright (c) 2025 William Pritz under MIT License


#include "GameSettings/DMLogsBlueprintLibrary.h"

#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMTeamComponent.h"			// LogTeams
#include "GalaxyObjects/DMGalaxyNode.h"			// LogGalaxy


DEFINE_LOG_CATEGORY(LogUI);

/*/////////////////////////////////////////////////////////////////////////////
*	LogCommands ///////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

void UDMLogsBlueprintLibrary::Print_LogCommands_Display(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogCommands, Display, TEXT("Blueprint Log: %s"), *InputText)
}

void UDMLogsBlueprintLibrary::Print_LogCommands_Warning(UPARAM(ref)FString & InputText)
{
	UE_LOG(LogCommands, Warning, TEXT("Blueprint Log: %s"), *InputText)
}

void UDMLogsBlueprintLibrary::Print_LogCommands_Error(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogCommands, Error, TEXT("Blueprint Log: %s"), *InputText)
}

/*/////////////////////////////////////////////////////////////////////////////
*	LogTeams //////////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////
void UDMLogsBlueprintLibrary::Print_LogTeams_Display(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogTeams, Display, TEXT("Blueprint Log: %s"), *InputText)
}

void UDMLogsBlueprintLibrary::Print_LogTeams_Warning(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogTeams, Warning, TEXT("Blueprint Log: %s"), *InputText)
}

void UDMLogsBlueprintLibrary::Print_LogTeams_Error(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogTeams, Error, TEXT("Blueprint Log: %s"), *InputText)
}

/*/////////////////////////////////////////////////////////////////////////////
*	LogGalaxy /////////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////
void UDMLogsBlueprintLibrary::Print_LogGalaxy_Display(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogGalaxy, Display, TEXT("Blueprint Log: %s"), *InputText)
}
void UDMLogsBlueprintLibrary::Print_LogGalaxy_Warning(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogGalaxy, Warning, TEXT("Blueprint Log: %s"), *InputText)
}
void UDMLogsBlueprintLibrary::Print_LogGalaxy_Error(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogGalaxy, Error, TEXT("Blueprint Log: %s"), *InputText)
}

/*/////////////////////////////////////////////////////////////////////////////
*	LogUI /////////////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////
void UDMLogsBlueprintLibrary::Print_LogUI_Display(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogUI, Display, TEXT("Blueprint Log: %s"), *InputText)
}

void UDMLogsBlueprintLibrary::Print_LogUI_Warning(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogUI, Warning, TEXT("Blueprint Log: %s"), *InputText)
}

void UDMLogsBlueprintLibrary::Print_LogUI_Error(UPARAM(ref)FString& InputText)
{
	UE_LOG(LogUI, Error, TEXT("Blueprint Log: %s"), *InputText)
}