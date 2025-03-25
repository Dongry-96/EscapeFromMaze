#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EFM_PlayerState.generated.h"

UCLASS()
class ESCAPEFROMMAZE_API AEFM_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated)
	FString PlayerNickname;
	
	void SetPlayerNickname(const FString& NewName);
	FString GetPlayerNickname() const { return PlayerNickname; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
