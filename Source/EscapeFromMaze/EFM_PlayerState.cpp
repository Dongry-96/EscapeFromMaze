#include "EFM_PlayerState.h"
#include "EFM_PlayerController.h"
#include "Net/UnrealNetwork.h"

void AEFM_PlayerState::SetPlayerNickname(const FString& NewName)
{
	if (HasAuthority())
	{
		PlayerNickname = NewName;
	}
}

void AEFM_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEFM_PlayerState, PlayerNickname);
}
