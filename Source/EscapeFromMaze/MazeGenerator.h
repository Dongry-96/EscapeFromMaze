#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeGenerator.generated.h"

UCLASS()
class ESCAPEFROMMAZE_API AMazeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMazeGenerator() : 
		MazeWidth(21), MazeHeight(21), CellSize(200.0f), GoalPoint({ 0,0 }), MaxDepth(-1) {}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	TSubclassOf<AActor> WallActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	TSubclassOf<AActor> GoalActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	int32 MazeWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	int32 MazeHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	float CellSize;

private:
	void InitMazeData();
	void GenerateMaze(int32 X, int32 Y, int32 Depth);
	TArray<FIntPoint> GetUnvisitedNeighbors(int32 X, int32 Y);
	void RemoveWall(FIntPoint Current, FIntPoint Next);
	void SpawnMazeActors();
	void SpawnGoalActor();

	FIntPoint GoalPoint;
	int32 MaxDepth;
};
