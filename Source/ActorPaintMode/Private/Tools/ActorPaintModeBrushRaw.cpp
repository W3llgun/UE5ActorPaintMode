// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPaintModeBrushRaw.h"
#include "InteractiveGizmoManager.h"
#include "InteractiveToolManager.h"
#include "ToolDataVisualizer.h"
#include "WidgetPainter.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "Logging/StructuredLog.h"

const FString BrushIndicatorGizmoType = TEXT("BrushIndicatorGizmoType");
#define LOCTEXT_NAMESPACE "UActorPaintModeBrushRaw"

UActorPaintModeBrushProperties::UActorPaintModeBrushProperties()
{
	BrushRadius = 100.0f;
	BrushDensity = 1;
	RoofHeight = 0;
	ProximityCheck = false;
	ProximityCheckRadius = BrushRadius;
	RandomRotation = 0;
	bUseChannelCheck = false;
	SpawnMode = ESpawnMode::Random;
}

void UActorPaintModeBrushProperties::Save()
{
	UE_LOGFMT(LogTemp, Warning, "Save");
	SaveConfig();
}

UInteractiveTool* UActorPaintModeBrushRawToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UActorPaintModeBrushRaw* NewTool = NewObject<UActorPaintModeBrushRaw>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	NewTool->SetPainter(Painter);

	return NewTool;
}

FInputCaptureRequest UBrushMouseWheelBehavior::WantsCapture(const FInputDeviceState& InputState)
{
	Modifiers.UpdateModifiers(InputState, Target);
	return Super::WantsCapture(InputState);
}

FInputCaptureUpdate UBrushMouseWheelBehavior::UpdateCapture(const FInputDeviceState& Input, const FInputCaptureData& Data)
{
	Modifiers.UpdateModifiers(Input, Target);
	return Super::UpdateCapture(Input, Data);
}


void UActorPaintModeBrushRaw::Setup()
{
	Super::Setup();

	if (PainterWidget) PainterWidget.Get()->OnButtonClick.BindUObject(this, &UActorPaintModeBrushRaw::OnSlateButtonClick);
}

void UActorPaintModeBrushRaw::CreateProperties()
{
	if (!PaintBrushProperties) PaintBrushProperties = NewObject<UActorPaintModeBrushProperties>(this, UActorPaintModeBrushProperties::StaticClass(), TEXT("Brush"));
	BaseBrushProperties = PaintBrushProperties;
	AddToolPropertySource(PaintBrushProperties);
}

bool UActorPaintModeBrushRaw::TryClick(const FHitResult& Result)
{
	if (CanSpawnValidValues() && CanSpawnDistanceCheck(Result))
	{
		LastSpawnPositionCenter = Result.Location;

		const FVector Normal = Result.ImpactNormal.GetSafeNormal();
		const FRotator SpawnRotation = FRotationMatrix::MakeFromZ(Normal).Rotator();

		TArray<FVector> Offsets;
		if (PaintBrushProperties->SpawnMode == ESpawnMode::Random)
		{
			Offsets = GetRandomPositionOffsets(Normal);
		}
		else
		{
			GEditor->BeginTransaction(FText::FromString("Spawn"));
			for (int i = 0; i < PaintBrushProperties->BrushDensity; ++i)
			{
				// Proximity Spawn ignored when all spawn on center
				if (AActor* Actor = DoSpawn(Result.Location, SpawnRotation))
				{
					PostSpawnAdjustement(Actor, Result);
				}
			}
			GEditor->EndTransaction();
			return true;
		}


		GEditor->BeginTransaction(FText::FromString("Spawn"));
		for (auto Offset : Offsets)
		{
			const FVector Pos = Result.Location + Offset;
			if (IsProximityAllowed(Pos))
			{
				if (AActor* Actor = DoSpawn(Pos, SpawnRotation))
				{
					PostSpawnAdjustement(Actor, Result);
				}
			}
		}
		GEditor->EndTransaction();
		return true;
	}
	return false;
}

void UActorPaintModeBrushRaw::OnSlateButtonClick(const FString& String) const
{
}

void UActorPaintModeBrushRaw::Render(IToolsContextRenderAPI* RenderAPI)
{
	Super::Render(RenderAPI);
	if (RenderAPI && bBrushVisible)
	{
		FToolDataVisualizer Draw;
		Draw.BeginFrame(RenderAPI);

		const FVector Normal = LastBrushStamp.WorldNormal.GetSafeNormal();

		const FVector Dir1 = FVector::CrossProduct(Normal, FVector::RightVector);
		const FVector Dir2 = FVector::CrossProduct(Normal, Dir1);

		Draw.DrawLine(LastBrushStamp.WorldPosition, LastBrushStamp.WorldPosition + (Dir1.GetSafeNormal() * LastBrushStamp.Radius), FLinearColor::Blue, 1);
		Draw.DrawLine(LastBrushStamp.WorldPosition, LastBrushStamp.WorldPosition + (Dir2.GetSafeNormal() * LastBrushStamp.Radius), FLinearColor::Red, 1);

		if (PaintBrushProperties->ProximityCheck)
		{
			Draw.DrawCircle(LastBrushStamp.WorldPosition, LastBrushStamp.WorldNormal, PaintBrushProperties->ProximityCheckRadius, 25, FLinearColor::White, 1, false);
		}
		Draw.EndFrame();
	}
}

void UActorPaintModeBrushRaw::DrawHUD(FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI)
{
	Super::DrawHUD(Canvas, RenderAPI);
}

void UActorPaintModeBrushRaw::Shutdown(EToolShutdownType ShutdownType)
{
	PaintBrushProperties->SaveConfig();
	Super::Shutdown(ShutdownType);
}


void UActorPaintModeBrushRaw::OnMouseWheelScrollUp(const FInputDeviceRay& CurrentPos)
{
	UE_LOGFMT(LogTemp, Warning, "OnMouseWheelScrollUp");
	if (isMouseWheelAltModifier && isMouseWheelCtrlModifier)
	{
		PaintBrushProperties->ProximityCheck |= true;
		ChangeProximityRadius(5);
	}
	else if (isMouseWheelAltModifier)
	{
		ChangeBrushDensity(1);
	}
	else
	{
		Super::OnMouseWheelScrollUp(CurrentPos);
	}
}

void UActorPaintModeBrushRaw::OnMouseWheelScrollDown(const FInputDeviceRay& CurrentPos)
{
	UE_LOGFMT(LogTemp, Warning, "OnMouseWheelScrollDown");
	if (isMouseWheelAltModifier && isMouseWheelCtrlModifier)
	{
		ChangeProximityRadius(-5);
		if (PaintBrushProperties->ProximityCheckRadius <= 0) PaintBrushProperties->ProximityCheck = false;
	}

	else if (isMouseWheelAltModifier)
	{
		ChangeBrushDensity(-1);
	}
	else
	{
		Super::OnMouseWheelScrollDown(CurrentPos);
	}
}

bool UActorPaintModeBrushRaw::CanSpawnDistanceCheck(const FHitResult& Result) const
{
	if (PaintBrushProperties->CustomSpawnDistance)
	{
		return FVector::Distance(LastSpawnPositionCenter, Result.Location) > PaintBrushProperties->CustomSpawnDistanceValue;
	}
	return FVector::Distance(LastSpawnPositionCenter, Result.Location) > PaintBrushProperties->BrushRadius * 2;
}

bool UActorPaintModeBrushRaw::CanSpawnValidValues() const
{
	return bPaintAllowed && PaintBrushProperties->ActorSpawnList.Num() > 0 && PaintBrushProperties->BrushDensity > 0 && TargetWorld.IsValid();
}

bool UActorPaintModeBrushRaw::IsProximityAllowed(const FVector& Position) const
{
	if (!PaintBrushProperties->ProximityCheck) return true;

	TArray<FOverlapResult> Overlaps;
	const FCollisionObjectQueryParams ObjectParams(FCollisionObjectQueryParams::AllDynamicObjects);

	if (TargetWorld->OverlapMultiByObjectType(Overlaps, Position, FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(PaintBrushProperties->ProximityCheckRadius)))
	{
		return false;
	}
	return true;
}

TArray<FVector> UActorPaintModeBrushRaw::GetRandomPositionOffsets(const FVector& Normal) const
{
	const float Radius = PaintBrushProperties->BrushRadius;

	// Get direction aligned with current plane
	const FVector Dir1 = FVector::CrossProduct(Normal, FVector::RightVector);
	const FVector Dir2 = FVector::CrossProduct(Normal, Dir1);

	TArray<FVector> Positions;
	for (int i = 0; i < PaintBrushProperties->BrushDensity; ++i)
	{
		FVector Position = (Dir1.GetSafeNormal() * FMath::RandRange(-Radius, Radius)) + (Dir2.GetSafeNormal() * FMath::RandRange(-Radius, Radius));
		Positions.Add(Position.GetClampedToMaxSize(PaintBrushProperties->BrushRadius));
	}
	return Positions;
}

AActor* UActorPaintModeBrushRaw::DoSpawn(const FVector& Position, const FRotator& Rotation)
{
	const FActorSpawnElement* SpawnInfo = GetActorFromPool();
	if (SpawnInfo == nullptr)
	{
		UE_LOGFMT(LogTemp, Warning, "No valid actor to spawn");
		return nullptr;
	}

	FActorSpawnParameters Parameters;
	Parameters.Name = MakeUniqueObjectName(this, SpawnInfo->Actor, SpawnInfo->ActorName, EUniqueObjectNameOptions::GloballyUnique);
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Actor = TargetWorld->SpawnActor<AActor>(SpawnInfo->Actor, Position, Rotation, Parameters);

	const FRotator RandomRotation = GetRandomRotation();

	UE_LOGFMT(LogTemp, Warning, "HasRandomRot {v}", RandomRotation.ToString());
	Actor->AddActorLocalRotation(RandomRotation);

	if (SpawnInfo->RandomScale.X == SpawnInfo->RandomScale.Y)
	{
		Actor->SetActorScale3D(FVector(SpawnInfo->RandomScale.X, SpawnInfo->RandomScale.X, SpawnInfo->RandomScale.X));
	}
	else
	{
		const float Rng = FMath::RandRange(SpawnInfo->RandomScale.X, SpawnInfo->RandomScale.Y);
		Actor->SetActorScale3D(FVector(Rng, Rng, Rng));
	}

	Actor->SetFolderPath("SpawnedActors");
	return Actor;
}

void UActorPaintModeBrushRaw::PostSpawnAdjustement(AActor* Actor, const FHitResult& Result) const
{
	if (!PaintBrushProperties->PostSpawnAdjustPosition && !PaintBrushProperties->PostSpawnAdjustRotation) return;
	FHitResult NewResult;

	FVector Direction;

	if (PaintBrushProperties->PostSpawnRayDirection == EPostSpawnRayMode::HitNormal)
	{
		Direction = Result.Normal.GetSafeNormal();
	}
	else if (PaintBrushProperties->PostSpawnRayDirection == EPostSpawnRayMode::HitObject)
	{
		Direction = (Actor->GetActorLocation() - Result.HitObjectHandle.GetLocation()).GetSafeNormal();
	}
	else if (PaintBrushProperties->PostSpawnRayDirection == EPostSpawnRayMode::WorldDown)
	{
		Direction = FVector::UpVector;
	}

	FVector StartPos = Actor->GetActorLocation() + Direction;
	FVector EndPos = Actor->GetActorLocation() - (Direction * PaintBrushProperties->PostSpawnSnapDownDistance);


	if (TargetWorld->LineTraceSingleByObjectType(NewResult, StartPos, EndPos, CollisionParam))
	{
		if (PaintBrushProperties->PostSpawnAdjustPosition)
		{
			Actor->SetActorLocation(NewResult.Location);
		}
		if (PaintBrushProperties->PostSpawnAdjustRotation)
		{
			FVector NewNormal = NewResult.Normal.GetSafeNormal();
			const FRotator SpawnRotation = FRotationMatrix::MakeFromZ(NewNormal).Rotator();
			Actor->SetActorRotation(SpawnRotation);
		}
	}
}

FActorSpawnElement* UActorPaintModeBrushRaw::GetActorFromPool() const
{
	const int ArrayCount = PaintBrushProperties->ActorSpawnList.Num();
	if (ArrayCount == 1)
	{
		return &PaintBrushProperties->ActorSpawnList[0];
	}
	if (ArrayCount > 0)
	{
		return &PaintBrushProperties->ActorSpawnList[FMath::RandRange(0, ArrayCount - 1)];
	}
	return nullptr;
}

FRotator UActorPaintModeBrushRaw::GetRandomRotation() const
{
	FRotator RngRotator = FRotator(0, 0, 0);
	if (PaintBrushProperties->RandomRotation & StaticCast<int32>(ERandomRotation::Pitch))
	{
		RngRotator.Pitch = FMath::FRandRange(0.0f, 360.0f);
	}
	if (PaintBrushProperties->RandomRotation & StaticCast<int32>(ERandomRotation::Yaw))
	{
		RngRotator.Yaw = FMath::FRandRange(0.0f, 360.0f);
	}
	if (PaintBrushProperties->RandomRotation & StaticCast<int32>(ERandomRotation::Roll))
	{
		RngRotator.Roll = FMath::FRandRange(0.0f, 360.0f);
	}
	UE_LOGFMT(LogTemp, Warning, "CheckRotation Roll:{a} = {v} ", PaintBrushProperties->RandomRotation, PaintBrushProperties->RandomRotation & StaticCast<int32>(ERandomRotation::Roll));
	return RngRotator;
}


void UActorPaintModeBrushRaw::ChangeBrushDensity(float Offset) const
{
	PaintBrushProperties->BrushDensity = FMath::Max(PaintBrushProperties->BrushDensity + Offset, 1);
}

void UActorPaintModeBrushRaw::ChangeProximityRadius(float Offset) const
{
	PaintBrushProperties->ProximityCheckRadius = FMath::Max(PaintBrushProperties->ProximityCheckRadius + Offset, 0);
}


#undef LOCTEXT_NAMESPACE
