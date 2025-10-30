// Copyright (c) 2025 William Pritz under MIT License


#include "Components/DMCommandFlagsComponent.h"

#include "Commands/DMCommand.h"					// UDMCommand
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands

/******************************************************************************
 * Constructor: Don't Tick!
******************************************************************************/
UDMActiveCommandsComponent::UDMActiveCommandsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

/******************************************************************************
 * Checks the TArray for a command active on this object and returns it.
 * returns nullptr if the command is not in use on the object
******************************************************************************/
UDMCommand* UDMActiveCommandsComponent::GetCommand(const UClass* pCommandClass) const
{
	if (pCommandClass == nullptr)
	{
		AActor* pOwner = GetOwner();
		UE_LOG(LogCommands, Warning, TEXT("UDMActiveCommandsComponent::GetCommand: Null class passed in for object %s"),
			IsValid(pOwner) ? *pOwner->GetName() : TEXT("INVALID OBJECT"))
		return nullptr;
	}

	for (UDMCommand* pCurrCommand : ActiveCommands)
	{
		if (pCurrCommand->GetClass() == pCommandClass)
		{
			return pCurrCommand;
		}
	}

	return nullptr;
}

/******************************************************************************
 * Attempts to add the command to our array of active commands
 * Will refuse to add a command if a command of the same class is
 *		already in the array
 * returns true if command added, false if command is rejected
 *		(Prints an error to LogCommands if false happens)
******************************************************************************/
bool UDMActiveCommandsComponent::RegisterCommand(UDMCommand* pCommand)
{
	if (pCommand == nullptr)
	{
		AActor* pOwner = GetOwner();
		UE_LOG(LogCommands, Warning, TEXT("UDMActiveCommandsComponent::AddCommand: Null class passed in for object %s"),
			IsValid(pOwner) ? *pOwner->GetName() : TEXT("INVALID OBJECT"))
		return false;
	}

	// if the command is a duplicate (Or a subclass/parent class!), don't add it
	const UClass* pAttemptedClass = pCommand->GetClass();
	for (UDMCommand* pCurrCommand : ActiveCommands)
	{
		UClass* pCurrClass = pCurrCommand->GetClass();
		if (pCurrClass->IsChildOf(pAttemptedClass) ||
			pAttemptedClass->IsChildOf(pCurrClass))
		{
			AActor* pOwner = GetOwner();
			UE_LOG(LogCommands, Error, TEXT("UDMActiveCommandsComponent::AddCommand: Object %s tried to register class %s, but its a duplicate class!"),
				IsValid(pOwner) ? *pOwner->GetName() : TEXT("INVALID OBJECT"),
				*pAttemptedClass->GetName())
			return false;
		}
	}

	// we cool
	ActiveCommands.AddTail(pCommand);
	return true;
}

/******************************************************************************
 * Attempts to remove the command from our array of active commands
 * returns true if command found and removed, false if command was
 *		not registered
******************************************************************************/
bool UDMActiveCommandsComponent::UnregisterCommand(const UDMCommand* pCommand)
{
	if (pCommand == nullptr)
	{
		AActor* pOwner = GetOwner();
		UE_LOG(LogCommands, Warning, TEXT("UDMActiveCommandsComponent::UnregisterCommand: Null command passed in for object %s"),
			IsValid(pOwner) ? *pOwner->GetName() : TEXT("INVALID OBJECT"))
		return false;
	}

	// find and remove the command
	auto pNode = ActiveCommands.GetHead();
	while (pNode != nullptr)
	{
		if (pNode->GetValue() == pCommand)
		{
			ActiveCommands.RemoveNode(pNode);
			return true;
		}

		pNode = pNode->GetNextNode();
	}

	return false;
}

/******************************************************************************
 * Add a flag to the component.
 * returns true if that flag already existed on the component, false otherwise
******************************************************************************/
bool UDMActiveCommandsComponent::AddCommandFlags(const ECommandFlags Flag)
{
	bool bFlagExisted = CheckForCommandFlags(Flag);
	ActiveFlags |= Flag;
	return bFlagExisted;
}

/******************************************************************************
 * Remove some list of flags from this component
 * returns true if that flag existed on the component, false otherwise
******************************************************************************/
bool UDMActiveCommandsComponent::RemoveCommandFlags(const ECommandFlags Flag)
{
	bool bFlagExisted = CheckForCommandFlags(Flag);
	ActiveFlags &= ~Flag;
	return bFlagExisted;
}
