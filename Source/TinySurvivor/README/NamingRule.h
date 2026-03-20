#pragma once



	// 0. 변수 관련 //

	// bool 값은 bIS 또는 bCan 로 시작한다.

	// 그 외 다른 데이터 값은 반드시 이 데이터 무얼 가리키는지 명확하게 적는다 (길어져도 상관없음)

	// FStruct 는 반드시 Data 라고 뒤에 붙인다.

	// TArray 는 반드시 Array 라고 뒤에 붙인다. 

	// TMap 는 반드시 Map 라고 뒤에 붙인다.

	// 그 외 다른 변수들도 뒤에 자신을 가리키는 값을 적는다. (float, int, duble 같은 기본 자료형은 제외) 


	// 클래스, 컴포넌트 관련
	// 클래스의 경우 Actor, Pawn, Character 에 따라 뒤에 붙인다. 
	// 컴포넌트의 경우 Comp 라고 뒤에 붙인다.



	// 1. 리플리케이트되는 변수 관련

	// 복제가 되는 변수는 반드시 끝에 _Rep 라고 붙인다.



	// 1.5 전역 상수 또는 매크로

	// 전부 대문자로 표기한다. #define MAX_PLAYER_COUNT 4



	// 1.6 이벤트 또는 델리게이트 관련

	// 공통 : 이벤트, 델리게이트 모두 On ... Delegate 또는 On ... Event 로 표시한다.



	// 2. API 관련

	// 외부 API와 내부 API를 반드시 구분한다.

	// 외부 API는 함수 끝에 아무것도 붙이지 않는다.

	// 내부 API는 반드시 함수 끝에 _internal 이라고 붙인다.



	// 3. RPC 관련

	// 서버 RPC는 반드시 ServerRPC_ 로 시작한다.

	// 클라이언트 RPC는 반드시 ClientRPC_로 시작한다.

	// 멀티 캐스트는 반드시 MultiCast_로 시작한다.



	// 4. 포인터 반환 타입 관련

	// 만약 데이터 자체를 복사해서 주는 게 아니라 포인터로 반환하는 경우 반드시 함수 끝에 Ptr 이라고 명시한다.



	// 5. 라이브러리화 시킨 함수의 경우, 인터페스 함수의 경우

	// 전역에서 쓰거나 라이브러리화 시켰거나 인터페이스화시킨 함수의 경우 다음과 같은 컨벤션을 따른다.

	// 2번과 3번을 지키되 라이브의 함수의 경우 _Lib 로 함수 명을 끝낸다.

	// 2번과 3번을 지키되 인터페이스 함수의 경우 _Int 로 함수 명을 끝낸다.



	// 5. 들어가는 인자는 반드시 In을 붙이고, 다시 빠져나오는 인자의 경우 Out을 붙인다.

	// 예를 들어 어떤 아이템을 찾기 위해 int32 값이 필요하다면 "InItemID"

	// 예를 들어 어떤 아이템을 찾으면 결과를 레퍼런스에 담아주고, 그걸 위해서 인자로 넘겨줘야 한다면 "OutItemData"



	// 아래는 예시.

	// 예시 : 아이템 데이터



	// 이 아이템은 플레이어나 몬스터 공격으로 부서질 수 있는가?

	// bool IsThisItemCanDestroyByAttack = false



	// 아이템을 복제시키는 거리

	// UPROPERTY(Replicated)

	// float ItemReplicateDistance_Rep = 100.f;







	// 아래는 예시.

	// 예시 : 아이템 API



	// 아이템 데이터 게터

	// 외부 API ex) virtual ItemData GetItemData() override;



	// 아이템 데이터를 찾는 내부 함수

	// 내부 API ex) void FindItemData_internal(int32 InItemID, ItemData& OutItemData)


	// 아이템 데이터 포인터 게터

	// 포인터 반환인 경우 ex) virtual ItemData* GetItemDataPtr() override;



	// 아이템 데이터 포인터 게터

	// 라이브러리 경우 ex) ItemData* GetItemDataPtr_Lib



	// 아이템 데이터 포인터 게터

	// 인터페이스 경우 ex) virtual ItemData* GetItemDataPtr_Int() override;



	// 아이템 데이터 게터

	// RPC 경우 ex) ItemData ServerRPC_GetItemData();#pragma once

	/**

		*