// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Test Slate widget I used to override default Details view of the EditorMode
 */
class ACTORPAINTMODE_API SWidgetPainter : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWidgetPainter)
		{
		}

		SLATE_ARGUMENT(FString, Message)
	SLATE_END_ARGS()

	DECLARE_DELEGATE_OneParam(FOnButtonClickCallback, const FString&);

	void Construct(const FArguments& InArgs);
	FText GetButtonText() const;
	FOnButtonClickCallback OnButtonClick;

private:
	/** Constructs this widget with InArgs */
	FString DisplayText = "Button";


	FReply OnClickedButton();


	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		DisplayText = "Hover";
		SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	}

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		DisplayText = "Exit";
		SCompoundWidget::OnMouseLeave(MouseEvent);
	}
};
