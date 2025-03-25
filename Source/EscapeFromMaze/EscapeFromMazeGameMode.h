#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EscapeFromMazeGameMode.generated.h"

UCLASS(minimalapi)
class AEscapeFromMazeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEscapeFromMazeGameMode();

	void EndGame(bool bIsSuccess, APlayerController* Player);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;

private:
	void AssignStartLocation(APlayerController* NewPlayer);
	void ExitGame();
};



