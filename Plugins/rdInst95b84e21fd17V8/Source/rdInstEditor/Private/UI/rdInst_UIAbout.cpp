//
// rdInst_UIAbout
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 3rd June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "rdInstOptions.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/SlateCore/Public/Brushes/SlateImageBrush.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Interfaces/IMainFrameModule.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

class FrdInstAboutOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstAboutOptionsCustomization();
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
protected:
};

class rdInstAboutOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstAboutOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstAboutOptions();
	void			Construct(const FArguments& InArgs);

	virtual bool	SupportsKeyboardFocus() const override { return true; }
	virtual FReply	OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override { return (InKeyEvent.GetKey()==EKeys::Escape)?OnConfirm():FReply::Unhandled(); }
	FReply			OnConfirm(){
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
private:
	TWeakPtr<SWindow>				WidgetWindow;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstAboutOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstAboutOptionsCustomization());
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstAboutOptionsCustomization::FrdInstAboutOptionsCustomization() {
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstAboutOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	IDetailCategoryBuilder& CategoryBuilder0=DetailBuilder.EditCategory(TEXT(" "));

	const FSlateBrush* sbrush=FPluginStyle::Get()->GetBrush("rdInst.Logo");
	FString ver=FString::Printf(TEXT("rdInst\n\nversion: %d.%d - %s\nRecourse Design ltd.\n\nUE version:%d.%d.%d\n\n\nvisit www.recourse.nz"),RDINST_MAJOR_VERSION,RDINST_MINOR_VERSION, TEXT(RDINST_RELEASEDATE), ENGINE_MAJOR_VERSION,ENGINE_MINOR_VERSION,ENGINE_PATCH_VERSION);

	FDetailWidgetRow& lrow0=CategoryBuilder0.AddCustomRow(FText::FromString(TEXT(" "))).IsEnabled(true);
	lrow0.NameContent()
		[
			SNew(SImage)
			.Image(sbrush)
		];

	TSharedPtr<SHorizontalBox> ContentBox0;
	lrow0.ValueContent()
		[
			SAssignNew(ContentBox0,SHorizontalBox)
		];

	ContentBox0->AddSlot()
		.Padding(FMargin(2.0f,2.0f,2.0f,2.0f))
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(*ver))
			.Font(DetailBuilder.GetDetailFontBold())
		];

	// .......................................................................
}

rdInstAboutOptions::rdInstAboutOptions() {}

void rdInstAboutOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=false;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Register instance property customization
	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(), 
														FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdInstAboutOptionsCustomization::MakeInstance(); }));

	// Set up root object customization to get desired layout
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));

	// Set provided objects on SDetailsView
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2) [	DetailsView->AsShared()	]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
				+ SUniformGridPanel::Slot(0,0) [
					rdWinButton("rdInst_Continue"," Continue ","rdInst_About_ToolTip0","Close and continue",&rdInstAboutOptions::OnConfirm)
				]

		]
	];
}

//.............................................................................
// ShowAboutSettings
//.............................................................................
bool FrdInstEditorModule::ShowAboutSettings() {

	// Create the settings window...
	TSharedRef<SWindow> winTex=SNew(SWindow)
										.Title(FText::FromString(TEXT("About rdInst")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
										.ClientSize(FVector2D(460,230));

	rdInstOptions=DuplicateObject(GetMutableDefault<UrdInstOptions>(),GetTransientPackage());
	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstAboutOptions> Options;

	winTex->SetContent(SAssignNew(Options,rdInstAboutOptions)
						.WidgetWindow(winTex)
						.SettingsObjects(OptionObjects)
					  );

	TSharedPtr<SWindow> ParentWindow;
	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return false;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(winTex,MainFrame.GetParentWindow(),false);

	return true;
}
//.............................................................................

#undef LOCTEXT_NAMESPACE
