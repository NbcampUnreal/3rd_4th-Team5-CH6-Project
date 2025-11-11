// ItemInstance.h
#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.generated.h"

/*
	아이템 인스턴스 구조체
	아이템의 런타임 동적 데이터를 관리
	
	필수 데이터:
	1. StaticDataID: 정적 데이터 참조용 (ItemDataSubsystem에서 FItemData 조회)
	2. CreationServerTime: 서버 기준 생성 시각 (부패도 계산용)
	
	주요 역할:
	- 아이템 액터와 인벤토리 슬롯에서 사용
	- 정적 데이터는 StaticDataID를 통해 ItemDataSubsystem에서 조회
	- 부패도 계산은 CreationServerTime 기준으로 수행
	
	네트워크 고려사항:
	- 이 구조체는 인벤토리 시스템에서 복제(Replicate)될 예정
	- 서버에서 생성된 CreationServerTime을 클라이언트도 동일하게 보유
	- 부패도 계산은 각 클라이언트에서 로컬로 수행 (UI 표시용)
	- 부패물 전환은 서버에서만 수행
	
	참고:
	- 내구도, 스택 개수 등의 추가 동적 데이터는 별도 관리
*/
USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

public:
	FItemInstance()
		: StaticDataID(0)
		, CreationServerTime(0.0)
	{}

	FItemInstance(int32 InStaticDataID, double InCreationServerTime)
		: StaticDataID(InStaticDataID)
		, CreationServerTime(InCreationServerTime)
	{}

	//========================================
	// 필수 데이터
	//========================================
	
	/*
		정적 데이터 접근용 아이템 ID
		ItemDataSubsystem에서 FItemData를 조회하는 키 값
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemInstance")
	int32 StaticDataID;

	/*
		서버 기준 생성 시각 (GameTimeInSeconds)
		아이템 액터 스폰 시점에 서버의 GetWorld()->GetTimeSeconds() 값으로 기록
		
		용도:
		- 부패도 계산
		- 부패물 전환 판정
		
		네트워크:
		- 서버에서 설정된 값이 클라이언트로 복제됨
		- 클라이언트는 이 값을 기준으로 로컬에서 부패도 UI 계산
		
		타입 선택 이유:
		- FDateTime 대신 double 사용으로 네트워크 복제 안전성 확보
		- 서버-클라이언트 간 시간 동기화 불필요
		- 부패도 계산 단순화 (단순 뺄셈)
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ItemInstance")
	double CreationServerTime;
	
	//========================================
	// Helper Functions
	//========================================
	/*
		디버그 정보 출력
	*/
	UFUNCTION(BlueprintCallable, Category="ItemInstance")
	FString ToString() const;
};