#include "MazeGenerator.h"
#include "MazeCell.h"
#include "EFM_GameState.h"
#include "EscapeFromMazeGameMode.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	TArray<TArray<FMazeCell>> MazeData;
}

void AMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		InitMazeData();

		int32 StartX = FMath::RandRange(0, MazeWidth - 1);
		int32 StartY = FMath::RandRange(0, MazeHeight - 1);
		GenerateMaze(StartX, StartY, 0);

		SpawnMazeActors();
		SpawnGoalActor();

		FVector MazeOrigin = GetActorLocation();
		FVector StartLocation = MazeOrigin + FVector(StartX * CellSize + 0.5f * CellSize, StartY * CellSize + 0.5f * CellSize, 100.f);

		if (AEFM_GameState* GameState = GetWorld()->GetGameState<AEFM_GameState>())
		{
			GameState->StartLocation = StartLocation;
		}
	}
}

void AMazeGenerator::InitMazeData()
{
	MazeData.Empty();
	MazeData.SetNum(MazeHeight);

	for (int32 Y = 0; Y < MazeHeight; ++Y)
	{
		MazeData[Y].SetNum(MazeWidth);

		for (int32 X = 0; X < MazeWidth; ++X)
		{
			MazeData[Y][X] = FMazeCell();
		}
	}
}

void AMazeGenerator::GenerateMaze(int32 X, int32 Y, int32 Depth)
{
	MazeData[Y][X].bVisited = true;

	if (Depth > MaxDepth)
	{
		MaxDepth = Depth;
		GoalPoint = FIntPoint(X, Y);
	}

	TArray<FIntPoint> Neighbors = GetUnvisitedNeighbors(X, Y);

	while (Neighbors.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, Neighbors.Num() - 1);
		FIntPoint Next = Neighbors[Index];

		RemoveWall(FIntPoint(X, Y), Next);
		GenerateMaze(Next.X, Next.Y, Depth + 1);

		Neighbors = GetUnvisitedNeighbors(X, Y);
	}
}

TArray<FIntPoint> AMazeGenerator::GetUnvisitedNeighbors(int32 X, int32 Y)
{
	TArray<FIntPoint> Result;

	const TArray<FIntPoint> Dirs = 
	{
		{0, -1}, {0, 1}, {-1, 0}, {1, 0}
	};

	for (const FIntPoint& D : Dirs)
	{
		int32 NX = X + D.X;
		int32 NY = Y + D.Y;

		if (NX >= 0 && NX < MazeWidth && NY >= 0 && NY < MazeHeight)
		{
			if (!MazeData[NY][NX].bVisited)
			{
				Result.Add({ NX, NY });
			}
		}
	}

	return Result;
}

void AMazeGenerator::RemoveWall(FIntPoint Current, FIntPoint Next)
{
	int32 DX = Next.X - Current.X;
	int32 DY = Next.Y - Current.Y;

	FMazeCell& CurrentCell = MazeData[Current.Y][Current.X];
	FMazeCell& NextCell = MazeData[Next.Y][Next.X];

	if (DX == 1)
	{
		CurrentCell.bWallRight = false;
		NextCell.bWallLeft = false;
	}
	else if (DX == -1)
	{
		CurrentCell.bWallLeft = false;
		NextCell.bWallRight = false;
	}
	else if (DY == 1)
	{
		CurrentCell.bWallTop = false;
		NextCell.bWallBottom = false;
	}
	else if (DY == -1)
	{
		CurrentCell.bWallBottom = false;
		NextCell.bWallTop = false;
	}
}

void AMazeGenerator::SpawnMazeActors()
{
	if (!HasAuthority() || !WallActorClass) return;

	FVector MazeOrigin = GetActorLocation();

	// 외곽 Top/Bottom
	for (int32 X = 0; X < MazeWidth; ++X)
	{
		FVector TopPos = MazeOrigin + FVector(X * CellSize + 0.5f * CellSize, MazeHeight * CellSize, 0.f);
		GetWorld()->SpawnActor<AActor>(WallActorClass, TopPos, FRotator(0.f, 0.f, 0.f));

		FVector BottomPos = MazeOrigin + FVector(X * CellSize + 0.5f * CellSize, 0.f, 0.f);
		GetWorld()->SpawnActor<AActor>(WallActorClass, BottomPos, FRotator(0.f, 0.f, 0.f));
	}

	// 외곽 Left/Right
	for (int32 Y = 0; Y < MazeHeight; ++Y)
	{
		FVector LeftPos = MazeOrigin + FVector(0.f, Y * CellSize + 0.5f * CellSize, 0.f);
		GetWorld()->SpawnActor<AActor>(WallActorClass, LeftPos, FRotator(0.f, 90.f, 0.f));

		FVector RightPos = MazeOrigin + FVector(MazeWidth * CellSize, Y * CellSize + 0.5f * CellSize, 0.f);
		GetWorld()->SpawnActor<AActor>(WallActorClass, RightPos, FRotator(0.f, 90.f, 0.f));
	}

	// 셀 내부 벽
	for (int32 Y = 0; Y < MazeHeight; ++Y)
	{
		for (int32 X = 0; X < MazeWidth; ++X)
		{
			const FMazeCell& Cell = MazeData[Y][X];
			FVector Base = MazeOrigin + FVector(X * CellSize + 0.5f * CellSize, Y * CellSize + 0.5f * CellSize, 0.f);

			if (Cell.bWallRight && X < MazeWidth - 1)
			{
				FVector Pos = Base + FVector(CellSize * 0.5f, 0.f, 0.f);
				GetWorld()->SpawnActor<AActor>(WallActorClass, Pos, FRotator(0.f, 90.f, 0.f));
			}

			if (Cell.bWallBottom && Y > 0)
			{
				FVector Pos = Base + FVector(0.f, -CellSize * 0.5f, 0.f);
				GetWorld()->SpawnActor<AActor>(WallActorClass, Pos, FRotator(0.f, 0.f, 0.f));
			}
		}
	}
}

void AMazeGenerator::SpawnGoalActor()
{
	if (!HasAuthority() || !GoalActorClass) return;

	FVector MazeOrigin = GetActorLocation();
	
	FVector GoalLocation = MazeOrigin + FVector(
		GoalPoint.X * CellSize + 0.5f * CellSize,
		GoalPoint.Y * CellSize + 0.5 * CellSize,
		10.0f);

	GetWorld()->SpawnActor<AActor>(GoalActorClass, GoalLocation, FRotator::ZeroRotator);
}


