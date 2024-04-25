// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
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

	FOnButtonClickCallback OnButtonClick;
	/** Constructs this widget with InArgs */
	FString text = "AA";
	FText GetButtonText() const;
	FReply OnClickedButton();
	void Construct(const FArguments& InArgs);

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		text = "IN";
		SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	}

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		text = "OUT";
		SCompoundWidget::OnMouseLeave(MouseEvent);
	}
};
