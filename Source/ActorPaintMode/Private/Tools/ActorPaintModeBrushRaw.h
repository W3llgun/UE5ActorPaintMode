// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBaseBrush.h"
#include "InteractiveTool.h"
#include "InteractiveToolBuilder.h"
#include "BaseBehaviors/MouseWheelBehavior.h"
#include "ActorPaintModeBrushRaw.generated.h"

class SWidgetPainter;

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ERandomRotation: uint8
{
	None = 0b000 UMETA(Hidden),
	Pitch = 0b001,
	Yaw = 0b010,
	Roll = 0b100
};

ENUM_CLASS_FLAGS(ERandomRotation);

UENUM()
enum ESpawnMode
{
	Random,
	Center,
};

UENUM()
enum EPostSpawnRayMode
{
	HitNormal,
	HitObject,
	WorldDown
};

USTRUCT()
struct FActorSpawnElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = AA)
	FName ActorName = NAME_Actor;
	UPROPERTY(EditAnywhere, Category = AA)
	TSubclassOf<AActor> Actor;
	UPROPERTY(EditAnywhere, Category = AA, DisplayName="RandomScale Min/Max")
	FVector2f RandomScale = FVector2f(1.0f, 1.0f);
};

UCLASS(config = ActorPaintMode)
class UActorPaintModeBrushProperties : public UActorBaseBrushProperties
{
	GENERATED_BODY()

public:
	ACTORPAINTMODE_API UActorPaintModeBrushProperties();

	UPROPERTY(Config, EditAnywhere, Category = Brush)
	TArray<FActorSpawnElement> ActorSpawnList;

	UPROPERTY(Config, EditAnywhere, Category = Brush, meta = (DisplayName = "Density", UIMin = "1.0", UIMax = "1000.0", ClampMin = "1", ClampMax = "5000.0"))
	int BrushDensity;

	UPROPERTY(Config, EditAnywhere, Category = Paint, meta = (DisplayName = "RandomRotation", Bitmask, BitmaskEnum = "/Script/ActorPaintMode.ERandomRotation"))
	int32 RandomRotation;
	UPROPERTY(Config, EditAnywhere, Category = Paint, meta = (DisplayName = "CustomSpawnDistance"))
	bool CustomSpawnDistance;
	UPROPERTY(Config, EditAnywhere, Category = Paint, meta = (DisplayName = "CustomSpawnDistanceValue", EditCondition="CustomSpawnDistance", UIMin = "1.0", UIMax = "500.0", ClampMin = "0.1", ClampMax = "500.0"))
	float CustomSpawnDistanceValue;
	UPROPERTY(Config, EditAnywhere, Category = Paint, meta = (DisplayName = "ProximityCheck"))
	bool ProximityCheck;
	UPROPERTY(Config, EditAnywhere, Category = Paint, meta = (DisplayName = "ProximityRadius", EditCondition="ProximityCheck", UIMin = "1.0", UIMax = "1000.0", ClampMin = "0.1", ClampMax = "50000.0"))
	float ProximityCheckRadius;
	UPROPERTY(EditAnywhere, Category = Paint, meta = (DisplayName = "RoofHeight"))
	float RoofHeight;
	UPROPERTY(Config, EditAnywhere, Category = Paint, meta = (DisplayName = "SpawnMode"))
	TEnumAsByte<ESpawnMode> SpawnMode;

	UPROPERTY(Config, EditAnywhere, Category = PostSpawn, meta = (DisplayName = "Adjust Position", Tooltip="Snap Position to the surface after a spawn"))
	bool PostSpawnAdjustPosition;
	UPROPERTY(Config, EditAnywhere, Category = PostSpawn, meta = (EditCondition = "PostSpawnAdjustPosition", DisplayName = "Adjust Position Distance", Tooltip="Snap Position to the surface after a spawn max distance check", UIMin = "0.1", UIMax = "2000.0", ClampMin = "0.1", ClampMax = "2000.0"))
	float PostSpawnSnapDownDistance = 50.f;
	UPROPERTY(Config, EditAnywhere, Category = PostSpawn, meta = (DisplayName = "Adjust Rotation", Tooltip="Snap Rotation to the surface after a spawn. Can nullify RandomRotation in some situations."))
	bool PostSpawnAdjustRotation;
	UPROPERTY(Config, EditAnywhere, Category = PostSpawn, meta = (DisplayName = "Check Direction", Tooltip="Define direction to search for surface"))
	TEnumAsByte<EPostSpawnRayMode> PostSpawnRayDirection;
	// Functions to save and load this objects to a file
	UFUNCTION(Category="Presets", CallInEditor)
	void Save();
};


UCLASS()
class ACTORPAINTMODE_API UActorPaintModeBrushRawToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	TSharedPtr<SWidgetPainter> Painter;
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

UCLASS()
class UBrushMouseWheelBehavior : public UMouseWheelInputBehavior
{
	GENERATED_BODY()

public:
	virtual FInputCaptureRequest WantsCapture(const FInputDeviceState& InputState) override;
	virtual FInputCaptureUpdate UpdateCapture(const FInputDeviceState& Input, const FInputCaptureData& Data) override;
};

/**
 * 
 */
UCLASS()
class ACTORPAINTMODE_API UActorPaintModeBrushRaw : public UActorBaseBrush
{
private:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UActorPaintModeBrushProperties> PaintBrushProperties;
	FVector LastSpawnPositionCenter;

	TSharedPtr<SWidgetPainter> PainterWidget;
	bool CanSpawnDistanceCheck(const FHitResult& Result) const;
	bool CanSpawnValidValues() const;
	bool IsProximityAllowed(const FVector& Position) const;
	AActor* DoSpawn(const FVector& Position, const FRotator& Rotation);
	void PostSpawnAdjustement(AActor* Actor, const FHitResult& Result) const;
	FActorSpawnElement* GetActorFromPool() const;
	TArray<FVector> GetRandomPositionOffsets(const FVector& Normal) const;
	FRotator GetRandomRotation() const;
	void ChangeBrushDensity(float Offset) const;
	void ChangeProximityRadius(float Offset) const;
	virtual void CreateProperties() override;
	virtual bool TryClick(const FHitResult& ClickInfo) override;
public:
	void SetPainter(const TSharedPtr<SWidgetPainter>& PainterParam) { PainterWidget = PainterParam; };

	void OnSlateButtonClick(const FString& String) const;
	virtual void Setup() override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
	virtual void DrawHUD(FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI) override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	// Mouse Wheel input
	virtual void OnMouseWheelScrollUp(const FInputDeviceRay& CurrentPos) override;
	virtual void OnMouseWheelScrollDown(const FInputDeviceRay& CurrentPos) override;
};
