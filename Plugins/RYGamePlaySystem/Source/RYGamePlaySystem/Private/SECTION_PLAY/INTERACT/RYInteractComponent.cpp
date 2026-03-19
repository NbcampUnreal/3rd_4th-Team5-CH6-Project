// All CopyRight From YulRyongGameStudio //


#include "SECTION_PLAY/INTERACT/RYInteractComponent.h"


URYInteractComponent::URYInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URYInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}


void URYInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

