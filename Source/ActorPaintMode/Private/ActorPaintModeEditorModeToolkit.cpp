// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorPaintModeEditorModeToolkit.h"
#include "ActorPaintModeEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"
#include "ActorPaintMode/Public/WidgetPainter.h"

#define LOCTEXT_NAMESPACE "ActorPaintModeEditorModeToolkit"

FActorPaintModeEditorModeToolkit::FActorPaintModeEditorModeToolkit()
{
}

void FActorPaintModeEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FActorPaintModeEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FActorPaintModeEditorModeToolkit::GetToolkitFName() const
{
	return FName("ActorPaintModeEditorMode");
}

FText FActorPaintModeEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "ActorPaintModeEditorMode Toolkit");
}

TSharedPtr<SWidget> FActorPaintModeEditorModeToolkit::GetInlineContent() const
{
	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		  .VAlign(VAlign_Top)
		  .Padding(5.f)
		[
			Painter.ToSharedRef()
		]
		+ SScrollBox::Slot()
		[
			ModeDetailsView.ToSharedRef()
		]
		+ SScrollBox::Slot()
		[
			DetailsView.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE
