// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CtpMushroom.h"
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

	void GenerateMushrooms(UWorld* World, ACtpGameMode* GameMode);
	void RemoveCellNeighbors(int Col, int Row, int32 NumberOfDeletedCells);
	void GenerateAvailableCells(ACtpGameMode* GameMode);
	void SpawnMushrooms(UWorld* World, ACtpGameMode* GameMode, int NumberOfMushrooms, int RowMin, int RowMax);

	UFUNCTION()
	void OnResetRoundComplete();
	UFUNCTION()
	void OnGameOverComplete();

	UPROPERTY(Category = "GameLoop", EditAnywhere)
	FTimerHandle ResetTimerHandle;
	UPROPERTY(Category = "GameLoop", EditAnywhere)
	FTimerHandle GameOverTimerHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void ResetRound();
	UFUNCTION()
	void RestartGame();
	
	void GameOver();
	void GenerateCentipede(UWorld* World, FActorSpawnParameters& SpawnParams, ACtpGameMode* GameMode);
	
	UPROPERTY(Category = "GameLoop", EditAnywhere)
	int CentipedeSize = 10;

	UPROPERTY(category = "GameLoop", EditAnywhere)
	int SpawnedMushroomsCount;

	int GetSpawnedMushrooms() const;
	void SetSpawnedMushroomsCount(int Count);

	UPROPERTY(category = "GameLoop", EditAnywhere)
	bool isFlea;

	UPROPERTY(Category = "GameLoop", EditAnywhere)
	bool IsScorpion;

	UPROPERTY(category = "GameLoop", EditAnywhere)
	bool IsSpider;

	UPROPERTY(category = "GameLoop", EditAnywhere)
	TArray<ACtpMushroom*> PoisonedMush;
};
