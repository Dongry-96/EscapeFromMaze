#pragma once

#include "CoreMinimal.h"
#include "MazeCell.generated.h"

USTRUCT(BlueprintType)
struct FMazeCell
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bVisited;
	UPROPERTY()
	bool bWallTop;
	UPROPERTY()
	bool bWallBottom;
	UPROPERTY()
	bool bWallLeft;
	UPROPERTY()
	bool bWallRight;

	FMazeCell()
		: bVisited(false), bWallTop(true), bWallBottom(true),
		bWallLeft(true), bWallRight(true) {
	}
};
