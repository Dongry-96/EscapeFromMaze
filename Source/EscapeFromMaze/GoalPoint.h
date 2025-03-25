#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoalPoint.generated.h"

UCLASS()
class ESCAPEFROMMAZE_API AGoalPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AGoalPoint();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> GoalMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> OverlapTrigger;

private:
	bool bIsTriggered;
	
};
