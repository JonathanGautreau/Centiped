// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPGameLoop.h"
#include "CTPCentiNode.h"
#include "CtpMushroom.h"
#include "Centiped/Public/CTPLog.h"
#include "Centiped/Public/CtpGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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
		
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
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

void ACtpGameLoop::GenerateMushrooms(UWorld* World, ACtpGameMode* GameMode)
{
	GenerateAvailableCells(GameMode);
	// A lot of mushrooms
	SpawnMushrooms(World, GameMode, 25, 1, FMath::RoundToInt(GameMode->Rows * 0.85f));
	// Some mushrooms
	// SpawnMushrooms(World, GameMode, 6, FMath::RoundToInt(GameMode->Rows * 0.7f) + 1, FMath::RoundToInt(GameMode->Rows * 0.85f));
}

void ACtpGameLoop::SpawnMushrooms(UWorld* World, ACtpGameMode* GameMode, int NumberOfMushrooms, int RowMin, int RowMax)
{
	int SpawnedMushrooms = 0;
	
	while (SpawnedMushrooms < NumberOfMushrooms && AvailableCells.Num() > 0)
	{
		// Chose location
		int Col = FMath::RandRange(0, GameMode->Columns - 1);
		int Row = FMath::RandRange(RowMin, RowMax);
		
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
			int x = round(GameMode->Bounds.Min.X) + round(Mushroom->MeshScale.X * 100 * Col) + round(Mushroom->MeshScale.X * 0.5 * 100);
			int y = round(GameMode->Bounds.Max.Y) - round(Mushroom->MeshScale.Y * 100 * Row) - round(Mushroom->MeshScale.Y * 0.5 * 100);
			Mushroom->InitializePosition(FVector(0, x, y));

			SpawnedMushrooms++;
		}
	}
}

void ACtpGameLoop::GenerateAvailableCells(ACtpGameMode* GameMode)
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

void ACtpGameLoop::RemoveCellNeighbors(int Col, int Row, int32 NumberOfDeletedCells)
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

void ACtpGameLoop::GenerateCentipede(UWorld* World, FActorSpawnParameters& SpawnParams, ACtpGameMode* GameMode)
{
	ACTPCentiNode* Prev = nullptr;
	
	for (int i = 0; i < CentipedeSize ; ++i )
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

void ACtpGameLoop::OnResetRoundComplete()
{
	// Rétablir la vitesse normale du temps
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	// Reset logique ici
	UE_LOG(LogTemp, Warning, TEXT("Round reset"));
}

void ACtpGameLoop::ResetRound()
{
	/**
	 * TODO
	 * Remove current centipede
	 * Reset player position
	 * Create new centipede
	 *
	 * Don't reset mushrooms
	 * Don't reset player life and score
	 */
	
	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		// Score mushrooms
		if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
			ScoreSystem->ScoreMushrooms();
	}
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	// Lancer un timer "normal"
	GetWorld()->GetTimerManager().SetTimer(
		ResetTimerHandle,
		this,
		&ACtpGameLoop::OnResetRoundComplete,
		.5f,
		false
	);
}

void ACtpGameLoop::GameOver()
{
	/**
	 * TODO
	 * Remove current centipede
	 * Remove current mushrooms
	 * Reset player position
	 * Reset player life and score
	 * Create new centipede
	 * Create new mushrooms
	 */
}