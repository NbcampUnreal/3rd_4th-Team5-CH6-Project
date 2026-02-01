//
// rdDebugHUD.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2024
// Last Modified: 12th October 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdDebugHUD.h"
#include "rdInstBaseActor.h"
#include "rdInstSubsystem.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Brushes/SlateImageBrush.h"
#include "Components/Image.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

#if !UE_BUILD_SHIPPING
//.............................................................................
//  SetDebugHUDType
//.............................................................................
void FrdInstModule::SetDebugHUDType(int32 hudLevel) {

	if(hudLevel>0) {

		UrdInstSubsystem* instsys=GEngine->GetEngineSubsystem<UrdInstSubsystem>();
		if(instsys) {
			APlayerController* playerController=UGameplayStatics::GetPlayerController(instsys->rdGetBase()->GetWorld(),0);
			debugHUD=CreateWidget<UrdDebugHUD>(instsys->rdGetBase()->GetWorld(),UrdDebugHUD::StaticClass());
			if(debugHUD) {

				UCanvasPanel* canvas=debugHUD->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
				if(canvas) {

					debugHUD->WidgetTree->Modify();
					debugHUD->WidgetTree->RootWidget=canvas;

					UTextBlock* txt=debugHUD->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),FName(TEXT("rdInstStats")));
					if(txt) {
						txt->SetFont(FCoreStyle::GetDefaultFontStyle("Regular",11));
						txt->SetShadowOffset(FVector2D(1.0f,1.0f));
						txt->SetShadowColorAndOpacity(FLinearColor(0.0f,0.0f,0.0f,0.9f));
						txt->SetText(FText::FromString(TEXT("")));
						UCanvasPanelSlot* slot=canvas->AddChildToCanvas(txt);
						slot->SetPosition(FVector2D(200.0f,100.0f));
						debugHUD->statsText=txt;
					}
					UrdDebugGraph* graph=debugHUD->WidgetTree->ConstructWidget<UrdDebugGraph>(UrdDebugGraph::StaticClass(),FName(TEXT("rdInstGraph")));
					if(graph) {
						UCanvasPanelSlot* slot=canvas->AddChildToCanvas(graph);
						slot->SetPosition(FVector2D(200.0f,280.0f));
						slot->SetSize(FVector2D(400.0f,120.0f));
						debugHUD->statsGraph=graph;
						debugHUD->statsGraph->storedData.SetNumZeroed(debugScanSize*8);
					}
					txt=debugHUD->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),FName(TEXT("label1")));
					if(txt) {
						UCanvasPanelSlot* slot=canvas->AddChildToCanvas(txt);
						slot->SetPosition(FVector2D(605.0f,470.0f));
						txt->SetFont(FCoreStyle::GetDefaultFontStyle("Regular",10));
						txt->SetShadowOffset(FVector2D(1.0f,1.0f));
						txt->SetText(FText::FromString(TEXT("0")));
						debugHUD->statsGraph->stats0label=txt;
					}
					txt=debugHUD->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),FName(TEXT("label2")));
					if(txt) {
						UCanvasPanelSlot* slot=canvas->AddChildToCanvas(txt);
						slot->SetPosition(FVector2D(605.0f,370.0f));
						txt->SetFont(FCoreStyle::GetDefaultFontStyle("Regular",10));
						txt->SetShadowOffset(FVector2D(1.0f,1.0f));
						txt->SetText(FText::FromString(TEXT("1")));
						debugHUD->statsGraph->stats1label=txt;
					}
					txt=debugHUD->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),FName(TEXT("label3")));
					if(txt) {
						UCanvasPanelSlot* slot=canvas->AddChildToCanvas(txt);
						slot->SetPosition(FVector2D(605.0f,270.0f));
						txt->SetFont(FCoreStyle::GetDefaultFontStyle("Regular",10));
						txt->SetShadowOffset(FVector2D(1.0f,1.0f));
						txt->SetText(FText::FromString(TEXT("2")));
						debugHUD->statsGraph->stats2label=txt;
					}
				}
				debugHUD->AddToViewport();
				debugHUD->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}

	} else {

		if(debugHUD) {
			debugHUD->SetVisibility(ESlateVisibility::Hidden);
		}
		RemoveDebugHUD();
	}
}

//.............................................................................
//  RemoveDebugHUD
//.............................................................................
void FrdInstModule::RemoveDebugHUD() {

	debugHUD=nullptr;
	lastDebugLevel=0;
	IConsoleVariable* cvar=IConsoleManager::Get().FindConsoleVariable(TEXT("rdInst.Debug"));
	if(cvar) {
		cvar->Set(0);
	}
}

//.............................................................................
//  UpdateDebugHUD
//.............................................................................
void FrdInstModule::UpdateDebugHUD() {

	if(debugHUD) {

		if(!IsValid(debugHUD)) {
			debugHUD=nullptr;
			return;
		}

		UrdInstSubsystem* instsys=GEngine->GetEngineSubsystem<UrdInstSubsystem>();
		if(instsys) {
			ArdInstBaseActor* base=instsys->rdGetBase();
			if(!base->rdIsPlaying()) return;

			base->rdCalcInstanceCounts();

			int32 totInstances=base->numInstances+base->numRawInstances+base->numRecycleInstances+base->numAutoInstances;

			if(debugHUD->statsText) {
				FString str=FString::Printf(TEXT("totalInstances=%d\nnumManagedInstances=%d\nnumRawInstances=%d\nnumComponents=%d\nnumRawComponents=%d\nnumRecycleInstances=%d\nnumRecycleComponents=%d\nnumAutoInstances=%d\nnumAutoComponents=%d"),
					totInstances,base->numInstances,base->numRawInstances,base->numComponents,base->numRawComponents,base->numRecycleInstances,
					base->numRecycleComponents,base->numAutoInstances,base->numAutoComponents);
				debugHUD->statsText->SetText(FText::FromString(*str));
			}

			if(debugHUD->statsGraph) {

				int32 i=debugHUD->statsGraph->currentCyclicIndex*8;
				TArray<int32>& ary=debugHUD->statsGraph->storedData;

				ary[i++]=base->numInstances;
				ary[i++]=base->numRawInstances;
				ary[i++]=base->numComponents;
				ary[i++]=base->numRawComponents;
				ary[i++]=base->numRecycleInstances;
				ary[i++]=base->numRecycleComponents;
				ary[i++]=base->numAutoInstances;
				ary[i++]=base->numAutoComponents;

				debugHUD->statsGraph->currentCyclicIndex=(debugHUD->statsGraph->currentCyclicIndex+1)%debugScanSize;
			}
		}
	}
}

//.............................................................................
//  LogInstanceStats
//.............................................................................
void FrdInstModule::LogInstanceStats() {

	UrdInstSubsystem* instsys=GEngine->GetEngineSubsystem<UrdInstSubsystem>();
	if(instsys) {
		ArdInstBaseActor* base=instsys->rdGetBase();
		base->rdCalcInstanceCounts();

		int32 totInstances=base->numInstances+base->numRawInstances+base->numRecycleInstances+base->numAutoInstances;

	
		UE_LOG(LogTemp,Display,TEXT("rdInst: Instance Statistics\nTotal Instances: %d\nNum Managed Instanced: %d\nManaged Components: %d\nRaw Instances: %d\nRaw Components: %d\nRecycled Instances: %d\nRecycled Components: %d\nAuto Instances: %d\nAuto Components: %d"),
				totInstances,base->numInstances,base->numComponents,base->numRawInstances,base->numRawComponents,base->numRecycleInstances,base->numRecycleComponents,base->numAutoInstances,base->numAutoComponents
				);

		FVector ploc=FVector();
		APawn* pawn=base->rdGetControlledPawn();
		if(pawn) {
			FTransform tran=pawn->GetActorTransform();
			ploc=tran.GetLocation();
		}

		TArray<UActorComponent*> list;
		base->GetComponents(UInstancedStaticMeshComponent::StaticClass(),list);
		int32 totalNotCulled=0;
		for(auto comp:list) {
			UInstancedStaticMeshComponent* ismc=(UInstancedStaticMeshComponent*)comp;
			if(!ismc->GetStaticMesh()) continue;
			FName sid=base->rdGetSMCsid(ismc);
			float cullDist=0.0f;
			if(ismc->InstanceStartCullDistance>0.0f && ismc->InstanceEndCullDistance>=ismc->InstanceStartCullDistance) {
				cullDist=ismc->InstanceStartCullDistance+(ismc->InstanceEndCullDistance-ismc->InstanceStartCullDistance)/2.0f;
			}

			int32 num=ismc->GetInstanceCount();
			int32 vis=0;
			int32 numNotCulled=0;
			for(int32 i=0;i<num;i++) {
				FTransform t;
				ismc->GetInstanceTransform(i,t);
				FVector loc=t.GetTranslation();
				if(loc.Z<-400000.0f) {
					// hidden
				} else {
 	
					float dist=FVector::Distance(loc,ploc);
					if(cullDist==0.0f || dist<cullDist) {
						numNotCulled++;
						totalNotCulled++;
					}

					vis++;
				}

			}
			UE_LOG(LogTemp,Display,TEXT("   > %s - Instances: %d, visible: %d, not culled: %d - sid: %s"),*ismc->GetStaticMesh()->GetName(),num,vis,numNotCulled,*sid.ToString());
		}

		UE_LOG(LogTemp,Display,TEXT("> total not culled: %d"),totalNotCulled);

	}
}

//.............................................................................
//  LogInstanceStatsVerbose
//.............................................................................
void FrdInstModule::LogInstanceStatsVerbose() {

	UrdInstSubsystem* instsys=GEngine->GetEngineSubsystem<UrdInstSubsystem>();
	if(instsys) {
		ArdInstBaseActor* base=instsys->rdGetBase();
		base->rdCalcInstanceCounts();

		int32 totInstances=base->numInstances+base->numRawInstances+base->numRecycleInstances+base->numAutoInstances;

	
		UE_LOG(LogTemp,Display,TEXT("rdInst: Instance Statistics\nTotal Instances: %d\nNum Managed Instanced: %d\nManaged Components: %d\nRaw Instances: %d\n,Raw Components: %d\nRecycles Instances: %d\nRecycled Components: %d\nAuto Instances: %d\nAuto Components: %d"),
				totInstances,base->numInstances,base->numComponents,base->numRawInstances,base->numRawComponents,base->numRecycleInstances,base->numRecycleComponents,base->numAutoInstances,base->numAutoComponents
				);

		FVector ploc=FVector();
		APawn* pawn=base->rdGetControlledPawn();
		if(pawn) {
			FTransform tran=pawn->GetActorTransform();
			ploc=tran.GetLocation();
		}

		TArray<UActorComponent*> list;
		base->GetComponents(UInstancedStaticMeshComponent::StaticClass(),list);
		int32 totalNotCulled=0;
		for(auto comp:list) {
			UInstancedStaticMeshComponent* ismc=(UInstancedStaticMeshComponent*)comp;
			if(!ismc->GetStaticMesh()) continue;
			FName sid=base->rdGetSMCsid(ismc);
			float cullDist=0.0f;
			if(ismc->InstanceStartCullDistance>0.0f && ismc->InstanceEndCullDistance>=ismc->InstanceStartCullDistance) {
				cullDist=ismc->InstanceStartCullDistance+(ismc->InstanceEndCullDistance-ismc->InstanceStartCullDistance)/2.0f;
			}

			int32 num=ismc->GetInstanceCount();
			int32 vis=0;
			int32 numNotCulled=0;
			FTransform t;
			for(int32 i=0;i<num;i++) {
				ismc->GetInstanceTransform(i,t);
				FVector loc=t.GetTranslation();
				FVector scale=t.GetScale3D();
				if(loc.Z<-400000.0f || scale.X==0.0f) {
					// hidden
				} else {
 	
					float dist=FVector::Distance(loc,ploc);
					if(cullDist==0.0f || dist<cullDist) {
						numNotCulled++;
						totalNotCulled++;
					}

					vis++;
				}
			}
			UE_LOG(LogTemp,Display,TEXT("   > %s - Instances: %d, visible: %d, not culled: %d - sid: %s"),*ismc->GetStaticMesh()->GetName(),num,vis,numNotCulled,*sid.ToString());
			TArray<int32> hiddenIndices;
			TArray<int32> visibleIndices;
			TArray<int32> culledIndices;
			for(int32 i=0;i<num;i++) {
				ismc->GetInstanceTransform(i,t);
				FVector loc=t.GetTranslation();
				FVector scale=t.GetScale3D();
				if(loc.Z<-400000.0f || scale.X==0.0f) {
//					UE_LOG(LogTemp,Display,TEXT("      > %d - Hidden"),i);
					hiddenIndices.Add(i);
				} else {
 	
					float dist=FVector::Distance(loc,ploc);
					if(cullDist==0.0f || dist<cullDist) {
						visibleIndices.Add(i);
//						UE_LOG(LogTemp,Display,TEXT("      > %d - Visible - %f,%f,%f"),i,loc.X,loc.Y,loc.Z);
					} else {
						culledIndices.Add(i);
//						UE_LOG(LogTemp,Display,TEXT("      > %d - Culled - %f,%f,%f"),i,loc.X,loc.Y,loc.Z);
					}
				}
			}
			UE_LOG(LogTemp,Display,TEXT("      > Visible:"));
			for(int32 i:visibleIndices) {
				ismc->GetInstanceTransform(i,t);
				FVector loc=t.GetTranslation();
				UE_LOG(LogTemp,Display,TEXT("      > %d - %f,%f,%f"),i,loc.X,loc.Y,loc.Z);
			}
			UE_LOG(LogTemp,Display,TEXT("      > Culled:"));
			for(int32 i:culledIndices) {
				ismc->GetInstanceTransform(i,t);
				FVector loc=t.GetTranslation();
				UE_LOG(LogTemp,Display,TEXT("      > %d - %f,%f,%f"),i,loc.X,loc.Y,loc.Z);
			}
			UE_LOG(LogTemp,Display,TEXT("      > Hidden:"));
			for(int32 i:hiddenIndices) {
				UE_LOG(LogTemp,Display,TEXT("        > %d"),i);
			}
		}
	}
}
#endif
//.............................................................................
// NativePaint
//.............................................................................
int32 UrdDebugGraph::NativePaint(const FPaintArgs& args,const FGeometry& geo,const FSlateRect& cullRect,FSlateWindowElementList& drawElements,int32 layerId,const FWidgetStyle& widgetStyle,bool bParentEnabled) const {

	FPaintContext ctx(geo,cullRect,drawElements,layerId,widgetStyle,bParentEnabled);

	int32 sz=storedData.Num()/8;

	int32 maxItems=0;
	for(int32 j=0;j<sz;j++) {
		int32 nm=storedData[(j*8)]+storedData[(j*8)+1]+storedData[(j*8)+4]+storedData[(j*8)+6]; // total instances
		if(nm>maxItems) maxItems=nm;
	}
	if(maxItems<1) maxItems=1;

	if(stats1label) {
		stats1label->SetText(FText::FromString(*FString::Printf(TEXT("%d"),maxItems/2)));
	}
	if(stats2label) {
		stats2label->SetText(FText::FromString(*FString::Printf(TEXT("%d"),maxItems)));
	}
	int32 height=200;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	TArray<FVector2f> points1,points2,points3,points4,pointsGrid;
	pointsGrid.Add(FVector2f(401.0f,0.0f));
	pointsGrid.Add(FVector2f(401.0f,height));
#else
	TArray<FVector2D> points1,points2,points3,points4,pointsGrid;
	pointsGrid.Add(FVector2D(401.0f,0.0f));
	pointsGrid.Add(FVector2D(401.0f,height));
#endif

	int32 i=currentCyclicIndex*8;
	float gap=400.0f/(sz-1.0f);
	for(int32 j=0;j<sz;j++) {

		int32 numInstances=storedData[i];
		int32 numRawInstances=storedData[i+1];
		int32 numRecycleInstances=storedData[i+4];
		int32 numAutoInstances=storedData[i+6];

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		int32 runningTot=(numRecycleInstances*height)/maxItems;
		points3.Add(FVector2f(j*gap,height-runningTot));
		runningTot+=(numInstances*height)/maxItems;
		points1.Add(FVector2f(j*gap,height-runningTot));
		runningTot+=(numRawInstances*height)/maxItems;
		points2.Add(FVector2f(j*gap,height-runningTot));
		runningTot+=(numAutoInstances*height)/maxItems;
		points4.Add(FVector2f(j*gap,height-runningTot));
#else
		int32 runningTot=(numRecycleInstances*height)/maxItems;
		points3.Add(FVector2D(j*gap,height-runningTot));
		runningTot+=(numInstances*height)/maxItems;
		points1.Add(FVector2D(j*gap,height-runningTot));
		runningTot+=(numRawInstances*height)/maxItems;
		points2.Add(FVector2D(j*gap,height-runningTot));
		runningTot+=(numAutoInstances*height)/maxItems;
		points4.Add(FVector2D(j*gap,height-runningTot));
#endif

		i+=8;
		if(i>=(sz*8)) {
			i=0;
		}
	}

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	FVector2d desiredSize=GetPadding().GetDesiredSize();
	FVector2d pad(GetPadding().Left,GetPadding().Top);
#else
	FVector2D desiredSize=Padding.GetDesiredSize();
	FVector2D pad(Padding.Left,Padding.Top);
#endif

	FSlateDrawElement::MakeLines(drawElements,layerId++,
						geo.ToPaintGeometry(geo.GetLocalSize()-desiredSize,FSlateLayoutTransform(pad)),
						pointsGrid,ESlateDrawEffect::None,FLinearColor::Gray,true,1.0f);

	FSlateDrawElement::MakeLines(drawElements,layerId++,
						geo.ToPaintGeometry(geo.GetLocalSize()-desiredSize,FSlateLayoutTransform(pad)),
						points1,ESlateDrawEffect::None,FLinearColor::White,true,1.0f);

	FSlateDrawElement::MakeLines(drawElements,layerId++,
						geo.ToPaintGeometry(geo.GetLocalSize()-desiredSize,FSlateLayoutTransform(pad)),
						points2,ESlateDrawEffect::None,FLinearColor::Red,true,1.0f);

	FSlateDrawElement::MakeLines(drawElements,layerId++,
						geo.ToPaintGeometry(geo.GetLocalSize()-desiredSize,FSlateLayoutTransform(pad)),
						points3,ESlateDrawEffect::None,FLinearColor::Blue,true,1.0f);

	FSlateDrawElement::MakeLines(drawElements,layerId++,
						geo.ToPaintGeometry(geo.GetLocalSize()-desiredSize,FSlateLayoutTransform(pad)),
						points4,ESlateDrawEffect::None,FLinearColor::Green,true,1.0f);

	return layerId;
}

//.............................................................................
