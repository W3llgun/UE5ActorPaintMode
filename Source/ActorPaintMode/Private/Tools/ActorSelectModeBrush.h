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
 * Settings UObject for UActorSelectModeBrush. This UClass inherits from UActorBaseBrushProperties,
 * which provides an OnModified delegate that the Tool will listen to for changes in property values and
 * contain all default properties for a brush
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
 * UActorSelectModeBrush give a brush that can be used to select or unselect all objets under the brush
 */
UCLASS()
class ACTORPAINTMODE_API UActorSelectModeBrush : public UActorBaseBrush
{
	GENERATED_BODY()

public:
	UActorSelectModeBrush();
	virtual void Setup() override;

protected:
	// Keyboard Modifier used to modify mouse action
	static constexpr int Mouse_ShiftModifierID = 3;

	UPROPERTY()
	TObjectPtr<UActorPaintModeSimpleToolProperties> SelectProperties;
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	// Last point where selection triggered
	FVector LastPoint;
	// Current collision check options
	FCollisionObjectQueryParams SelectCollision;
	// Current Selection
	TArray<AActor*> Selection;
	// Selection mode
	bool bIsUnselectMode;

	/**
	 * Setup Drag behavior. Add modifier input to it.
	 * @return the created drag behavior
	 */
	virtual UClickDragInputBehavior* MakeDragInput() override;

	/**
	 * Create this tool Properties class
	 */
	virtual void CreateProperties() override;

	/**
	 * Check if selection can trigger based on distance from last trigger
	 * @param Point World position to check against LastPoint
	 * @return Is the distance far enough depending on brush size
	 */
	bool DistanceCheck(const FVector& Point) const;

	/**
	 * Get All overlaped actors from position
	 * @param Position World position where get happen
	 * @param ActorList Return found actors
	 * @return true if anything has been found
	 */
	bool GetOverlapActors(const FVector& Position, TArray<AActor*>& ActorList) const;

	/**
	 * Is the passed actor selectable depending on current selection settings
	 * @param Actor Object to check
	 * @return true if it is possible to selected it
	 */
	bool CanBeSelected(AActor* Actor) const;

	/**
	 * Push actor list to the UE selection component
	 * @param Actors Actors to add
	 */
	void SelectActors(TArray<AActor*> Actors);
	/**
	 * Remove actor list from the UE selection component
	 * @param Actors Actors to remove
	 */
	void UnselectActors(TArray<AActor*> Actors);

	/**
	 * Do all test possible and proceed to trigger click if possible.
	 * @param ClickInfo Information to try click
	 */
	virtual bool TryClick(const FHitResult& ClickInfo) override;

	/**
	 * Called when click is released
	 * @param ReleasePos Release information
	 */
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;

	/**
	 * Change active modifiers
	 * @param ModifierID unique modifier id
	 * @param bIsOn On/Off modifier
	 */
	virtual void OnUpdateModifierState(int ModifierID, bool bIsOn) override;
};
