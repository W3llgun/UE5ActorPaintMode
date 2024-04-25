// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorBaseBrush.h"

#include "ActorPaintModeBrushRaw.h"
#include "InteractiveGizmoManager.h"
#include "InteractiveToolManager.h"
#include "ToolTargetManager.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseGizmos/BrushStampIndicator.h"

const FString BaseBrushIndicatorGizmoType = TEXT("BaseBrushIndicatorGizmoType");

UInteractiveTool* UActorBaseBrushBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UActorBaseBrush* NewTool = NewObject<UActorBaseBrush>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

const FToolTargetTypeRequirements& UActorBaseBrushBuilder::GetTargetRequirements() const
{
	static FToolTargetTypeRequirements TypeRequirements(AActor::StaticClass());
	return TypeRequirements;
}

UActorBaseBrushProperties::UActorBaseBrushProperties()
{
}

void UActorBaseBrush::CreateProperties()
{
	if (!BaseBrushProperties) BaseBrushProperties = NewObject<UActorBaseBrushProperties>(this, UActorBaseBrushProperties::StaticClass(), TEXT("Brush"));
	AddToolPropertySource(BaseBrushProperties);
}

bool UActorBaseBrush::CanBrushClick()
{
	return true;
}

void UActorBaseBrush::TryClick(const FHitResult& ClickInfo)
{
}

void UActorBaseBrush::Setup()
{
	Super::Setup();

	if (UClickDragInputBehavior* MouseDragBehavior = MakeDragInput())
	{
		AddInputBehavior(MouseDragBehavior);
	}

	UMouseHoverBehavior* HoverBehavior = NewObject<UMouseHoverBehavior>();
	HoverBehavior->Initialize(this);
	AddInputBehavior(HoverBehavior);

	UBrushMouseWheelBehavior* WheelBehavior = NewObject<UBrushMouseWheelBehavior>();
	WheelBehavior->Modifiers.RegisterModifier(MouseWheel_CtrlModifierID, FInputDeviceState::IsCtrlKeyDown);
	WheelBehavior->Modifiers.RegisterModifier(MouseWheel_AltModifierID, FInputDeviceState::IsAltKeyDown);
	WheelBehavior->Initialize(this);
	AddInputBehavior(WheelBehavior);

	if (!BrushStampIndicator)
	{
		// register and spawn brush indicator gizmo
		GetToolManager()->GetPairedGizmoManager()->RegisterGizmoType(BaseBrushIndicatorGizmoType, NewObject<UBrushStampIndicatorBuilder>());
		BrushStampIndicator = GetToolManager()->GetPairedGizmoManager()->CreateGizmo<UBrushStampIndicator>(BaseBrushIndicatorGizmoType, FString(), this);
	}
	CreateProperties();

	MakeCollisionParam();
}

UClickDragInputBehavior* UActorBaseBrush::MakeDragInput()
{
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	MouseBehavior->Initialize(this);
	return MouseBehavior;
}

void UActorBaseBrush::Shutdown(EToolShutdownType ShutdownType)
{
	BaseBrushProperties->SaveConfig();
	if (BrushStampIndicator)
	{
		GetToolManager()->GetPairedGizmoManager()->DestroyGizmo(BrushStampIndicator);
		BrushStampIndicator = nullptr;
		GetToolManager()->GetPairedGizmoManager()->DeregisterGizmoType(BaseBrushIndicatorGizmoType);
	}
	Super::Shutdown(ShutdownType);
}

void UActorBaseBrush::Render(IToolsContextRenderAPI* RenderAPI)
{
	Super::Render(RenderAPI);
	if (BrushStampIndicator)
	{
		BrushStampIndicator->LineColor = bPaintAllowed ? (bPaintInput ? BaseBrushProperties->PaintingColor : BaseBrushProperties->DefaultColor) : BaseBrushProperties->BlockingColor;
		BrushStampIndicator->bVisible = bBrushVisible;
		BrushStampIndicator->Update(LastBrushStamp.Radius, LastBrushStamp.WorldPosition, LastBrushStamp.WorldNormal, LastBrushStamp.Falloff);
	}
}

FInputRayHit UActorBaseBrush::FindRayHit(const FRay& WorldRay, FVector& HitPos, bool& PaintAllowed) const
{
	// trace a ray into the World
	FHitResult Result;
	if (HitTest(WorldRay, Result, PaintAllowed))
	{
		HitPos = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}
	return FInputRayHit();
}

bool UActorBaseBrush::HitTest(const FRay& WorldRay, FHitResult& HitRes, bool& PaintAllowed) const
{
	//UE_LOGFMT(LogTemp, Warning, "Trace {v} ", BrushProperties->PaintOnChannel.Name);

	// trace a ray into the World
	if (TargetWorld->LineTraceSingleByObjectType(HitRes, WorldRay.Origin, WorldRay.PointAt(999999), CollisionParam))
	{
		PaintAllowed = true;
		const ECollisionChannel Channel = HitRes.Component->GetCollisionObjectType();
		for (auto Blocking : BaseBrushProperties->BlockingChannels)
		{
			if (Channel == Blocking)
			{
				PaintAllowed = false;
				return true;
			}
		}
		if (BaseBrushProperties->OnlyPaintOnActors.Num() > 0)
		{
			if (!BaseBrushProperties->OnlyPaintOnActors.Contains(HitRes.GetActor()))
			{
				PaintAllowed = false;
				return true;
			}
		}
		if (BaseBrushProperties->IgnorePaintOnActors.Num() > 0)
		{
			if (BaseBrushProperties->IgnorePaintOnActors.Contains(HitRes.GetActor()))
			{
				PaintAllowed = false;
			}
		}
		return true;
	}
	PaintAllowed = false;
	return false;
}


void UActorBaseBrush::OnUpdateModifierState(const int ModifierID, const bool bIsOn)
{
	if (ModifierID == MouseWheel_CtrlModifierID)
	{
		isMouseWheelCtrlModifier = bIsOn;
	}
	else if (ModifierID == MouseWheel_AltModifierID)
	{
		isMouseWheelAltModifier = bIsOn;
	}
}

void UActorBaseBrush::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	Super::OnPropertyModified(PropertySet, Property);
	if (PropertySet == BaseBrushProperties)
	{
		MakeCollisionParam();
	}
}

FInputRayHit UActorBaseBrush::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	FVector Temp;
	return FindRayHit(PressPos.WorldRay, Temp, bPaintAllowed);
}

void UActorBaseBrush::OnClickPress(const FInputDeviceRay& PressPos)
{
	FHitResult Result;
	bPaintInput = true;
	if (CanBrushClick() && HitTest(PressPos.WorldRay, Result, bPaintAllowed))
	{
		TryClick(Result);
	}
	bBrushVisible = true;
}

void UActorBaseBrush::OnClickRelease(const FInputDeviceRay& PressPos)
{
}

void UActorBaseBrush::OnClickDrag(const FInputDeviceRay& DragPos)
{
	FHitResult Result;
	if (HitTest(DragPos.WorldRay, Result, bPaintAllowed))
	{
		bBrushVisible = true;
		LastBrushStamp.WorldPosition = Result.ImpactPoint;
		LastBrushStamp.WorldNormal = Result.Normal;
		LastBrushStamp.HitResult = Result;
		TryClick(Result);
	}
}

void UActorBaseBrush::OnTerminateDragSequence()
{
}

FInputRayHit UActorBaseBrush::ShouldRespondToMouseWheel(const FInputDeviceRay& CurrentPos)
{
	FHitResult Hit;
	if ((isMouseWheelCtrlModifier || isMouseWheelAltModifier) && HitTest(CurrentPos.WorldRay, Hit, bPaintAllowed))
	{
		bBrushVisible = true;
		return FInputRayHit(Hit.Distance, Hit.Normal);
	}
	return FInputRayHit();
}

void UActorBaseBrush::OnMouseWheelScrollUp(const FInputDeviceRay& CurrentPos)
{
	if (isMouseWheelCtrlModifier)
	{
		ChangeBrushRadius(5);
	}
}

void UActorBaseBrush::OnMouseWheelScrollDown(const FInputDeviceRay& CurrentPos)
{
	if (isMouseWheelCtrlModifier)
	{
		ChangeBrushRadius(-5);
	}
}

FInputRayHit UActorBaseBrush::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
{
	LastWorldRay = PressPos.WorldRay;
	FHitResult OutHit;
	if (HitTest(PressPos.WorldRay, OutHit, bPaintAllowed))
	{
		return FInputRayHit(OutHit.Distance);
	}
	return FInputRayHit();
}

void UActorBaseBrush::OnBeginHover(const FInputDeviceRay& DevicePos)
{
	bBrushVisible = true;
}

bool UActorBaseBrush::OnUpdateHover(const FInputDeviceRay& DevicePos)
{
	FHitResult OutHit;
	if (HitTest(DevicePos.WorldRay, OutHit, bPaintAllowed))
	{
		LastBrushStamp.Radius = BaseBrushProperties->BrushRadius;
		LastBrushStamp.WorldPosition = OutHit.ImpactPoint;
		LastBrushStamp.WorldNormal = OutHit.Normal;
		LastBrushStamp.HitResult = OutHit;
		LastBrushStamp.Falloff = 0;
	}
	return true;
}

void UActorBaseBrush::OnEndHover()
{
	//bBrushVisible = false;
}

void UActorBaseBrush::MakeCollisionParam()
{
	if (!BaseBrushProperties->bUseChannelCheck || BaseBrushProperties->PaintChannels.Num() == 0)
	{
		CollisionParam = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects);
		return;
	}
	CollisionParam = FCollisionObjectQueryParams();
	for (auto Channel : BaseBrushProperties->PaintChannels)
	{
		CollisionParam.AddObjectTypesToQuery(Channel);
	}

	// Add blocking channel to the trace so we can Hit them and not pass through (they get ignored after)
	for (auto Channel : BaseBrushProperties->BlockingChannels)
	{
		CollisionParam.AddObjectTypesToQuery(Channel);
	}
}


void UActorBaseBrush::ChangeBrushRadius(const int Offset) const
{
	BaseBrushProperties->BrushRadius = FMath::Max(BaseBrushProperties->BrushRadius + Offset, 1);
}
