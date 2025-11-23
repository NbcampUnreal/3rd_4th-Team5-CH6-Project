#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GiantSwitchingResourceArea.generated.h"

class ATSResourcePoint;
class ATSResourceBaseActor;
class UBoxComponent;

// 자원 원천 관련 세부 데이터  : 각 자원 원천의 재생 시간 관리용 데이터. 
USTRUCT(BlueprintType)
struct FTheResourceData
{
	GENERATED_BODY()
	
	// 재생시간 스택용 
	int32 ReplayTime = 0;
	
	// 재생 시간 
	UPROPERTY()
	int32 ReplayTimeMax = 600;
	
	// 자원 원천 
	UPROPERTY()
	TObjectPtr<ATSResourceBaseActor> Resource;
};

// 자원 원천 관련 데이터  : 각 섹터마다 내부에 독립적으로 존재하는 동적 데이터. 
USTRUCT(BlueprintType)
struct FResourcePointData
{
	GENERATED_BODY()
	
	// 들어갈 수 있는 자원 원천 타입 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceTypeTag")
	FGameplayTag ResourceTypeTag;
	
	// 들어갈 수 있는 자원 원천 생성 포인트
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ResourcePoints = {};
	
	// 최대로 들어갈 수 있는 자원 원천 숫자
	int32 MaxResourceCount = 1;
	
	// 자원 관리 데이터 풀 key : 자원 생성 포인트 / value : 활성화 중인 자원 데이터 
	TMap<TObjectPtr<ATSResourcePoint>, FTheResourceData> ResourcePool;
};

UCLASS()
class TINYSURVIVOR_API AGiantSwitchingResourceArea : public AActor
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------
	// AGiantSwitchingResourceArea 라이프 사이클
	//----------------------------------
	
public:
	AGiantSwitchingResourceArea();
	virtual void BeginPlay() override;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------
	// AGiantSwitchingResourceArea 관심 지역 시스템 / 자원 생성기 관리 시스템 공통 변수 
	//----------------------------------

public:
	// 지역 태그 게터 (외부 API)
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetResourceAreaTag() const { return ResourceAreaTag; }

protected:
	
	// 지역 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GiantSwitchingResourceArea")
	TObjectPtr<UBoxComponent> ResourceArea;
	
	// 지역 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GiantSwitchingResourceArea")
	FGameplayTag ResourceAreaTag;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------
	// AGiantSwitchingResourceArea 관심 지역 시스템 관련
	//----------------------------------
	
protected:
	// 1초마 타이머 구독 (잊는 시간 관리를 통한 관심 지역 잊기)
	UFUNCTION()
	void ControlInterestMaxAge();
	
public:
	// 모든 관심 지역 게터 (거인에게 쓰임) 
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetAllIntersectArea() const;
	
	// 관심 지역 추가 요청 API (서브 시스템으로부터 받는 것이니 직접 호출 금지)
	UFUNCTION(BlueprintCallable)
	void AddIntersectArea(const FVector& Location);	

	// 관심 지역 제거 요청 API (서브 시스템으로부터 받는 것이니 직접 호출 금지)
	UFUNCTION(BlueprintCallable)
	void RemoveIntersectArea(const FVector& Location);
	
protected:
	// interset 위치들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GiantSwitchingResourceArea")
	TSet<FVector> IntersectArea;
	
	// interset 위치 잊는 시간들
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GiantSwitchingResourceArea")
	TSet<float> InterestMaxAge;

	// 잊는 시간 스택용 변수
	int32 InterestMaxAgeTimer = 0;
	
	// 각 interset 위치마다 잊는 시각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GiantSwitchingResourceArea")
	int32 InterestMaxAgeTime = 120;
	
	// 가능한 지역 관심 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GiantSwitchingResourceArea")
	int32 MaxIntersectArea = 2;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------
	// AGiantSwitchingResourceArea 자원 생성기 관리 시스템
	//----------------------------------

public:
	// 자원 생성기 관리 풀 : Key 자원 원천 타입, Value 자원 원천 데이터 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GiantSwitchingResourceArea")
	TMap<FGameplayTag, FResourcePointData> ResourcePointPool;
};
