// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPGameLoop.h"
#include "CtpMushroom.h"
#include "Centiped/Public/CTPLog.h"
#include "Centiped/Public/CtpGameMode.h"

// Sets default values
ACtpGameLoop::ACtpGameLoop()
{
}

// Called when the game starts or when spawned
void ACtpGameLoop::BeginPlay()
{
	Super::BeginPlay();

	GenerateMushrooms();
}

// Called every frame
void ACtpGameLoop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACtpGameLoop::GenerateMushrooms()
{
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			GenerateAvailableCells(GameMode);
			// A lot of mushrooms
			SpawnMushrooms(World, GameMode, 20, 0, GameMode->Rows - 20);
			// Some mushrooms
			SpawnMushrooms(World, GameMode, 8, GameMode->Rows - 21, GameMode->Rows - 6);
		}
	}
}

void ACtpGameLoop::SpawnMushrooms(UWorld* World, const ACtpGameMode* GameMode, int NumberOfMushrooms, int RowMin, int RowMax)
{
	int SpawnedMushrooms = 0;
	
	while (SpawnedMushrooms < NumberOfMushrooms && AvailableCells.Num() > 0)
	{
		// Chose location
		const int Col = FMath::RandRange(0, GameMode->Columns - 1);
		const int Row = FMath::RandRange(RowMin, RowMax);

		// Remove chosen location from AvailableCells array
		AvailableCells.Remove(FIntPoint(Row, Col));

		// Don't spawn mushrooms near for the player
		if (!(Col < 13 && Col > 7 && Row > 35))
		{
			// Create mushrooms
			ACtpMushroom* Mushroom = World->SpawnActor<ACtpMushroom>(ACtpMushroom::StaticClass());

			// Define position of the mushroom
			const int x = round(GameMode->Bounds.Min.X) + round(Mushroom->MeshScale.X * 100 * Col) + round(Mushroom->MeshScale.X * 0.5 * 100);
			const int y = round(GameMode->Bounds.Max.Y) - round(Mushroom->MeshScale.Y * 100 * Row) - round(Mushroom->MeshScale.Y * 0.5 * 100);
			Mushroom->InitializePosition(FVector(0, x, y));

			SpawnedMushrooms++;
		}
	}
}

void ACtpGameLoop::GenerateAvailableCells(const ACtpGameMode* GameMode)
{
	AvailableCells.Empty();
	
	for (int Row = 0; Row < GameMode->Rows; ++Row)
	{
		for (int32 Col = 0; Col < GameMode->Columns; ++Col)
		{
			AvailableCells.Add(FIntPoint(Row, Col));
		}
	}
}
