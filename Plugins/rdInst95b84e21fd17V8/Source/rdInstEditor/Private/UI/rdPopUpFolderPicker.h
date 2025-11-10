// Copyright (c) 2021 Recourse Design ltd.
#pragma once
#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "AssetRegistry/AssetData.h"
#else
#include "AssetData.h"
#endif

DECLARE_DELEGATE_OneParam(FOnFolderSelected,const FString&);

class rdPopUpFolderPicker : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdPopUpFolderPicker)
		: _CurrentFolder()
		, _OnFolderSelected()
		, _FolderBoxWidth(120)
		, _PurePath(false)
		, _PrefixGame(false)
		, _ShowBtns(true)
	{}
		SLATE_ATTRIBUTE(FString,CurrentFolder)
		SLATE_EVENT(FOnFolderSelected,OnFolderSelected)
		SLATE_ATTRIBUTE(float,FolderBoxWidth)
		SLATE_ATTRIBUTE(bool,PurePath)
		SLATE_ATTRIBUTE(bool,PrefixGame)
		SLATE_ATTRIBUTE(bool,ShowBtns)
	SLATE_END_ARGS()

	void						Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget>			createFolderPicker();

	void						pickerFolderSelected(const FString& FolderPath);

	FText						getFolderName() const;

	FReply						selectClicked();
	FReply						browseClicked();

	TSharedPtr<SButton>			btnSelect;
	TSharedPtr<SButton>			btnBrowse;
	TSharedPtr<SMenuAnchor>		mnuAnchor;

	FOnFolderSelected			OnFolderSelected;
	float						folderBoxWidth;
	bool						purePath;
	bool						prefixGame;
	bool						showBtns;

	FString						lastFolder;
	FString						currentFolder;
};
