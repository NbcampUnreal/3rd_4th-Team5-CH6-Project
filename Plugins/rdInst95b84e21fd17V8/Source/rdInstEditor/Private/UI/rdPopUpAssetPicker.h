// Copyright (c) 2022 Recourse Design ltd.
#pragma once

#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "AssetRegistry/AssetData.h"
#else
#include "AssetData.h"
#endif

DECLARE_DELEGATE_OneParam(FOnAssetSelected,const FAssetData&);

class rdPopUpAssetPicker : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdPopUpAssetPicker)
		: _CurrentAsset(nullptr)
		, _OnAssetSelected()
		, _FileBoxWidth(120)
		, _AssetFilter(UStaticMesh::StaticClass())
		, _DefaultEmptyValue(TEXT("(Select Asset)"))
	{}
		SLATE_ATTRIBUTE(TWeakObjectPtr<UObject>,CurrentAsset)
		SLATE_EVENT(FOnAssetSelected,OnAssetSelected)
		SLATE_ATTRIBUTE(float,FileBoxWidth)
		SLATE_ATTRIBUTE(UClass*,AssetFilter)
		SLATE_ATTRIBUTE(FString,DefaultEmptyValue)
	SLATE_END_ARGS()

	void						Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget>			createAssetPicker();

	void						pickerAssetSelected(const struct FAssetData& AssetData);

	FText						getAssetDetails() const;
	FText						getAssetName() const;
	FAssetData&					getAssetData() const;

	FReply						selectClicked();
	FReply						browseClicked();

	TSharedPtr<SButton>			btnSelect;
	TSharedPtr<SButton>			btnBrowse;
	TSharedPtr<SMenuAnchor>		mnuAnchor;

	mutable FAssetData			lastAssetData;
	FOnAssetSelected			OnAssetSelected;
	TAttribute<float>			fileBoxWidth;
	UClass*						assetFilter;

	TAttribute<TWeakObjectPtr<UObject>> currentAsset;
	FString						defaultEmptyValue;
};
