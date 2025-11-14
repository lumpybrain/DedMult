// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Commands/DMCommand.h"
#include "DMCommand_BuildShip.generated.h"

class ADMShip;
/**
 * Class used to initialize build ship commands
 *
 * Not exposed as a blueprint types; blueprints should use the DMCommand Blueprint Library
 *		to build this class
 */
UCLASS()
class MULTSTRAT_API UDMCommandInit_BuildShip : public UDMCommandInit
{
	GENERATED_BODY()

public:
	/** Ship type to make */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Override Ship Type"))
	TSubclassOf<ADMShip> ShipClass = nullptr;
};

/**
 * Command to build a ship on a planet; only works if the target is a planet
 */
UCLASS()
class MULTSTRAT_API UDMCommand_BuildShip : public UDMCommand
{
	GENERATED_BODY()

public:
	/** Constructor: set command flag */
	UDMCommand_BuildShip(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<ADMShip> GetShipClass() const { return ShipSpawnClass; }

	/**
	 * Class that will be spawned when this build ship command evaluates.
	 * Note: Be careful not to make it a invisible ship class, I.e ADMShip!
	 * If this is null, the default class as set in the game modes
	 * UCommandsDataAsset will be used
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ADMShip> ShipSpawnClass = nullptr;

	//~ Begin UDMCommand Interface

	/**
	 * Build the ship on the planet!
	 * returns true if command executes successfully
	 */
	virtual bool RunCommand_Implementation() override;

	/** When we register, tell our target planet that a ship is incoming! */
	virtual void CommandQueued_Implementation() override;

	/** When we unregister, tell our target planet no ship is coming anymore! */
	virtual void CommandUnqueued_Implementation() override;

	/**
	 * Initialize variables. InitVariables must be of class UDMCommandInit_BuildShip.
	 * returns whether init was successful. Init can be unsuccessful if
	 * the input is not proper (i.e owning player is a nullptr)
	 */
	virtual bool InitializeCommand_Implementation(UDMCommandInit* InitVariables) override;
	
	/**
	 * Make sure our planet still exists
	 * returns true if command can be run successfully
	 */
	virtual bool Validate_Implementation() const override;

	/** returns a string with the name of the command, what it does, and what it will operate on */
	virtual FString CommandDebug_Implementation() const override;

	/**
	 * Create a new UObject of this command's type
	 */
	virtual UDMCommand* CopyCommand(const struct FCommandPacket& Packet) override;

	/** Fill data for future use of CopyCommand calls */
	virtual void FillCopyCommandData(TArray<TObjectPtr<UObject>>& CommandData) override;

protected:

	/** Get data for CopyCommand calls */
	virtual void GetCopyCommandData(const TArray<TObjectPtr<UObject>>& CommandData) override;

	//~ End UDMCommand Interface
};
