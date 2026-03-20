#pragma once

// 1. 코드 컨밴션 
// 파일 단위 구조 처리

// :: 모든 파일은 다음과 같은 큰 목차로 구분한다. // 

// 1. 해당 파일의 클래스 및 이하 단위 게터, 델리게이트등은 최상단에 위치한다.

// 2. 라이프 사이클, 게임 사이클은 그 아래에 위치한다.

// 3. 외부에서 쓰는 API와 내부에서 쓰는 API를 구별하지 않고 "하나의 섹션"에 뭉쳐서 저장한다. 
// 예를 들어 "카메라" "스프링암" 관련 API 는 "시점"이라는 섹션에 뭉칠 수 있으므로 하나의 섹션에 뭉쳐서 저장한다.

// 4. 하나의 섹션은 "외부 API, 내부 API, 외부에서 쓸 수 있는 데이터, 내부에서만 쓰는 데이터 순으로 정리해서 저장한다."

// 5. 모든 파일 섹션은 반드시 "구분줄", "region 표시" "내부 표시말"을 붙인다.

// 6. 내부 표시말은 반드시 위아래 - 가 20개씩 들어간다.

// 7. 섹션마다 반드시 독립적인 접근 지정자를 부여한다.

// 8. 라이프 사이클, 게임 사이클은 전부 public으로 고정한다.

// 9. 데이터나 API는 무슨 일을 하는 함수인지 반드시 상단에 주석으로 설명한다.

// 아래는 예시

/*

UClASS()

UCLASS()
class TINYSURVIVOR_API MyCharacter : public ACharacter
{
	GENERATED_BODY()
	
//======================================================================================================================
#pragma region 게터

	//━━━━━━━━━━━━━━━━━━━━
	// 게터 
	//━━━━━━━━━━━━━━━━━━━━
	
public :
	static MyCharacter* GetMyCharacter();

#pragam endrigon 
//======================================================================================================================
#pragma region 라이프 사이클

	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
	
public :
	// 생성자
	MyCharacter();
	
	// 비긴 플레이 함수
	virtual void BeginPlay() override;

#pragam endrigon 
//======================================================================================================================
#pragma region 카메라

	//━━━━━━━━━━━━━━━━━━━━
	// 카메라
	//━━━━━━━━━━━━━━━━━━━━
	
public : 
	// 스프링 암 및 카메라 게터
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	
	// 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera" , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	
	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> CameraComponent;
	
protected:
	//shoulder Switch 설정 오프셋 값 (우측)	
	UPROPERTY(EditAnywhere, Category="ShoulderSwitchCamera")
	float RightShoulderOffset = 40.0f;

	//shoulder Switch 설정 오프세 값 (좌측)	
	UPROPERTY(EditAnywhere, Category = "ShoulderSwitchCamera")
	float LeftShoulderOffset = -80.0f;
	
	// 오프셋 적용시 오른쪽 어깨인지 확인하는 변수
	bool bIsRightShoulder = true; 

#pragam endrigon 
//======================================================================================================================


*/