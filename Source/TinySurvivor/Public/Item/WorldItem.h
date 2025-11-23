// WorldItem.h
/*
	===============================================================================
	[ FILE MODIFICATION NOTICE - DECAY SYSTEM INTEGRATION ]
	작성자: 양한아
	날짜: 2025/11/21
	
	본 파일에는 '부패(Decay) 시스템'을 통합하기 위한 변경이 포함되어 있습니다.
	해당 변경들은 모두 아래 표기된 주석 블록 내부에 위치합니다:
	
		// ■ Decay
		//[S]=====================================================================================
			(Decay 관련 통합 코드)
		//[E]=====================================================================================
		
	위 영역 외의 기존 풀링/스폰/인스턴싱 로직은 변경하지 않았습니다.
	Decay 시스템만 연동한 최소 변경입니다.
	후속 작업 시 해당 블록을 참고해주세요.
	===============================================================================
*/
#pragma once

#include "CoreMinimal.h"
#include "PoolableActorBase.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "Engine/StreamableManager.h"
#include "Runtime/DecayManager.h"
#include "Components/TextRenderComponent.h"
#include "WorldItem.generated.h"

class UStaticMeshComponent;
class USphereComponent;

// ■ Decay
//[S]=====================================================================================
class UDecayManager;
//[E]=====================================================================================

UCLASS()
class TINYSURVIVOR_API AWorldItem : public APoolableActorBase
{
	GENERATED_BODY()

public:
	AWorldItem();

	// 디버그. 머리 위에 정보를 띄울 텍스트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	UTextRenderComponent* DebugTextComp;
	
	void UpdateDebugText();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ItemData, Category = "Item")
	FSlotStructMaster ItemData;

	// 비동기 메시 로딩을 위한 핸들
	TSharedPtr<FStreamableHandle> MeshLoadHandle;
	
	UFUNCTION()
	void OnRep_ItemData();
	UFUNCTION()
	void OnInteractionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void UpdateAppearance();
	
	// ■ Decay
	//[S]=====================================================================================
	/*
		DecayManager 캐시
	*/
	UPROPERTY()
	TObjectPtr<UDecayManager> DecayManager;
	
	/*
		부패도 틱 핸들러
		1초마다 호출됨 (서버 전용)
	*/
	UFUNCTION()
	void OnDecayTick();
	
	/*
		부패물로 전환 처리
	*/
	void ConvertToDecayedItem();
	//[E]=====================================================================================
	// 부패 시스템 초기화 헬퍼 함수
	void InitializeDecaySystem();

public:
	// 이 액터가 유래된 HISM 인스턴스 인덱스 (-1이면 순수 액터)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pooling")
	int32 SourceInstanceIndex = -1;
	
	// 풀에서 액터 꺼낼 때
	virtual void OnAcquire_Implementation(const int32& IntParam, const FString& StringParam, const UObject* ObjectParam) override;
	// 풀로 액터를 반납할 때
	virtual void OnRelease_Implementation() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetItemData(const FSlotStructMaster& NewItemData);
	const FSlotStructMaster& GetItemData() const { return ItemData; }
	
	// ■ Decay
	//[S]=====================================================================================
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//[E]=====================================================================================
};
