// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Enum/TSResourceType.h"
#include "Engine/DataTable.h"
#include "TSResourceBaseInfoTable.generated.h"

class ATSResourceActorBase;
class UTSResourceInfoDataAsset;
/**
 * 
 */
USTRUCT(blueprinttype)
struct FTSResourceBaseInfoTable : public FTableRowBase
{
	GENERATED_BODY()
	
	// 자원 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 ResourceID = -1;
	
	// 자원 타입 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	ETSResourceType ResourceType = ETSResourceType::None;
	
	// 자원 클래스 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	TSubclassOf<ATSResourceActorBase> ResourceClass = nullptr;
	
	// 자원 데이터 에셋 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	TSubclassOf<UTSResourceInfoDataAsset> ResourceDataAsset = nullptr;
};
