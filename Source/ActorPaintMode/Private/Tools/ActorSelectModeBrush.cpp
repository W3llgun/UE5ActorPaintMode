// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorSelectModeBrush.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "Engine/World.h"
#include "Logging/StructuredLog.h"
#include "Subsystems/EditorActorSubsystem.h"

// localization namespace
#define LOCTEXT_NAMESPACE "ActorPaintModeSimpleTool"

/*
 * ToolBuilder implementation
 */

UInteractiveTool* UActorPaintModeSimpleToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UActorSelectModeBrush* NewTool = NewObject<UActorSelectModeBrush>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}


/*
 * ToolProperties implementation
 */

UActorPaintModeSimpleToolProperties::UActorPaintModeSimpleToolProperties()
{
}


/*
 * Tool implementation
 */

UActorSelectModeBrush::UActorSelectModeBrush()
{
}


void UActorSelectModeBrush::Setup()
{
	if (EditorActorSubsystem == nullptr)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	Super::Setup();
}

bool UActorSelectModeBrush::DistanceCheck(const FVector& Point) const
{
	if (LastPoint.IsZero()) return true;
	return FVector::Distance(LastPoint, Point) > SelectProperties->BrushRadius * 2;
}

bool UActorSelectModeBrush::GetOverlapActors(const FVector& Position, TArray<AActor*>& ActorList) const
{
	TArray<FOverlapResult> Overlaps;
	auto VCol = FCollisionObjectQueryParams();
	for (auto col : SelectProperties->Collisions)
	{
		VCol.AddObjectTypesToQuery(col);
	}
	if (TargetWorld->OverlapMultiByObjectType(Overlaps, Position, FQuat::Identity, VCol, FCollisionShape::MakeSphere(SelectProperties->BrushRadius)))
	{
		for (auto Overlap : Overlaps)
		{
			if (CanBeSelected(Overlap.GetActor())) ActorList.AddUnique(Overlap.GetActor());
		}
		return true;
	}
	return false;
}

bool UActorSelectModeBrush::CanBeSelected(AActor* Actor) const
{
	return true; // TODO
}

void UActorSelectModeBrush::SelectActors(TArray<AActor*> Actors)
{
	GEditor->BeginTransaction(FText::FromString("Select Actors"));
	for (const auto OverlapActor : Actors)
	{
		Selection.AddUnique(OverlapActor);
	}

	EditorActorSubsystem->SetSelectedLevelActors(Selection);
	GEditor->EndTransaction();
}

void UActorSelectModeBrush::UnselectActors(TArray<AActor*> Actors)
{
	GEditor->BeginTransaction(FText::FromString("Unselect Actors"));
	for (const auto OverlapActor : Actors)
	{
		Selection.Remove(OverlapActor);
	}

	EditorActorSubsystem->SetSelectedLevelActors(Selection);
	GEditor->EndTransaction();
}

void UActorSelectModeBrush::CreateProperties()
{
	SelectProperties = NewObject<UActorPaintModeSimpleToolProperties>(this);
	BaseBrushProperties = SelectProperties;
	AddToolPropertySource(SelectProperties);
}

void UActorSelectModeBrush::TryClick(const FHitResult& ClickInfo)
{
	if (!DistanceCheck(ClickInfo.Location)) return;
	Selection = EditorActorSubsystem->GetSelectedLevelActors();

	TArray<AActor*> OverlapActors;
	if (GetOverlapActors(ClickInfo.Location, OverlapActors))
	{
		if (bIsUnselectMode)
		{
			UnselectActors(OverlapActors);
		}
		else
		{
			SelectActors(OverlapActors);
		}
	}
}

void UActorSelectModeBrush::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
	Super::OnClickRelease(ReleasePos);
	LastPoint = FVector();
}

void UActorSelectModeBrush::OnUpdateModifierState(int ModifierID, bool bIsOn)
{
	Super::OnUpdateModifierState(ModifierID, bIsOn);
	if (ModifierID == Mouse_ShiftModifierID)
	{
		bIsUnselectMode = bIsOn;
	}
}

UClickDragInputBehavior* UActorSelectModeBrush::MakeDragInput()
{
	const auto Drag = Super::MakeDragInput();
	Drag->Modifiers.RegisterModifier(Mouse_ShiftModifierID, FInputDeviceState::IsShiftKeyDown);
	return Drag;
}


#undef LOCTEXT_NAMESPACE
