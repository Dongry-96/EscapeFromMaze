#include "GoalPoint.h"
#include "EscapeFromMazeCharacter.h"
#include "EscapeFromMazeGameMode.h"
#include "Components/SphereComponent.h"

AGoalPoint::AGoalPoint()
{
	bReplicates = true;

	GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoalMesh"));
	SetRootComponent(GoalMesh);
	GoalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapTrigger"));
	OverlapTrigger->SetupAttachment(RootComponent);

	bIsTriggered = false;
}

void AGoalPoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!HasAuthority()) return;
	if (bIsTriggered) return;

	bIsTriggered = true;

	if (AEscapeFromMazeCharacter* Player = Cast<AEscapeFromMazeCharacter>(OtherActor))
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()))
		{
			if (AEscapeFromMazeGameMode* GameMode = GetWorld()->GetAuthGameMode<AEscapeFromMazeGameMode>())
			{
				GameMode->EndGame(true, PlayerController);
			}
		}
	}
}

