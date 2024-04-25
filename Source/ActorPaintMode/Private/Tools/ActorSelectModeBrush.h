// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorBaseBrush.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "ActorSelectModeBrush.generated.h"

class UClickDragInputBehavior;
/**
 * Builder for UActorPaintModeSimpleTool
 */
UCLASS()
class ACTORPAINTMODE_API UActorPaintModeSimpleToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Settings UObject for UActorPaintModeSimpleTool. This UClass inherits from UInteractiveToolPropertySet,
 * which provides an OnModified delegate that the Tool will listen to for changes in property values.
 */
UCLASS(Transient)
class ACTORPAINTMODE_API UActorPaintModeSimpleToolProperties : public UActorBaseBrushProperties
{
	GENERATED_BODY()

public:
	UActorPaintModeSimpleToolProperties();

	UPROPERTY(Config, EditAnywhere, Category = SelectionRules, meta = (DisplayName = "Object Type", Bitmask, BitmaskEnum = "/Script/Engine.ECollisionChannel"))
	int32 CollisionObjects;
	UPROPERTY(Config, EditAnywhere, Category = SelectionRules, meta = (DisplayName = "Collisions"))
	TArray<TEnumAsByte<ECollisionChannel>> Collisions;
};


/**
 * UActorPaintModeSimpleTool is an example Tool that opens a message box displaying info about an actor that the user
 * clicks left mouse button. All the action is in the ::OnClicked handler.
 */
UCLASS()
class ACTORPAINTMODE_API UActorSelectModeBrush : public UActorBaseBrush
{
	GENERATED_BODY()

public:
	UActorSelectModeBrush();


	virtual void Setup() override;

protected:
	static constexpr int Mouse_ShiftModifierID = 3;
	UPROPERTY()
	TObjectPtr<UActorPaintModeSimpleToolProperties> SelectProperties;
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;
	FVector LastPoint;
	FCollisionObjectQueryParams SelectCollision;
	TArray<AActor*> Selection;
	bool bIsUnselectMode;

	bool DistanceCheck(const FVector& Point) const;
	bool GetOverlapActors(const FVector& Position, TArray<AActor*>& ActorList) const;
	bool CanBeSelected(AActor* Actor) const;
	void SelectActors(TArray<AActor*> Actors);
	void UnselectActors(TArray<AActor*> Actors);

	virtual void CreateProperties() override;
	virtual void TryClick(const FHitResult& ClickInfo) override;

	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
	virtual void OnUpdateModifierState(int ModifierID, bool bIsOn) override;
	virtual UClickDragInputBehavior* MakeDragInput() override;
};
