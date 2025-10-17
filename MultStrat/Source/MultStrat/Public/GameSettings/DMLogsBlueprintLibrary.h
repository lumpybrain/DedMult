// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DMLogsBlueprintLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);

/**
 * Blueprint library for printing to our various log categories
 */
UCLASS()
class MULTSTRAT_API UDMLogsBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// DMTODO: Unreal logging verbosity is... weird. Maybe look back into whether
	// verbosity can be a variable we pass in at runtime. Otherwise, for now,
	// making 3 functions per category is fine
	
	//~=============================================================================
	// LogCommands
	UFUNCTION(BlueprintCallable, Category = "Logging|Commands")
	static void Print_LogCommands_Display(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|Commands")
	static void Print_LogCommands_Warning(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|Commands")
	static void Print_LogCommands_Error(UPARAM(ref)FString& InputText);

	//~=============================================================================
	// LogTeams
	UFUNCTION(BlueprintCallable, Category = "Logging|Teams")
	static void Print_LogTeams_Display(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|Teams")
	static void Print_LogTeams_Warning(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|Teams")
	static void Print_LogTeams_Error(UPARAM(ref)FString& InputText);

	//~=============================================================================
	// LogGalaxy
	UFUNCTION(BlueprintCallable, Category = "Logging|Galaxy")
	static void Print_LogGalaxy_Display(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|Galaxy")
	static void Print_LogGalaxy_Warning(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|Galaxy")
	static void Print_LogGalaxy_Error(UPARAM(ref)FString& InputText);
	
	//~=============================================================================
	// LogUI
	UFUNCTION(BlueprintCallable, Category = "Logging|UI")
	static void Print_LogUI_Display(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|UI")
	static void Print_LogUI_Warning(UPARAM(ref)FString& InputText);
	UFUNCTION(BlueprintCallable, Category = "Logging|UI")
	static void Print_LogUI_Error(UPARAM(ref)FString& InputText);


	
};
