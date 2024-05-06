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


UClickDragInputBehavior* UActorSelectModeBrush::MakeDragInput()
{
	const auto Drag = Super::MakeDragInput();

	// Add modifier to the drag behavior
	Drag->Modifiers.RegisterModifier(Mouse_ShiftModifierID, FInputDeviceState::IsShiftKeyDown);
	return Drag;
}

void UActorSelectModeBrush::CreateProperties()
{
	SelectProperties = NewObject<UActorPaintModeSimpleToolProperties>(this);
	BaseBrushProperties = SelectProperties;
	AddToolPropertySource(SelectProperties);
}

bool UActorSelectModeBrush::DistanceCheck(const FVector& Point) const
{
	if (LastPoint.IsZero()) return true;
	return FVector::Distance(LastPoint, Point) > SelectProperties->BrushRadius * 2;
}

bool UActorSelectModeBrush::GetOverlapActors(const FVector& Position, TArray<AActor*>& ActorList) const
{
	auto LocalCollisionParams = FCollisionObjectQueryParams();
	for (auto col : SelectProperties->Collisions)
	{
		LocalCollisionParams.AddObjectTypesToQuery(col);
	}

	// World overlap check to find selectable actors.
	TArray<FOverlapResult> Overlaps;
	if (TargetWorld->OverlapMultiByObjectType(Overlaps, Position, FQuat::Identity, LocalCollisionParams, FCollisionShape::MakeSphere(SelectProperties->BrushRadius)))
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

bool UActorSelectModeBrush::TryClick(const FHitResult& ClickInfo)
{
	if (!DistanceCheck(ClickInfo.Location)) return false; // new click not far enough from last


	/*  Get current UE selected objects (may have changed because user used shift-click on some actors or our tool was just opened)
	 *  Need optimisation ?
	 */
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

	// eat click even if nothing was found
	return true;
}


void UActorSelectModeBrush::SelectActors(TArray<AActor*> Actors)
{
	for (const auto OverlapActor : Actors)
	{
		Selection.AddUnique(OverlapActor);
	}

	// Transaction to add majors actions to UE history (that enable ctrl-z modifications)
	GEditor->BeginTransaction(FText::FromString("Select Actors"));
	EditorActorSubsystem->SetSelectedLevelActors(Selection);
	GEditor->EndTransaction();
}

void UActorSelectModeBrush::UnselectActors(TArray<AActor*> Actors)
{
	for (const auto OverlapActor : Actors)
	{
		Selection.Remove(OverlapActor);
	}

	// Transaction to add majors actions to UE history (that enable ctrl-z modifications)
	GEditor->BeginTransaction(FText::FromString("Unselect Actors"));
	EditorActorSubsystem->SetSelectedLevelActors(Selection);
	GEditor->EndTransaction();
}

void UActorSelectModeBrush::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
	Super::OnClickRelease(ReleasePos);

	/*
	 *  On release drag, reset LastPoint.
	 *	But will prevent click on close to world pos (0,0,0).
	 *	To improve.
	 */
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


#undef LOCTEXT_NAMESPACE
