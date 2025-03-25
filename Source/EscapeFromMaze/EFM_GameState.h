#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EFM_GameState.generated.h"

UCLASS()
class ESCAPEFROMMAZE_API AEFM_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere)
	FVector StartLocation;

};
