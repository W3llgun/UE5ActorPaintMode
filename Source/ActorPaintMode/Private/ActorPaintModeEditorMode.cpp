// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorPaintModeEditorMode.h"
#include "ActorPaintModeEditorModeToolkit.h"
#include "InteractiveToolManager.h"
#include "ActorPaintModeEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "WidgetPainter.h"
#include "Tools/ActorPaintModeBrushRaw.h"
#include "Tools/ActorSelectModeBrush.h"
#include "Tools/ActorPaintModeInteractiveTool.h"

// step 2: register a ToolBuilder in FActorPaintModeEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "ActorPaintModeEditorMode"

const FEditorModeID UActorPaintModeEditorMode::EM_ActorPaintModeEditorModeId = TEXT("EM_ActorPaintModeEditorMode");

FString UActorPaintModeEditorMode::SimpleToolName = TEXT("ActorPaintMode_ActorInfoTool");
FString UActorPaintModeEditorMode::InteractiveToolName = TEXT("ActorPaintMode_MeasureDistanceTool");
FString UActorPaintModeEditorMode::BrushRawToolName = TEXT("ActorPaintMode_Raw");
FString UActorPaintModeEditorMode::BrushToolName = TEXT("ActorPaintMode_baseBrush");


UActorPaintModeEditorMode::UActorPaintModeEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UActorPaintModeEditorMode::EM_ActorPaintModeEditorModeId,
	                       LOCTEXT("ModeName", "Actor Painter"),
	                       FSlateIcon(),
	                       true);
}


UActorPaintModeEditorMode::~UActorPaintModeEditorMode()
{
}


void UActorPaintModeEditorMode::ActorSelectionChangeNotify()
{
}

void UActorPaintModeEditorMode::Enter()
{
	Painter = SNew(SWidgetPainter).Message(TEXT("Hello lol"));
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FActorPaintModeEditorModeCommands& SampleToolCommands = FActorPaintModeEditorModeCommands::Get();

	const auto RawPainterBuilder = NewObject<UActorPaintModeBrushRawToolBuilder>(this);
	RawPainterBuilder->Painter = Painter;

	RegisterTool(SampleToolCommands.BrushToolRaw, BrushRawToolName, RawPainterBuilder);
	//RegisterTool(SampleToolCommands.BrushTool, BrushToolName, NewObject<UActorBaseBrushBuilder>(this));
	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UActorPaintModeSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UActorPaintModeInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UActorPaintModeEditorMode::CreateToolkit()
{
	const TSharedPtr<FActorPaintModeEditorModeToolkit> CreatedToolkit = MakeShareable(new FActorPaintModeEditorModeToolkit);

	CreatedToolkit->Painter = Painter;
	Toolkit = CreatedToolkit;
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UActorPaintModeEditorMode::GetModeCommands() const
{
	return FActorPaintModeEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
