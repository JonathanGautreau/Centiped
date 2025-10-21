// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTPGameLoop.h"
#include "CTPScoreSystem.h"
#include "GameFramework/GameModeBase.h"
#include "CtpGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACtpGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACtpGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Game")
	int Height = 1920;
	
	UPROPERTY(VisibleAnywhere, Category="Game")
	int Width = 1600;

	UPROPERTY(VisibleAnywhere, Category="Game")
	FVector2D SquareSize = FVector2D(80, 80);

	UPROPERTY(VisibleAnywhere, Category="Game")
	int Rows = static_cast<int>(Height / SquareSize.Y);
	
	UPROPERTY(VisibleAnywhere, Category="Game")
	int Columns = static_cast<int>(Width / SquareSize.X);
	
	UPROPERTY(VisibleAnywhere, Category="Game")
	FBox2D Bounds = FBox2D(FVector2D(-(Width / 2), -(Height / 2)), FVector2D(Width / 2, Height / 2));

	ACtpGameLoop* GetGameLoop() const { return GameLoop; };
	ACTPScoreSystem* GetScoreSystem() const { return ScoreSystem; };

protected:
	UPROPERTY(VisibleAnywhere, Category="Game")
	ACtpGameLoop* GameLoop;
	
	UPROPERTY(VisibleAnywhere, Category="Game")
	ACTPScoreSystem* ScoreSystem;
};
