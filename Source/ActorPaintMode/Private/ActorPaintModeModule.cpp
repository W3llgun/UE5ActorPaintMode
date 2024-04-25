// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorPaintModeModule.h"
#include "ActorPaintModeEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "ActorPaintModeModule"

void FActorPaintModeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FActorPaintModeEditorModeCommands::Register();
}

void FActorPaintModeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FActorPaintModeEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FActorPaintModeModule, ActorPaintModeEditorMode)