// Copyright (c) 2022 Recourse Design ltd.
#pragma once

#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "SceneOutlinerModule.h"
#include "rdInstEditor.h"

class rdPopUpActorPicker : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdPopUpActorPicker)
		: _CurrentActor(nullptr)
		, _OnActorSelected()
		, _IsMultipleValues(false)
		, _NameBoxWidth(120)
		, _ShowPicker(true)
		, _ActorList()
	{}
		SLATE_ATTRIBUTE(AActor*,CurrentActor)
		SLATE_EVENT(FOnActorPicked,OnActorSelected)
		SLATE_ATTRIBUTE(bool,IsMultipleValues)
		SLATE_ATTRIBUTE(float,NameBoxWidth)
		SLATE_ATTRIBUTE(bool,ShowPicker)
		SLATE_ATTRIBUTE(TArray<AActor*>,ActorList)
	SLATE_END_ARGS()

	void						Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget>			createActorPicker();

	void						pickerActorSelected(AActor* actor);

	FText						getActorName() const;

	bool						IsSelectableActor(const AActor* actor) const;
	void						OnActorPickedFromPicker(AActor* actor);

	FReply						clearClicked();
	FReply						browseClicked();
	bool						ShouldShowPicker() const { return showPicker; }

	TSharedPtr<SButton>			btnClear;
	TSharedPtr<SButton>			btnBrowse;
	TSharedPtr<SMenuAnchor>		mnuAnchor;

	FOnActorPicked				onActorSelected;
	float						nameBoxWidth;

	AActor*						currentActor;
	bool						isMultipleValues;
	bool						showPicker;
	TArray<AActor*>				actorList;
};
