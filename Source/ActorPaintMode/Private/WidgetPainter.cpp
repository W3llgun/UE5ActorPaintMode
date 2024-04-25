// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetPainter.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText SWidgetPainter::GetButtonText() const
{
	return FText::FromString(text);
}

FReply SWidgetPainter::OnClickedButton()
{
	UE_LOG(LogTemp, Warning, TEXT("SWidgetPainter::OnClickedButton"));
	text = "XDD";
	bool V = OnButtonClick.ExecuteIfBound(text);
	return FReply::Handled();
}

void SWidgetPainter::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		  .VAlign(VAlign_Top)
		  .Padding(5.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InArgs._Message))
		]
		+ SScrollBox::Slot()
		  .VAlign(VAlign_Top)
		  .Padding(5.f)
		[
			SNew(SButton)
			.Text(this, &SWidgetPainter::GetButtonText)
			.OnClicked(this, &SWidgetPainter::OnClickedButton)
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
