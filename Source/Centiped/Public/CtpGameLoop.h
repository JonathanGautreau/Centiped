// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CtpMushroom.h"
#include "CtpGameLoop.generated.h"

// Forward declaration avoids circular dependencies 
class ACtpGameMode;


UCLASS()
class CENTIPED_API UCtpGameLoop : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(category = "GameLoop", EditAnywhere)
	int SpawnedMushroomsCount;

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void GenerateMushrooms(UWorld* World, ACtpGameMode* GameMode);
	void GenerateAvailableCells(ACtpGameMode* GameMode);
	void RemoveCellNeighbors(int Col, int Row, int32 NumberOfDeletedCells);
	void SpawnMushrooms(UWorld* World, ACtpGameMode* GameMode, int MushroomsCount, int RowMin, int RowMax);
	int CountMushroomInPlayerZone();

	void GenerateFlea();
	void GenerateScorpion();
	void GenerateSpider();

	UFUNCTION()
	void OnResetRoundComplete();
	UFUNCTION()
	void OnGameOverComplete();
	
	UPROPERTY(Category="GameLoop", EditAnywhere)
	TArray<FIntPoint> AvailableCells;

	UPROPERTY(Category = "GameLoop", EditAnywhere)
	FTimerHandle ResetTimerHandle;
	UPROPERTY(Category = "GameLoop", EditAnywhere)
	FTimerHandle GameOverTimerHandle;

public:
	UFUNCTION()
	void ResetRound();
	UFUNCTION()
	void RestartGame();
	void GameOver();
	
	void GenerateCentipede(UWorld* World, FActorSpawnParameters& SpawnParams, ACtpGameMode* GameMode);
	void CheckFleaGeneration();
	void CheckScorpionGeneration();
	void CheckSpiderGeneration();

	int GetSpawnedMushroomsCount() const;
	void SetSpawnedMushroomsCount(int Count);
	
	UPROPERTY(Category = "GameLoop", EditAnywhere)
	int CentipedeSize = 10;
	
	UPROPERTY(category = "GameLoop", EditAnywhere)
	int InitialNumberOfMushrooms = 20;
	
	UPROPERTY(category = "GameLoop", EditAnywhere)
	bool bIsFlea;

	UPROPERTY(Category = "GameLoop", EditAnywhere)
	bool bIsScorpion;

	UPROPERTY(category = "GameLoop", EditAnywhere)
	bool bIsSpider;

	UPROPERTY(category = "GameLoop", EditAnywhere)
	TArray<ACtpMushroom*> PoisonedMush;
};
