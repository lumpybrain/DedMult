// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DMCommand.generated.h"

class ADMGalaxyNode;
class ADMPlayerState;
enum class ECommandFlags : uint8;

/**
 * Class to be filled for command initialization
 * Child commands with unique inputs can inherit from this to init with
 *		unique variables in InitializeCommand_Implementation
 * 
 * Not exposed as a blueprint types; blueprints should use the DMCommand Blueprint Library
 *		to build this class
 * 
 * Note: I wish this could be a struct, but we need to pass this by pointer
 *		for casting to work, and you can't pass structs by pointers through
 *		blueprint functions because blueprints rely on it being a UObject.
 *		Unreal's data assets inherit from UObject, so i'm assuming the "extra"
 *		cost of making it a UObject will be negligible
 */
UCLASS()
class MULTSTRAT_API UDMCommandInit : public UObject
{
	GENERATED_BODY()

public:
	/** Player requesting the command */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Requesting Player"))
	ADMPlayerState* pRequestingPlayer = nullptr;
	
	/** Target node of the command */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Target"))
	ADMGalaxyNode* pTarget = nullptr;
};

/**
 * Parent Class of all commands players used to change the current gamestate
 * Run only by Command Queue Subsystem.
 * 
 * DMTODO: 
 * 
 * Simulate Commands for local players testing strategies
 * 
 * Make friend class of CommandQueueSubsystem so protect RunCommand and other functions, 
 * or leave functions public for simulation later?
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class MULTSTRAT_API UDMCommand : public UObject
{
	GENERATED_BODY()
	
public:
	//~=============================================================================
	// Command Queue Subsystem Functions

	/**
	 * Command executes on its target; presumed to be only be run by CommandQueueSubsystem
	 * returns true if command executes successfully
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool RunCommand() const;
	virtual bool RunCommand_Implementation() const;

	/** Called when a command is queued in our local player state */
	UFUNCTION(BlueprintNativeEvent)
	void CommandQueued();
	virtual void CommandQueued_Implementation() {}

	/** Called when a command is unqueued in our local player state or after it runs */
	UFUNCTION(BlueprintNativeEvent)
	void CommandUnqueued();
	virtual void CommandUnqueued_Implementation() {}

	//~=============================================================================
	// Command Management Functions

	/**
	 * Command Initializes the command's target and owning player; those targets may have flags modified by the command.
	 * returns true if the input is valid
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (ToolTip = "Note; manual initialization is considered development only,\n Should you be making this through the blueprint library?"))
	bool InitializeCommand(UDMCommandInit* InitVariables);
	virtual bool InitializeCommand_Implementation(UDMCommandInit* InitVariables);

	/**
	 * Overrideable IsValid method; makes sure the command can still execute in the current gamestate
	 * returns true if command can be run successfully
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool Validate() const;
	virtual bool Validate_Implementation() const;

	/** returns a string with the name of the command, what it does, and what it will operate on */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FString CommandDebug() const;
	virtual FString CommandDebug_Implementation() const		{ return FString::Printf(TEXT("No Debug for class %s"), *GetClass()->GetFName().ToString()); }

	/**
	 * Create a new UObject of this command's class type and copy the data from command data into it
	 * Used when sending commands from the client to the server instead of replication so that we
	 *		don't have to wait for unreal's automatic replication to copy the data over for us;
	 *		not all clients need a copy of the command, just the server.
	 */
	virtual UDMCommand* CopyCommand(const struct FCommandPacket& Packet);

	/**
	 * Fill data for future use of CopyCommand calls
	 */
	virtual void FillCopyCommandData(TArray<TObjectPtr<UObject>> &CommandData);
	
	//~=============================================================================
	// Properties and Accessors

	UFUNCTION(BlueprintCallable)
	const ADMPlayerState* GetOwningPlayer() const			{ return pOwningPlayer; }

	UFUNCTION(BlueprintCallable)
	ECommandFlags GetCommandFlags() const					{ return CommandFlags;}

	/** higher priority commands run first.Set when registered to the command queue */
	uint8 Priority = 0;

protected:
	virtual void GetCopyCommandData(const TArray<TObjectPtr<UObject>>& CommandData);

	/** 
	 * Commands can set these flags on target objects/homebased objects when registered
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECommandFlags CommandFlags;

	/** Player that owns the command; mostly used to determine team */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Owning Player"))
	TObjectPtr<ADMPlayerState> pOwningPlayer;

	/** Galaxy node the command wants to interact with(i.e, spawning on a node, moving to a node) */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Target Node"))
	TObjectPtr<ADMGalaxyNode> pTargetNode;
};

USTRUCT()
struct MULTSTRAT_API FCommandPacket
{
	GENERATED_USTRUCT_BODY()

	FCommandPacket() {}
	FCommandPacket(const FCommandPacket& other) : CommandClass(other.CommandClass), Data(other.Data) {}

	void InitializePacket(UDMCommand* CommandForInfo);

	UPROPERTY()
	TSubclassOf<UDMCommand> CommandClass;

	UPROPERTY()
	TArray<TObjectPtr<UObject>> Data;
};
