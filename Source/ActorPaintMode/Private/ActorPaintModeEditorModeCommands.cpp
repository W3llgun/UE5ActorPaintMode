// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorPaintModeEditorModeCommands.h"
#include "ActorPaintModeEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "ActorPaintModeEditorModeCommands"

FActorPaintModeEditorModeCommands::FActorPaintModeEditorModeCommands()
	: TCommands<FActorPaintModeEditorModeCommands>("ActorPaintModeEditorMode",
	                                               NSLOCTEXT("ActorPaintModeEditorMode", "ActorPaintModeEditorModeCommands", "ActorPaintMode Editor Mode"),
	                                               NAME_None,
	                                               FAppStyle::GetAppStyleSetName())
{
}

void FActorPaintModeEditorModeCommands::RegisterCommands()
{
	TArray<TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(BrushToolRaw, "Paint Actor Raw", "Spawn actors using a brush", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(BrushToolRaw);

	//UI_COMMAND(BrushTool, "Paint Actor", "Paint actor in level", EUserInterfaceActionType::ToggleButton, FInputChord());
	//ToolCommands.Add(BrushTool);

	UI_COMMAND(SimpleTool, "Actor Selector", "Select Actors using a brush", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FActorPaintModeEditorModeCommands::GetCommands()
{
	return FActorPaintModeEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
