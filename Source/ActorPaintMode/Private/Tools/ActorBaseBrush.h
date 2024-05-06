// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTools/BaseBrushTool.h"
#include "ActorBaseBrush.generated.h"

class UClickDragInputBehavior;

UCLASS()
class ACTORPAINTMODE_API UActorBaseBrushBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	const FToolTargetTypeRequirements& GetTargetRequirements() const;
};

UCLASS(config = BaseActorPaintMode)
class UActorBaseBrushProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	ACTORPAINTMODE_API UActorBaseBrushProperties();

	// Rule for surface allowed to paint
	UPROPERTY(Config, EditAnywhere, Category = PaintRule)
	bool bUseChannelCheck;
	UPROPERTY(Config, EditAnywhere, Category = PaintRule, meta=(EditCondition = "bUseChannelCheck", EditConditionHides))
	TArray<TEnumAsByte<ECollisionChannel>> PaintChannels;
	UPROPERTY(Config, EditAnywhere, Category = PaintRule, meta=(EditCondition = "bUseChannelCheck", EditConditionHides))
	TArray<TEnumAsByte<ECollisionChannel>> BlockingChannels;
	UPROPERTY(EditAnywhere, Category = PaintRule)
	TArray<TObjectPtr<AActor>> OnlyPaintOnActors;
	UPROPERTY(EditAnywhere, Category = PaintRule)
	TArray<TObjectPtr<AActor>> IgnorePaintOnActors;

	/** brush */
	UPROPERTY(Config, EditAnywhere, Category = Brush, meta = (DisplayName = "Radius", UIMin = "1.0", UIMax = "2000.0", ClampMin = "0.1", ClampMax = "5000.0"))
	float BrushRadius;
	UPROPERTY(Config, EditAnywhere, Category = Brush, AdvancedDisplay)
	FColor DefaultColor = FColor::Green;
	UPROPERTY(Config, EditAnywhere, Category = Brush, AdvancedDisplay)
	FColor PaintingColor = FColor::Yellow;
	UPROPERTY(Config, EditAnywhere, Category = Brush, AdvancedDisplay)
	FColor BlockingColor = FColor::Red;
};

/**
 * Base Editor Mode to override that provide by default brush functionality with all mouses actions
 */
UCLASS()
class ACTORPAINTMODE_API UActorBaseBrush : public UInteractiveTool, public IClickDragBehaviorTarget, public IHoverBehaviorTarget, public IMouseWheelBehaviorTarget
{
	GENERATED_BODY()

protected:
	static constexpr int MouseWheel_CtrlModifierID = 1;
	static constexpr int MouseWheel_AltModifierID = 2;

	bool bBrushVisible;
	bool bPaintAllowed;
	bool bPaintInput;
	bool isMouseWheelCtrlModifier;
	bool isMouseWheelAltModifier;
	FRay LastWorldRay;
	FCollisionObjectQueryParams CollisionParam;

	TWeakObjectPtr<UWorld> TargetWorld;
	UPROPERTY()
	FBrushStampData LastBrushStamp;
	UPROPERTY()
	TObjectPtr<UBrushStampIndicator> BrushStampIndicator;
	UPROPERTY()
	TObjectPtr<UActorBaseBrushProperties> BaseBrushProperties;

	virtual void CreateProperties();
	virtual bool CanBrushClick();
	virtual bool TryClick(const FHitResult& ClickInfo);
	virtual bool HitTest(const FRay& WorldRay, FHitResult& HitRes, bool& PaintAllowed) const;
	virtual void MakeCollisionParam();
	virtual UClickDragInputBehavior* MakeDragInput();
	void ChangeBrushRadius(const int Offset) const;

public:
	void SetWorld(UWorld* World)
	{
		TargetWorld = World;
	}

	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos, bool& PaintAllowed) const;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	virtual void OnUpdateModifierState(int ModifierID, bool bIsOn) override;

	/* Mouse Click override */
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& PressPos) override;

	/* Mouse Drag override */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnTerminateDragSequence() override;

	/* Mouse wheel override */
	virtual FInputRayHit ShouldRespondToMouseWheel(const FInputDeviceRay& CurrentPos) override;
	virtual void OnMouseWheelScrollUp(const FInputDeviceRay& CurrentPos) override;
	virtual void OnMouseWheelScrollDown(const FInputDeviceRay& CurrentPos) override;

	/* Mouse hover override */
	virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
	virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
	virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
	virtual void OnEndHover() override;
};
