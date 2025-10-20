// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTPGameLoop.generated.h"

// Forward declaration avoids circular dependencies 
class ACtpGameMode;


UCLASS()
class CENTIPED_API ACtpGameLoop : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACtpGameLoop();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(Category="GameLoop", EditAnywhere)
	TArray<FIntPoint> AvailableCells;

	void GenerateMushrooms(UWorld* World, const ACtpGameMode* GameMode);
	void RemoveCellNeighbors(int Col, int Row, int32 NumberOfDeletedCells);
	void GenerateAvailableCells(const ACtpGameMode* GameMode);
	void SpawnMushrooms(UWorld* World, const ACtpGameMode* GameMode, int NumberOfMushrooms, int RowMin, int RowMax);
	void GenerateCentipede(UWorld* World, const FActorSpawnParameters& SpawnParams, const ACtpGameMode* GameMode) const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ResetRound();
	void GameOver();

	UPROPERTY(Category = "Centipede", EditAnywhere)
	int CentiSize = 10;

	UPROPERTY(Category = "Centipede", EditAnywhere)
	FVector2D HeatDirection;

	UPROPERTY(category = "Centipede", EditAnywhere)
	float CentiSpeed;
};
