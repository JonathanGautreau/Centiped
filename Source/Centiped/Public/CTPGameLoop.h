// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CtpPlayerPawn.h"
#include "UObject/Object.h"
#include "CTPGameLoop.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACtpGameLoop : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACtpGameLoop();
	
	void GenerateMushrooms();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(Category="PlayerPawn", EditAnywhere)
	TArray<FIntPoint> AvailableCells;

	void GenerateAvailableCells(const ACtpGameMode* GameMode);
	void SpawnMushrooms(UWorld* World, const ACtpGameMode* GameMode, int NumberOfMushrooms, int RowMin, int RowMax);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
