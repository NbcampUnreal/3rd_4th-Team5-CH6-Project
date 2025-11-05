#include "GameMode/TSGameMode.h"

#include "Character/TSCharacter.h"
#include "PlayerState/TSPlayerState.h"

ATSGameMode::ATSGameMode()
{
	PlayerStateClass = ATSPlayerState::StaticClass();
}

