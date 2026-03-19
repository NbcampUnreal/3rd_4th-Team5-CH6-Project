// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RYPlayerComponentInterface.generated.h"

class UCameraComponent;

// This class does not need to be modified.
UINTERFACE()
class URYPlayerComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RYGAMEPLAYINTERFACE_API IRYPlayerComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 카메라 getter
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RY | PlayerInterface")
	UCameraComponent* GetCameraComponent();
};
