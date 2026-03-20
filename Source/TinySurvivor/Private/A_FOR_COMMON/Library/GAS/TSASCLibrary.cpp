// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/GAS/TSASCLibrary.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSCharacter.h"

UAbilitySystemComponent* UTSASCLibrary::GetASCFromComp(AActor* InOwner)
{
	if (!IsValid(InOwner)) return nullptr;
	
	IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(InOwner);
	if (!OwnerAbilitySystemInterface) return nullptr;
	
	return OwnerAbilitySystemInterface->GetAbilitySystemComponent();
}
