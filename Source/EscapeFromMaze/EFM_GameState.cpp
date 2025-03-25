#include "EFM_GameState.h"
#include "EscapeFromMazeGameMode.h"
#include "Net/UnrealNetwork.h"

void AEFM_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEFM_GameState, StartLocation);
}

