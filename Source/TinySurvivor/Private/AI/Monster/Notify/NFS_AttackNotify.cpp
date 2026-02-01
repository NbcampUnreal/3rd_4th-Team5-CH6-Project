// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/Notify/NFS_AttackNotify.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AI/Monster/Base/MonsterCharacterInterface.h"
#include "AI/Monster/MonsterGAS/TSMonsterAS.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UNFS_AttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	    
    AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (!Owner || !Owner->HasAuthority()) return;

    // ----------------------------------------------------------
    // 1) 몬스터 공격력 가져오기
    // ----------------------------------------------------------
   
    UAbilitySystemComponent* MonsterASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
    const UTSMonsterAS* MonsterAS = MonsterASC ? MonsterASC->GetSet<UTSMonsterAS>() : nullptr;
    float AttackDamageValue = MonsterAS ? MonsterAS->GetAttackDamage() : 0.f;

    // ----------------------------------------------------------
    // 2) 기존 Trace
    // ----------------------------------------------------------
    
    FVector Start = Owner->GetActorLocation();
    FVector Forward = Owner->GetActorForwardVector();
    FVector End = Start + Forward * 250.f;

    FVector BoxHalfSize(80.f, 120.f, 80.f);
    FRotator Orientation = Owner->GetActorRotation();

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Owner);

    TArray<FHitResult> HitResults;
    UKismetSystemLibrary::BoxTraceMultiForObjects(
        Owner, Start, End, BoxHalfSize, Orientation,
        ObjectTypes, false, ActorsToIgnore,
        EDrawDebugTrace::None, HitResults, true
    );

    TSet<AActor*> UniqueHitActors;
    for (auto& Hit : HitResults)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            UniqueHitActors.Add(HitActor);
        }
    }

    // ----------------------------------------------------------
    // 3) 각각의 플레이어에게 Gameplay Event Payload 보내기
    // ----------------------------------------------------------
    
    int32 AttackCount = 0;
    
    for (AActor* HitActor : UniqueHitActors)
    {
        ACharacter* PlayerChar = Cast<ACharacter>(HitActor);
        if (!PlayerChar) continue;

        APlayerController* ThePlayerController = Cast<APlayerController>(PlayerChar->GetController()); 
        if (!IsValid(ThePlayerController)) continue;
        
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerChar);
        if (!TargetASC || !HitAbilityClass) continue;

        // ====== 핵심: GameplayEventData 생성 ======
        FGameplayEventData EventData;
        EventData.EventTag = AttackTag;                 // 이 태그 Ability를 Event 방식으로 실행
        EventData.Instigator = Owner;                   // 공격자
        EventData.Target = PlayerChar;                  // 피격자
        EventData.EventMagnitude = AttackDamageValue;   // 공격력 전달

        // AbilitySpec 생성
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(
            HitAbilityClass,     // Ability Class
            1,                   // Ability Level
            INDEX_NONE,          // InputID
            Owner                // SourceObject
        );
        
        // ====== Ability 실행 (Payload 포함) ======
        TargetASC->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);
        ++AttackCount;
        UE_LOG(LogTemp, Warning, TEXT("Monster damage event sent: %s | Damage=%.1f"), *PlayerChar->GetName(), AttackDamageValue);
    }
    
    IMonsterCharacterInterface* MonsterInterface = Cast<IMonsterCharacterInterface>(Owner);
    if (!MonsterInterface) return;
    
    if (AttackCount <= 0)
    {
        MonsterInterface->SetSpeedIncrease();
    }
    else
    {
        MonsterInterface->ResetSpeed();
    }
}
