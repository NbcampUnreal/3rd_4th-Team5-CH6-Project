// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceStaticData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSResourceDataSystemCachingHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceDataSystemCachingHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// [Base 전용] 캐싱 함수 
	template<typename RowType>
	static void CacheBaseTable_Lib(UDataTable* InTable, TMap<int32, FTSResourceStaticData>& InOutMap, RowType FTSResourceStaticData::* TargetMemberPtr)
	{
		if (!IsValid(InTable)) return;

		TArray<RowType*> Rows;
		InTable->GetAllRows<RowType>(TEXT("BaseInfoLoad"), Rows);

		for (RowType* Row : Rows)
		{
			if (!Row) continue; 
			if (Row->ResourceID <= 0) continue;

			// 이미 등록된 ID인지 확인 (중복 방지)
			if (InOutMap.Contains(Row->ResourceID)) continue;
			
			// 새로운 정적 데이터 구조체 생성 및 기본 정보 채우기
			FTSResourceStaticData NewStaticData;
			NewStaticData.*TargetMemberPtr = *Row;
		
			// 임시 맵에 추가
			InOutMap.Add(Row->ResourceID, NewStaticData);
		}
	}
	
	// [ base 이외 공통] 캐싱 함수 
	template<typename RowType>
	static void CacheOtherTable_Lib(UDataTable* InTable, TMap<int32, FTSResourceStaticData>& InOutMap, RowType FTSResourceStaticData::* TargetMemberPtr)
	{
		if (!IsValid(InTable)) return;

		TArray<RowType*> Rows;
		InTable->GetAllRows<RowType>(TEXT("OtherInfoLoad"), Rows);

		for (RowType* Row : Rows)
		{
			if (!Row) continue; 
			if (Row->ResourceID <= 0) continue;

			// 등록되지 않은 ID인지 확인 (기본 정보 없으면 패스)
			if (!InOutMap.Contains(Row->ResourceID)) continue;

			// 정적 데이터 가져오기
			FTSResourceStaticData* FoundITemStaticData = InOutMap.Find(Row->ResourceID);
			if (!FoundITemStaticData) continue;
		
			// 정보 넣기
			FoundITemStaticData->*TargetMemberPtr = *Row;
		}
	}
	
};
