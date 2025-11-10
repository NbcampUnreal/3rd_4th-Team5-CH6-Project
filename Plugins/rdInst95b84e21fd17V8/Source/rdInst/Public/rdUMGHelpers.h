// Copyright (c) 2022 Recourse Design ltd.
#pragma once

#include "Widgets/Input/SNumericEntryBox.h"

#define _ProdName "rdInst"

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#define GetAppFontStyle FAppStyle::GetFontStyle
#define GetAppStyle FAppStyle::Get
#define GetAppBrush FAppStyle::GetBrush
#else
#define GetAppFontStyle FEditorStyle::GetFontStyle
#define GetAppStyle FEditorStyle::Get
#define GetAppBrush FEditorStyle::GetBrush
#endif

// Rows

#define rdFullRow(bldr,rw,box,l) FDetailWidgetRow& rw=bldr.AddCustomRow(FText::FromString(TEXT(l))); \
	TSharedPtr<SHorizontalBox> box; rw.WholeRowContent()[SAssignNew(box,SHorizontalBox)];

#define rdRow(bldr,rw,box,l) FDetailWidgetRow& rw=bldr.AddCustomRow(FText::FromString(TEXT(l))); \
	rw.NameContent()[SNew(STextBlock).Text(FText::FromString(l)).Font(DetailBuilder.GetDetailFont())]; \
	TSharedPtr<SHorizontalBox> box; rw.ValueContent()[SAssignNew(box,SHorizontalBox)];

// SpinBoxes

#define rdSpinBox(box,var,mn,mx,w,tt1,tt2) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w)[ SNew(SSpinBox<float>).MinDesiredWidth(w) \
				.Value_Lambda([this](){ return var; }) \
				.OnValueChanged_Lambda([this](float val){ var=val;}) \
				.MinValue(mn).MaxValue(mx).MaxSliderValue(mx).ToolTipText(LOCTEXT(tt1,tt2))]]

#define rdSpinBoxI(box,var,mn,mx,w,tt1,tt2) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w)[ SNew(SSpinBox<uint32>).MinDesiredWidth(w) \
				.Value_Lambda([this](){ return var; }) \
				.OnValueChanged_Lambda([this](uint32 val){ var=val;}) \
				.MinValue(mn).MaxValue(mx).MaxSliderValue(mx).ToolTipText(LOCTEXT(tt1,tt2))]]

// NumBoxes

#if ENGINE_MAJOR_VERSION>4

#define rdNumBox(box,lab,var,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(1.0f)[ SNew(SNumericEntryBox<float>) \
				.AllowSpin(true) \
				.Value_Lambda([this](){return var;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.OnValueChanged_Lambda([this](float val){ var=val;}) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxI(box,lab,var,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(1.0f)[ SNew(SNumericEntryBox<int32>) \
				.AllowSpin(true) \
				.Value_Lambda([this](){return var;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.OnValueChanged_Lambda([this](int32 val){ var=val;}) \
				.Label()[ SNumericEntryBox<int32>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxEn(box,lab,var,mn,mx,w,col,en) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(1.0f)[ SNew(SNumericEntryBox<float>) \
				.AllowSpin(true) \
				.Value_Lambda([this](){return var;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.OnValueChanged_Lambda([this](float val){ var=val;}) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return en;}) \
			]]

#define rdNumBoxM(box,lab,var,dfV,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<float>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this]{return dfV?TOptional<float>(var):TOptional<float>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](float val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxMEn(box,lab,var,dfV,mn,mx,w,col,en) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<float>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this]{return dfV?TOptional<float>(var):TOptional<float>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](float val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return en;}) \
			]]

#define rdNumBoxMI(box,lab,var,dfV,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<int32>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this]{return dfV?TOptional<int32>(var):TOptional<int32>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](int32 val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.Label()[ SNumericEntryBox<int32>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxMIEn(box,lab,var,dfV,mn,mx,w,col,en) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<int32>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this]{return dfV?TOptional<int32>(var):TOptional<int32>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](int32 val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.Label()[ SNumericEntryBox<int32>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return en;}) \
			]]

#define rdNumBoxMS(box,lab,var,dfV,la,lt,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<float>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this](){return dfV?TOptional<float>(la?lt:var):TOptional<float>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](float val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(FMargin(0,1,-4,1)) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return !la;}) \
			]]

#else

#define rdNumBox(box,lab,var,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(1.0f)[ SNew(SNumericEntryBox<float>) \
				.Value_Lambda([=]{return var;}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([=](float val){ var=val;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxI(box,lab,var,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(1.0f)[ SNew(SNumericEntryBox<int32>) \
				.Value_Lambda([=]{return var;}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([=](int32 val){ var=val;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<int32>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxEn(box,lab,var,mn,mx,w,col,en) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(1.0f)[ SNew(SNumericEntryBox<float>) \
				.Value_Lambda([=]{return var;}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([=](float val){ var=val;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return en;}) \
			]]

#define rdNumBoxM(box,lab,var,dfV,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<float>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([=]{return dfV?TOptional<float>(var):TOptional<float>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([=](float val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxMEn(box,lab,var,dfV,mn,mx,w,col,en) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<float>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([=]{return dfV?TOptional<float>(var):TOptional<float>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([=](float val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return en;}) \
			]]

#define rdNumBoxMI(box,lab,var,dfV,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<int32>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this](){return dfV?TOptional<int32>(var):TOptional<int32>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](int32 val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<int32>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
			]]

#define rdNumBoxMIEn(box,lab,var,dfV,mn,mx,w,col,en) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<int32>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this](){return dfV?TOptional<int32>(var):TOptional<int32>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](int32 val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<int32>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return en;}) \
			]]

#define rdNumBoxMS(box,lab,var,dfV,la,lt,mn,mx,w,col) \
			box->AddSlot().Padding(0.0f,2.0f,2.0f,2.0f).AutoWidth()[ \
			SNew(SBox).WidthOverride(w).Padding(0)[ SNew(SNumericEntryBox<float>) \
				.UndeterminedString(FText::FromString(TEXT("Multi")))  \
				.Value_Lambda([this](){return dfV?TOptional<float>(la?lt:var):TOptional<float>();}) \
				.AllowSpin(true) \
				.OnValueChanged_Lambda([this](float val){ var=val;dfV=true;}) \
				.MinValue(mn).MaxValue(mx).MinSliderValue(mn).MaxSliderValue(mx) \
				.LabelPadding(0) \
				.Label()[ SNumericEntryBox<float>::BuildLabel(FText::FromString(lab),FLinearColor::White,col) ] \
				.IsEnabled_Lambda([this](){return !la;}) \
			]]

#endif

// Labels

#define rdCatLabel(box,l1,l2,tt1,tt2) box->AddSlot().Padding(0.0f,5.0f,2.0f,2.0f).AutoWidth()[SNew(SBox).WidthOverride(100)[SNew(STextBlock).MinDesiredWidth(100).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(tt1,tt2))]]
#define rdLabel(box,w,l1,l2) box->AddSlot().Padding(2.0f,5.0f,0.0f,2.0f).AutoWidth()[SNew(SBox).WidthOverride(w)[SNew(STextBlock).Text(LOCTEXT(l1,l2))]]
#define rdLabel2(box,l) box->AddSlot().Padding(10.0f,5.0f,0.0f,2.0f).AutoWidth()[SNew(SBox).WidthOverride(20)[SNew(STextBlock).Text(LOCTEXT(l,l))]]

// Checkboxes

#define rdCheckbox(box,var,l1,l2,t1,t2) box->AddSlot().Padding(margin).AutoWidth()[SNew(SCheckBox) \
			.IsChecked_Lambda([this](){ return var?ECheckBoxState::Checked:ECheckBoxState::Unchecked; }) \
			.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState){  var=(NewState==ECheckBoxState::Checked); }) \
			.ToolTipText(LOCTEXT(t1,t2))]; \
			box->AddSlot().Padding(labMargin).AutoWidth()[SNew(STextBlock).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).MinDesiredWidth(labWidth)]

#define rdCheckboxNL(box,var,t1,t2) box->AddSlot().Padding(margin).AutoWidth()[SNew(SCheckBox) \
			.IsChecked_Lambda([this](){ return var?ECheckBoxState::Checked:ECheckBoxState::Unchecked; }) \
			.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState){  var=(NewState==ECheckBoxState::Checked); }) \
			.ToolTipText(LOCTEXT(t1,t2))]

#define rdCheckboxEn(box,var,l1,l2,t1,t2,en) box->AddSlot().Padding(margin).AutoWidth()[SNew(SCheckBox) \
			.IsChecked_Lambda([this](){ return (en&&var)?ECheckBoxState::Checked:ECheckBoxState::Unchecked; }) \
			.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState){  var=(NewState==ECheckBoxState::Checked); }) \
			.IsEnabled_Lambda([this](){return en;}) \
			.ToolTipText(LOCTEXT(t1,t2))]; \
			box->AddSlot().Padding(labMargin).AutoWidth()[SNew(STextBlock).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).MinDesiredWidth(labWidth)]

#define rdCheckboxMEn(box,var,d,l1,l2,t1,t2,en) box->AddSlot().Padding(margin).AutoWidth()[SNew(SCheckBox) \
			.IsChecked_Lambda([this](){ return d?((en&&var)?ECheckBoxState::Checked:ECheckBoxState::Unchecked):ECheckBoxState::Undetermined; }) \
			.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState){  var=(NewState==ECheckBoxState::Checked); d=true; }) \
			.IsEnabled_Lambda([this](){return en;}) \
			.ToolTipText(LOCTEXT(t1,t2))]; \
			box->AddSlot().Padding(labMargin).AutoWidth()[SNew(STextBlock).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).MinDesiredWidth(labWidth)]

// Combos

#define rdStringCombo(box,var,src,onChng,mkC,cl,tt1,tt2,lw) box->AddSlot().Padding(margin).AutoWidth()[SNew(SComboBox<TSharedPtr<FString>>) \
			.OptionsSource(src).OnSelectionChanged(this,onChng).OnGenerateWidget(this,mkC).ToolTipText(LOCTEXT(tt1,tt2)) \
			.Content()[SAssignNew(cl,STextBlock).Text(FText::FromString(*var)).MinDesiredWidth(lw)]]

#define rdInt32Combo(box,var,src,onChng,mkC,cl,tt1,tt2,lw) box->AddSlot().Padding(margin).AutoWidth()[SNew(SComboBox<TSharedPtr<FString>>) \
			.OptionsSource(src).OnSelectionChanged(this,onChng).OnGenerateWidget(this,mkC).ToolTipText(LOCTEXT(tt1,tt2)) \
			.Content()[SAssignNew(cl,STextBlock).Text(FText::FromString(var)).MinDesiredWidth(lw)]]

#define rdInt32ComboEn(box,var,src,onChng,mkC,cl,tt1,tt2,lw,en) box->AddSlot().Padding(margin).AutoWidth()[SNew(SComboBox<TSharedPtr<FString>>) \
			.OptionsSource(src).OnSelectionChanged(this,onChng).OnGenerateWidget(this,mkC).ToolTipText(LOCTEXT(tt1,tt2)) \
			.Content()[SAssignNew(cl,STextBlock).Text(FText::FromString(var)).MinDesiredWidth(lw)] \
			.IsEnabled_Lambda([this](){return en;}) \
			]

// Editable Textbox

#define rdEditBox(bx,t,tc,w) bx->AddSlot().Padding(margin).AutoWidth()[SNew(SEditableTextBox).Text(this,t).OnTextCommitted(this,tc).MinDesiredWidth(w)]
#define rdEditBoxEn(bx,t,tc,w,en) bx->AddSlot().Padding(margin).AutoWidth()[SNew(SEditableTextBox).Text(this,t).OnTextCommitted(this,tc).MinDesiredWidth(w).IsEnabled_Lambda([this](){ return en; })]

// ListView
#define rdListView(bx,v,s,gr,sc,t,w) bx->AddSlot().Padding(FMargin(2.0f)).AutoWidth()[SNew(SBox).WidthOverride(w)[SAssignNew(v,SListView<TSharedPtr<FString>>).ListItemsSource(s).SelectionMode(ESelectionMode::SingleToggle).OnGenerateRow(this,gr).OnSelectionChanged(this,sc).HeaderRow(SNew(SHeaderRow)+ SHeaderRow::Column(TEXT(t)).FillWidth(w).DefaultLabel(LOCTEXT("TypeColumnHeaderName",t)))]];
#define rdListViewEn(bx,v,s,gr,sc,t,w,en) bx->AddSlot().Padding(FMargin(2.0f)).AutoWidth()[SNew(SBox).WidthOverride(w)[SAssignNew(v,SListView<TSharedPtr<FString>>).ListItemsSource(s).SelectionMode(ESelectionMode::SingleToggle).OnGenerateRow(this,gr).OnSelectionChanged(this,sc).HeaderRow(SNew(SHeaderRow)+ SHeaderRow::Column(TEXT(t)).FillWidth(w).DefaultLabel(LOCTEXT("TypeColumnHeaderName",t))).IsEnabled_Lambda([this](){ return en; })]];

// Color Picker
#if ENGINE_MAJOR_VERSION<5
#define rdColorPicker(bx,wd,v,scp,tt1,tt2) bx->AddSlot().Padding(FMargin(2.0f,2.0f,2.0f,2.0f)).AutoWidth()[SAssignNew(wd,SColorBlock).Color_Lambda([this](){return v;}).ShowBackgroundForAlpha(true).IgnoreAlpha(false).OnMouseButtonDown(this,scp).Size(FVector2D(90.f,20.f)).ToolTipText(LOCTEXT(tt1,tt2))];
#define rdColorPickerEn(bx,wd,v,scp,tt1,tt2,en) bx->AddSlot().Padding(FMargin(2.0f,2.0f,2.0f,2.0f)).AutoWidth()[SAssignNew(wd,SColorBlock).Color_Lambda([this](){return v;}).ShowBackgroundForAlpha(true).IgnoreAlpha(false).OnMouseButtonDown(this,scp).Size(FVector2D(90.f,20.f)).ToolTipText(LOCTEXT(tt1,tt2)).IsEnabled_Lambda([this](){ return en; })];
#define rdColorPickerMEn(bx,wd,v,d,scp,tt1,tt2,en) bx->AddSlot().Padding(FMargin(2.0f,2.0f,2.0f,2.0f)).AutoWidth()[SAssignNew(wd,SColorBlock).Color_Lambda([this](){return v;}).ShowBackgroundForAlpha(true).IgnoreAlpha(false).OnMouseButtonDown(this,scp).Size(FVector2D(90.f,20.f)).ToolTipText(LOCTEXT(tt1,tt2)).IsEnabled_Lambda([this](){ return en; })];
#else
#define rdColorPicker(bx,wd,v,scp,tt1,tt2) bx->AddSlot().Padding(FMargin(2.0f,2.0f,2.0f,2.0f)).AutoWidth()[SAssignNew(wd,SColorBlock).Color_Lambda([this](){return v;}).ShowBackgroundForAlpha(true).AlphaDisplayMode(EColorBlockAlphaDisplayMode::Combined).OnMouseButtonDown(this,scp).Size(FVector2D(90.f,20.f)).ToolTipText(LOCTEXT(tt1,tt2))];
#define rdColorPickerEn(bx,wd,v,scp,tt1,tt2,en) bx->AddSlot().Padding(FMargin(2.0f,2.0f,2.0f,2.0f)).AutoWidth()[SAssignNew(wd,SColorBlock).Color_Lambda([this](){return v;}).ShowBackgroundForAlpha(true).AlphaDisplayMode(EColorBlockAlphaDisplayMode::Combined).OnMouseButtonDown(this,scp).Size(FVector2D(90.f,20.f)).ToolTipText(LOCTEXT(tt1,tt2)).IsEnabled_Lambda([this](){ return en; })];
#define rdColorPickerMEn(bx,wd,v,d,scp,tt1,tt2,en) bx->AddSlot().Padding(FMargin(2.0f,2.0f,2.0f,2.0f)).AutoWidth()[SAssignNew(wd,SColorBlock).Color_Lambda([this](){return v;}).ShowBackgroundForAlpha(true).AlphaDisplayMode(EColorBlockAlphaDisplayMode::Combined).OnMouseButtonDown(this,scp).Size(FVector2D(90.f,20.f)).ToolTipText(LOCTEXT(tt1,tt2)).IsEnabled_Lambda([this](){ return en; })];
#endif

// Buttons

#define rdWinButton(l1,l2,t1,t2,cl) SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).OnClicked(this,cl).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
#define rdWinButtonEn(l1,l2,t1,t2,cl,en) SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).OnClicked(this,cl).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText").IsEnabled_Lambda([this](){ return en; })
			
// Actor Picker
#define rdActorPicker(bx,ca,as,al,w) bx->AddSlot().Padding(0.2f).AutoWidth()[SNew(rdPopUpActorPicker).CurrentActor(ca).OnActorSelected(this,as).ActorList(al).ShowPicker(false).NameBoxWidth(w)];
#define rdActorPickerEn(bx,ca,as,al,w,en) bx->AddSlot().Padding(0.2f).AutoWidth()[SNew(rdPopUpActorPicker).CurrentActor(ca).OnActorSelected(this,as).ActorList(al).ShowPicker(false).NameBoxWidth(w).IsEnabled_Lambda([this](){ return en; })];

// Asset Picker
#define rdAssetPicker(bx,as,ca,w,c) bx->AddSlot().Padding(0.2f).AutoWidth()[SNew(rdPopUpAssetPicker).OnAssetSelected(this,as).CurrentAsset(this,ca).FileBoxWidth(w).AssetFilter(c)];
#define rdAssetPickerEn(bx,as,ca,w,c,en) bx->AddSlot().Padding(0.2f).AutoWidth()[SNew(rdPopUpAssetPicker).OnAssetSelected(this,as).CurrentAsset(this,ca).FileBoxWidth(w).AssetFilter(c).IsEnabled_Lambda([this](){ return en; })];

// Class Picker
#define rdClassPicker(bx,as,ca,w,c) bx->AddSlot().Padding(0.2f).AutoWidth()[SNew(rdPopUpClassPicker).OnClassSelected(this,as).CurrentClass(this,ca).FileBoxWidth(w).ClassFilter(c)];
#define rdClassPickerEn(bx,as,ca,w,c,en) bx->AddSlot().Padding(0.2f).AutoWidth()[SNew(rdPopUpClassPicker).OnClassSelected(this,as).CurrentClass(this,ca).FileBoxWidth(w).ClassFilter(c).IsEnabled_Lambda([this](){ return en; })];

// Menus

#define rdAddMenuItem(l,tt,i,a) MenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(tt,tt),FSlateIcon(FPluginStyle::GetStyleSetName(),i,i),FExecuteAction::CreateRaw(this,a),NAME_None,EUserInterfaceActionType::Button)
#define rdAddMenuItemR(l,tt,i,c,a) MenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(tt,tt),FSlateIcon(FPluginStyle::GetStyleSetName(),i,i),FExecuteAction::CreateRaw(c,a),NAME_None,EUserInterfaceActionType::Button)
#define rdAddSubMenu(l,i,a)	MenuBuilder.AddSubMenu(LOCTEXT(l,l),LOCTEXT(l,l),FNewMenuDelegate::CreateRaw(this,a),false,FSlateIcon(FPluginStyle::GetStyleSetName(),i,i))
#define rdAddSubMenuR(l,i,c,a) MenuBuilder.AddSubMenu(LOCTEXT(l,l),LOCTEXT(l,l),FNewMenuDelegate::CreateRaw(c,a),false,FSlateIcon(FPluginStyle::GetStyleSetName(),i,i))
#define rdAddMenuDiv() MenuBuilder.AddMenuSeparator()

#define rdAddSubMenuT(l,i,a) Section.AddEntry(FToolMenuEntry::InitSubMenu(NAME_None,LOCTEXT(l,l),LOCTEXT(l,l),FNewToolMenuDelegate::CreateRaw(this,a),false,FSlateIcon(FPluginStyle::GetStyleSetName(),i,i)))
#define rdAddMenuItemT(l,tt,i,a) Section.AddEntry(FToolMenuEntry::InitMenuEntry(NAME_None,LOCTEXT(l,l),LOCTEXT(tt,tt),FSlateIcon(FPluginStyle::GetStyleSetName(),i,i),FUIAction(FExecuteAction::CreateRaw(this,a))))
#define rdAddMenuDivT() Section.AddSeparator(NAME_None)

