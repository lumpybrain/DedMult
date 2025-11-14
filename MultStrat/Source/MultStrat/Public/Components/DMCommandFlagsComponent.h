// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMCommandFlagsComponent.generated.h"

class UDMCommand;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandRegistered, const UDMCommand*, NewCommand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandUnregistered, const UDMCommand*, OldCommand);


// DMTODO: I should probably use Actor tags instead
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECommandFlags : uint8
{
	None = 0,
	Resolved = 1 << 1,
	HasShip = 1 << 2,
	MovingShip = 1 << 3
};
ENUM_CLASS_FLAGS(ECommandFlags);

/**
 * Get commands acting on an object i.e for cancellation
 */
UCLASS(Blueprintable)
class MULTSTRAT_API UDMActiveCommandsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Constructor : don't tick! */
	UDMActiveCommandsComponent();

	//~=============================================================================
	// Command Registration and Unregistration

	/** Events for blueprints to use when our active commmands have changed. */
	UPROPERTY(BlueprintAssignable)
	FCommandRegistered OnCommandRegistered;
	UPROPERTY(BlueprintAssignable)
	FCommandUnregistered OnCommandUnregistered;

	/** 
	 * Attempts to add the command to our array of active commands
	 * Will refuse to add a command if a command of the same class is
	 *		already in the array
	 * returns true if command added, false if command is rejected
	 *		(Prints an error to LogCommands if false happens)
	 */
	UFUNCTION(BlueprintCallable)
	bool RegisterCommand(UDMCommand* Command);

	/** 
	 * Attempts to remove the command from our array of active commands
	 * returns true if command found and removed, false if command was
	 *		not registered
	 */
	UFUNCTION(BlueprintCallable)
	bool UnregisterCommand(const UDMCommand* Command);

	/** 
	 * Add flags to the component.
	 * returns true if that flag already existed on the component, false otherwise
	 */
	UFUNCTION(BlueprintCallable)
	bool AddCommandFlags(const ECommandFlags Flag);

	/**
	 * Remove some list of flags from this component
	 * returns true if that flag existed on the component, false otherwise
	 */
	UFUNCTION(BlueprintCallable)
	bool RemoveCommandFlags(const ECommandFlags Flag);

	/** Called on the server to clear flags and commands used to process the turn */
	// DMTODO Ufunction server
	void Reset();

	//~=============================================================================
	// Gettors
	/** 
	 * Checks the TArray for a command active on this object and returns it.
	 * Turning on include subclasses will also return the first found subclassed command
	 * returns nullptr if the given command class/subclasses are not registered
	 */
	UFUNCTION(BlueprintCallable)
	UDMCommand* GetCommand(const UClass* CommandClass, bool IncludeSubclasses = false);

	/** Check for some active flag on this component */
	UFUNCTION(BlueprintCallable)
	bool CheckForCommandFlags(ECommandFlags Flag) const { return (ActiveFlags & Flag) != ECommandFlags::None; }

protected:

	/** Flags are only used locally to verify command correctness, so we don't replicate them */
	ECommandFlags ActiveFlags = ECommandFlags::None;

	/** 
	 * Storage for all the commands registed on this object 
	 * Realistically, the size of this should never be more then ~4 at MAX,
	 * so we don't worry about an efficient finder data struct like TMap
	 */
	TDoubleLinkedList<TObjectPtr<UDMCommand>> ActiveCommands;

};
