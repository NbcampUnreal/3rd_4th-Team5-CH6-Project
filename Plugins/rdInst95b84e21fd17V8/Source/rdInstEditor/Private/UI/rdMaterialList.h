// Copyright (c) 2021 Recourse Design ltd.
#pragma once
#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "DetailCategoryBuilder.h"
#include "MaterialList.h"

DECLARE_DELEGATE_OneParam(FOnMaterialsUpdated,const TArray<UMaterialInterface*>&);

class rdMaterialList : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdMaterialList)
		: _OnMaterialsUpdated()
		, _TextBoxWidth(200)
		, _CatBuilder()
		, _MaterialList()
	{}
		SLATE_EVENT(FOnMaterialsUpdated,OnMaterialsUpdated)
		SLATE_ATTRIBUTE(float,TextBoxWidth)
		SLATE_ATTRIBUTE(IDetailCategoryBuilder*,CatBuilder)
		SLATE_ATTRIBUTE(TArray<UMaterialInterface*>,MaterialList)
	SLATE_END_ARGS()

	void						Construct(const FArguments& InArgs);

	void						OnCopyMaterialList();
	void						OnPasteMaterialList();
	bool						OnCanCopyMaterialList() const;

	void						OnGetMaterials(class IMaterialListBuilder& matList,int32 lod);
	void						OnMaterialListChanged(UMaterialInterface* newMaterial, UMaterialInterface* prevMaterial,int32 slot,bool bReplaceAll,int32 lod);
	TSharedRef<SWidget>			OnGenerateCustomNameWidgetsForMaterialList(UMaterialInterface* material,int32 matIndex);
	TSharedRef<SWidget>			OnGenerateCustomMaterialWidgetsForMaterialList(UMaterialInterface* material,int32 matIndex,int32 lod);
	FText						GetMaterialSlotName(int32 matIndex) const;
	void						OnMaterialNameCommitted(const FText& value, ETextCommit::Type commitType, int32 matIndex);
	bool						CanDeleteMaterial(int32 matIndex) const;
	void						OnDeleteMaterial(int32 matIndex);
	FReply						AddMaterial();
	bool						OnMaterialListDirty();
	void						SetMaterialList(UStaticMesh* mesh);

	FOnMaterialsUpdated				onMaterialsUpdated;

	TArray<UMaterialInterface*>		materialList;
	TArray<UMaterialInterface*>		origMaterialList;

	float							textBoxWidth=200.0f;
	IDetailCategoryBuilder*			catBuilder=nullptr;
	FMaterialList*					matListWidget=nullptr;
};
