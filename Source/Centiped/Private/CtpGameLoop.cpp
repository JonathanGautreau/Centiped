// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpGameLoop.h"

#include "BlendSpaceAnalysis.h"
#include "CTPCentiNode.h"
#include "CtpHud.h"
#include "CtpMushroom.h"
#include "EngineUtils.h"
#include "Centiped/Public/CTPLog.h"
#include "Centiped/Public/CtpGameMode.h"
#include "CTPFlea.h"
#include "CTPScorpion.h"
#include "CTPSpider.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ACtpGameLoop::ACtpGameLoop()
{
}

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

void ACtpGameLoop::GenerateMushrooms(UWorld* World, ACtpGameMode* GameMode)
{
	GenerateAvailableCells(GameMode);
	SpawnMushrooms(World, GameMode, InitialNumberOfMushrooms, 1, FMath::RoundToInt(GameMode->Rows * 0.85f));
}

void ACtpGameLoop::SpawnMushrooms(UWorld* World, ACtpGameMode* GameMode, int MushroomsCount, int RowMin, int RowMax)
{
	int SpawnedMushrooms = 0;
	
	while (SpawnedMushrooms < MushroomsCount && AvailableCells.Num() > 0)
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
	SetSpawnedMushroomsCount(SpawnedMushrooms);
}

int ACtpGameLoop::CountMushroomInPlayerZone()
{
	int CountInPlayerZone = 0;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (ACtpMushroom* Mushroom = Cast<ACtpMushroom>(*It))
		{
			if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
			{
				double PlayerVerticalLimit = GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f));
				if (Mushroom->GetActorLocation().Z < PlayerVerticalLimit)
					CountInPlayerZone++;
			}
		}
	}
	return CountInPlayerZone;
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

void ACtpGameLoop::CheckFleaGeneration()
{
	if (GetSpawnedMushroomsCount() <= FMath::FloorToInt(InitialNumberOfMushrooms * .5f) && !IsFlea)
	{
		GenerateFlea();
	}
	else if (GetSpawnedMushroomsCount() > FMath::FloorToInt(InitialNumberOfMushrooms * .5f))
	{
		IsFlea = false;
	}
}

void ACtpGameLoop::GenerateFlea()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
 			IsFlea = true;
			ACTPFlea* Flea = World->SpawnActor<ACTPFlea>(ACTPFlea::StaticClass());
			float SpawnOnY = FMath::RandRange(GameMode->Bounds.Min.X + Flea->MeshScale.X * 100,GameMode->Bounds.Max.X - Flea->MeshScale.X * 100);
			Flea->SetActorLocation(FVector(0,SpawnOnY,GameMode->Bounds.Max.Y) - Flea->MeshScale.Y * 100);
			Flea->HitSwitch = FVector2D(SpawnOnY,GameMode->Bounds.Max.Y - Flea->MeshScale.Y * 200 - Flea->VerticalOffset);
		}
	}
}

void ACtpGameLoop::CheckScorpionGeneration()
{
	if (GetSpawnedMushroomsCount() <= FMath::FloorToInt(InitialNumberOfMushrooms * .65f) && !IsScorpion)
	{
		GenerateScorpion();
	}
	else if (GetSpawnedMushroomsCount() > FMath::FloorToInt(InitialNumberOfMushrooms * .65f))
	{
		IsScorpion = false;
	}
}

void ACtpGameLoop::GenerateScorpion()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			IsScorpion = true;
			ACTPScorpion* Scorpion = World->SpawnActor<ACTPScorpion>(ACTPScorpion::StaticClass());
			float SpawnOnZ = FMath::RandRange( GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5),GameMode->Bounds.Max.Y);
			Scorpion->IsLeftDirection = FMath::RandBool();
			if (Scorpion->IsLeftDirection) Scorpion->SetActorLocation(FVector(0,GameMode->Bounds.Max.X,SpawnOnZ));
			else Scorpion->SetActorLocation(FVector(0,GameMode->Bounds.Min.X,SpawnOnZ));
		}
	}
}

void ACtpGameLoop::CheckSpiderGeneration()
{
	if (CountMushroomInPlayerZone() <= FMath::FloorToInt(InitialNumberOfMushrooms * .15f) && !IsSpider)
	{
 		GenerateSpider();
	}
	else if (CountMushroomInPlayerZone() > FMath::FloorToInt(InitialNumberOfMushrooms * .15f))
	{
		IsSpider = false;
	}
}

void ACtpGameLoop::GenerateSpider()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			IsSpider = true;
			ACTPSpider* Spider = World->SpawnActor<ACTPSpider>(ACTPSpider::StaticClass());
  			float SpawnOnZ = FMath::RandRange(GameMode->Bounds.Min.Y / 2.f, GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5));
			// Spawn on right, going left
			if (FMath::RandBool())
			{
				Spider->Direction = FVector2D(-1, -1);
				Spider->SetActorLocation(FVector(0, GameMode->Bounds.Max.X - Spider->MeshScale.X * 100 * 0.5, SpawnOnZ));
			}
			// Spawn on left, going right
			else
			{
				Spider->Direction = FVector2D(1, -1);
				Spider->SetActorLocation(FVector(0, GameMode->Bounds.Min.X + Spider->MeshScale.X * 100 * 0.5, SpawnOnZ));
			}
		}
	}
}

int ACtpGameLoop::GetSpawnedMushroomsCount() const
{
	return SpawnedMushroomsCount;
}

void ACtpGameLoop::SetSpawnedMushroomsCount(const int Count)
{
	SpawnedMushroomsCount = Count;
}

void ACtpGameLoop::ResetRound()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	GetWorld()->GetTimerManager().SetTimer(
		ResetTimerHandle,
		this,
		&ACtpGameLoop::OnResetRoundComplete,
		.3f,
		false
	);
}

void ACtpGameLoop::OnResetRoundComplete()
{
	if (UWorld* World = GetWorld())
	{
		// Destroy all Enemies and Bullets
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (Cast<ACTPEnemy>(*It))
				It->Destroy();
			if (Cast<ACtpBullet>(*It))
				It->Destroy();
			if (ACtpMushroom* Mushroom = Cast<ACtpMushroom>(*It))
				Mushroom->BecomeNormal();
		}
		
		// Reset all Poisoned Mushroom to normal ones
		PoisonedMush.Empty();
		
		// Generate a new Centipede
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			GenerateCentipede(World, SpawnParams, GameMode);
		}
		
		// Reset Player
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!PlayerController) return;
		ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(PlayerController->GetPawn());
		if (!Player) return;

		Player->bIsOverlappedByEnemy = false;
		Player->SetPlayerInitialPosition();

		// Reset booleans for enemy generations
		IsFlea = false;
		IsScorpion = false;
		IsSpider = false;
		
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}

void ACtpGameLoop::GameOver()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .1f);
	
	GetWorld()->GetTimerManager().SetTimer(
		GameOverTimerHandle,
		this,
		&ACtpGameLoop::OnGameOverComplete,
		.3f,
		false
	);
}

void ACtpGameLoop::OnGameOverComplete()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .0f);
	
	// Display Game Over text
	if (UWorld* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!PlayerController) return;
		ACtpHud* Hud = Cast<ACtpHud>(PlayerController->GetHUD());
		if (!Hud) return;

		Hud->ShowGameOverText(true);
	}
}

void ACtpGameLoop::RestartGame()
{
	if (UWorld* World = GetWorld())
	{
		// Destroy all Enemies and Bullets and Mushrooms
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (Cast<ACTPEnemy>(*It))
				It->Destroy();
			if (Cast<ACtpBullet>(*It))
				It->Destroy();
			if (Cast<ACtpMushroom>(*It))
				It->Destroy();
		}

		// Reset booleans for enemy generations
		IsFlea = false;
		IsScorpion = false;
		IsSpider = false;
	
		// Generate a new centipede and new mushrooms
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			GenerateMushrooms(World, GameMode);
			GenerateCentipede(World, SpawnParams, GameMode);
		}
		
		// Reset Player
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!PlayerController) return;
		ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(PlayerController->GetPawn());
		if (!Player) return;

		Player->bIsOverlappedByEnemy = false;
		Player->SetPlayerInitialPosition();
		Player->SetLife(3);

		// Reset score
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			// Score mushrooms
			if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
				ScoreSystem->ResetScore();
		}
		
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}
