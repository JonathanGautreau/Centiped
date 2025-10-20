// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPGameLoop.h"

#include "CTPCentiNode.h"
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

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			GenerateMushrooms(World, GameMode);
			GenerateCentipede(World, SpawnParams, GameMode);
		}
	}
}

// Called every frame
void ACtpGameLoop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACtpGameLoop::GenerateMushrooms(UWorld* World, const ACtpGameMode* GameMode)
{
	GenerateAvailableCells(GameMode);
	// A lot of mushrooms
	SpawnMushrooms(World, GameMode, 25, 1, FMath::RoundToInt(GameMode->Rows * 0.85f));
	// Some mushrooms
	// SpawnMushrooms(World, GameMode, 6, FMath::RoundToInt(GameMode->Rows * 0.7f) + 1, FMath::RoundToInt(GameMode->Rows * 0.85f));
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
		int32 NumberOfDeletedCells = AvailableCells.Remove(FIntPoint(Row, Col));
		RemoveCellNeighbors(Col, Row, NumberOfDeletedCells);
		
		// Don't spawn mushrooms near the player
		FVector PlayerSquare = FVector(
			FMath::RoundToInt(GameMode->Columns * 0.4f),
			FMath::RoundToInt(GameMode->Columns * 0.6f),
			FMath::RoundToInt(GameMode->Rows * 0.7f));
		
		if (!(Col > PlayerSquare.X && Col < PlayerSquare.Y && Row > PlayerSquare.Z) && NumberOfDeletedCells > 0)
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

void ACtpGameLoop::RemoveCellNeighbors(const int Col, const int Row, int32 NumberOfDeletedCells)
{
	if (NumberOfDeletedCells > 0)
	{
		AvailableCells.Remove(FIntPoint(Row-1, Col-1));
		AvailableCells.Remove(FIntPoint(Row-1, Col));
		AvailableCells.Remove(FIntPoint(Row-1, Col+1));
		AvailableCells.Remove(FIntPoint(Row, Col-1));
		AvailableCells.Remove(FIntPoint(Row, Col+1));
		AvailableCells.Remove(FIntPoint(Row+1, Col-1));
		AvailableCells.Remove(FIntPoint(Row+1, Col));
		AvailableCells.Remove(FIntPoint(Row+1, Col+1));
	}
}

void ACtpGameLoop::GenerateCentipede(UWorld* World, const FActorSpawnParameters& SpawnParams, const ACtpGameMode* GameMode) const
{
	ACTPCentiNode* Prev = nullptr;
	
	for (int i = 0; i < CentiSize ; ++i )
	{
		ACTPCentiNode* Curr = World->SpawnActor<ACTPCentiNode>(SpawnParams);
				
		Curr-> PrevNode = Prev;
		Curr-> DefaultVector = FVector2D(2000,2000);
		Curr-> HitSwitch = FVector2D(2000,2000); ;//GameMode->Bounds.Max.X - Curr->MeshScale.X * 100 * 0.5, GameMode->Bounds.Max.Y - Curr->MeshScale.Y * 100 * 0.5
		
		if (Prev)
		{
			Prev->NextNode = Curr;
			Curr->SetActorLocation(FVector(0, Prev->GetActorLocation().Y + Curr->MeshScale.X * 100, Prev->GetActorLocation().Z));
		}
		else
		{
			Curr->SetActorLocation(FVector(0, 0, GameMode->Bounds.Max.Y - Curr->MeshScale.Y * 100 * 0.5));
		}
		Prev = Curr;
	}
}
