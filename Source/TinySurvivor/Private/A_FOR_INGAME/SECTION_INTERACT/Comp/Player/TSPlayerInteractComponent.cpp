// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_INTERACT/Comp/Player/TSPlayerInteractComponent.h"
#include "A_FOR_COMMON/GameplayMessage/Data/Struct/Interact/FTSInteractMessageData.h"
#include "A_FOR_COMMON/GameplayMessage/Setting/TSGameplayChannelSetting.h"
#include "A_FOR_COMMON/Library/Getter/Controller/TSGetControllerLibrary.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSInteractInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

class UTSGameplayChannelSetting;

UTSPlayerInteractComponent::UTSPlayerInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UTSPlayerInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SubscribeInteract_Internal();
}

void UTSPlayerInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	TickInteract_internal();
}

void UTSPlayerInteractComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnsubscribeInteract_Internal();
	
	Super::EndPlay(EndPlayReason);
}

#pragma endregion 
//======================================================================================================================	
#pragma region 인터렉트_내부_동작_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트 내부 동작 API
	//━━━━━━━━━━━━━━━━━━━━



	//--------------------
	// 상호작용 
	//--------------------
	
void UTSPlayerInteractComponent::TickInteract_internal()
{
	// 캐싱 확인 (Valid 체크 있음)
	if (!HasCachingOwnerPlayerController_internal()) return;
	
	// 갱신 
	LastInteractActor = CurrentInteractActor;
	
	// 라인 트레이스 실시 
	CurrentInteractActor = LineTrace_internal();

	// 추 처리
	DoInteractUILogicAfterLineTrace();
}

bool UTSPlayerInteractComponent::HasCachingOwnerPlayerController_internal()
{
	// 이미 오너 캐싱햇으면 리턴 
	if (OwnerPlayerController.IsValid() && IsValid(OwnerPlayerController.Get())) return true;
	
	// 플레이어 컨트롤러 가져오기 	
	APlayerController* PC = UTSGetControllerLibrary::GetPlayerControllerFromComponent(GetOwner());
	if (!IsValid(PC)) return false;
	
	// 캐싱
	OwnerPlayerController = PC;
	
	// 후 검증 
	if (!OwnerPlayerController.IsValid() || !IsValid(OwnerPlayerController.Get())) return false;
	return true;
}

AActor* UTSPlayerInteractComponent::LineTrace_internal()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	
	// 1. 카메라의 위치와 회전값 가져오기 (화면 중앙 기준점)
	OwnerPlayerController->GetPlayerViewPoint(EyeLocation, EyeRotation);

	// 2. 시작점과 끝점 계산
	FVector TraceStart = EyeLocation;
	FVector TraceEnd = TraceStart + (EyeRotation.Vector() * InteractDistance);

	// 3. 트레이스 설정 (자기 자신은 무시)
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	// 4. 라인 트레이스 실행 (ECC_Visibility 채널 사용)
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECC_Visibility,QueryParams);
	
	// [추가] 디버그 함수 호출 (결과값과 HitResult를 같이 전달)
	DrawInteractionDebugLine_internal(TraceStart, TraceEnd, HitResult, bHit);
	
	// 맞은 게 있으면 반환 
	if (HitResult.bBlockingHit) return HitResult.GetActor();
	
	// 없으면 nullptr 반환 
	return nullptr;
}

void UTSPlayerInteractComponent::DrawInteractionDebugLine_internal(const FVector& Start, const FVector& End, const FHitResult& HitResult, bool bHit)
{
	if (!IsValid(GetWorld())) return;
	
	// 1. 색상 결정 (맞으면 녹색, 아니면 적색)
	FColor DebugColor = bHit ? FColor::Green : FColor::Red;

	// 2. 라인 그리기 (무조건 실행)
	DrawDebugLine(GetWorld(),Start,End,DebugColor,false, 0.1f, 0, 1.0f);

	// 3. 충돌 시에만 해당 위치에 스피어 그리기
	if (bHit) DrawDebugSphere(GetWorld(),HitResult.ImpactPoint, 10.0f,12, DebugColor,false, 0.1f);
}

void UTSPlayerInteractComponent::DoInteractUILogicAfterLineTrace()
{
	// 1. 상태 변화 체크: 현재 액터와 마지막 액터가 같다면 로직 수행 불필요
	if (CurrentInteractActor == LastInteractActor) return;
	
	// 2. [지난 액터 처리] 마지막 액터가 유효했다면 위젯 끄기
	if (LastInteractActor.IsValid() && IsValid(LastInteractActor.Get()) && LastInteractActor->GetClass()->ImplementsInterface(UTSInteractInterface::StaticClass()))
	{
		ITSInteractInterface::Execute_ToggleInteractWidget(LastInteractActor.Get(), false);
	}
	
	// 3. [현재 액터 처리] 현재 액터가 유효하다면 위젯 켜기
	if (CurrentInteractActor.IsValid() && IsValid(CurrentInteractActor.Get()) && CurrentInteractActor->GetClass()->ImplementsInterface(UTSInteractInterface::StaticClass()))
	{
		ITSInteractInterface::Execute_ToggleInteractWidget(CurrentInteractActor.Get(), true);
	}
}

	//--------------------
	// 게임 플레이 메시지 
	//--------------------

void UTSPlayerInteractComponent::SubscribeInteract_Internal()
{
	if (!IsValid(GetWorld())) return;
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	if (!IsValid(&MessageSubsystem)) return;	
	
	const UTSGameplayChannelSetting* Settings = GetDefault<UTSGameplayChannelSetting>();
	if (!IsValid(Settings)) return;
	
	// 구독 및 핸들 보관
	ListenerHandle = MessageSubsystem.RegisterListener<FTSInteractMessageData>(Settings->RequestCurrentWatchingActorChannelTag, this, &UTSPlayerInteractComponent::OnRequestCurrentActorChannelGameplayMessageReceived);
}

void UTSPlayerInteractComponent::OnRequestCurrentActorChannelGameplayMessageReceived(FGameplayTag InChannelTag, const FTSInteractMessageData& OutPayload)
{
	if (!IsValid(GetWorld())) return;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	if (!IsValid(&MessageSubsystem)) return;	
	
	FTSInteractMessageData MSG;
	MSG.CurrentInteractTarget = CurrentInteractActor;
	
	const UTSGameplayChannelSetting* Settings = GetDefault<UTSGameplayChannelSetting>();
	if (!IsValid(Settings)) return;
	
	MessageSubsystem.BroadcastMessage(Settings->SendCurrentWatchingActorChannelTag, MSG);
}

void UTSPlayerInteractComponent::UnsubscribeInteract_Internal()
{
	if (!IsValid(GetWorld())) return;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	if (!IsValid(&MessageSubsystem)) return;	
	
	if (!ListenerHandle.IsValid()) return;
	MessageSubsystem.UnregisterListener(ListenerHandle);
}

#pragma endregion 
//======================================================================================================================