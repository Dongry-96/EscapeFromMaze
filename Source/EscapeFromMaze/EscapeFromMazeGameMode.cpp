#include "EscapeFromMazeGameMode.h"
#include "EscapeFromMazeCharacter.h"
#include "EFM_PlayerController.h"
#include "EFM_PlayerState.h"
#include "EFM_GameState.h"
#include "MazeGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "GenericPlatform/GenericPlatformMisc.h"

AEscapeFromMazeGameMode::AEscapeFromMazeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bUseSeamlessTravel = false;
}

void AEscapeFromMazeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FTimerHandle DelayHandle;
	FTimerDelegate DelayDelegate;
	DelayDelegate.BindUObject(this, &AEscapeFromMazeGameMode::AssignStartLocation, NewPlayer);
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, DelayDelegate, 0.3f, false);
}

void AEscapeFromMazeGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AEscapeFromMazeGameMode::AssignStartLocation(APlayerController* NewPlayer)
{
	static int32 PlayerIndex = 0;

	if (AEFM_GameState* MazeGameState = GetGameState<AEFM_GameState>())
	{
		const FVector BaseLocation = MazeGameState->StartLocation;
		const float Radius = 30.0f;
		const float AngleStep = 45.0f;

		float Angle = FMath::DegreesToRadians(AngleStep * PlayerIndex);
		FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Radius;
		FVector FinalLocation = BaseLocation + Offset;
		FinalLocation.Z = 50.0f;

		if (AEscapeFromMazeCharacter* Player = Cast<AEscapeFromMazeCharacter>(NewPlayer->GetPawn()))
		{
			Player->SetActorLocation(FinalLocation);
		}
		PlayerIndex++;
	}
}

void AEscapeFromMazeGameMode::EndGame(bool bIsSuccess, APlayerController* Player)
{
	FString WinnerName = TEXT("?");

	if (AEFM_PlayerState* PlayerState = Player ? Cast<AEFM_PlayerState>(Player->PlayerState) : nullptr)
	{
		WinnerName = PlayerState->GetPlayerNickname();
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AEFM_PlayerController* PlayerController = Cast<AEFM_PlayerController>(*It))
		{
			PlayerController->Client_ReachedGoal(WinnerName);
		}
	}

	FTimerHandle ExitGameTimerHandle;
	GetWorldTimerManager().SetTimer(ExitGameTimerHandle, this, &AEscapeFromMazeGameMode::ExitGame, 3.0f, false);
}

void AEscapeFromMazeGameMode::ExitGame()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AEFM_PlayerController* PC = Cast<AEFM_PlayerController>(*It))
		{
			PC->Client_ExitGame();
		}
	}

	FGenericPlatformMisc::RequestExit(false);
}