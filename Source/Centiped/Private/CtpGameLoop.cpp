// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpGameLoop.h"
#include "CTPCentiNode.h"
#include "CtpHud.h"
#include "CtpMushroom.h"
#include "EngineUtils.h"
#include "Centiped/Public/CtpGameMode.h"
#include "CTPFlea.h"
#include "CtpLog.h"
#include "CTPScorpion.h"
#include "CTPSpider.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "CTPScoreSystem.h"
#include "Engine/Engine.h"


void UCtpGameLoop::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			if (UWorld* World = GetWorld())
			{
				if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = GameMode;
					
					GenerateMushrooms(World, GameMode);
					GenerateCentipede(World, SpawnParams, GameMode);
				}
			}
		});
	}
}

void UCtpGameLoop::GenerateMushrooms(UWorld* World, ACtpGameMode* GameMode)
{
	GenerateAvailableCells(GameMode);
	SpawnMushrooms(World, GameMode, InitialNumberOfMushrooms, 1, FMath::RoundToInt(GameMode->Rows * 0.85f));
}

void UCtpGameLoop::SpawnMushrooms(UWorld* World, ACtpGameMode* GameMode, int MushroomsCount, int RowMin, int RowMax)
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

int UCtpGameLoop::CountMushroomInPlayerZone()
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

void UCtpGameLoop::GenerateAvailableCells(ACtpGameMode* GameMode)
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

void UCtpGameLoop::RemoveCellNeighbors(int Col, int Row, int32 NumberOfDeletedCells)
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

void UCtpGameLoop::GenerateCentipede(UWorld* World, FActorSpawnParameters& SpawnParams, ACtpGameMode* GameMode)
{
	ACTPCentiNode* Prev = nullptr;
	
	for (int i = 0; i < CentipedeSize ; ++i )
	{
		ACTPCentiNode* Curr = World->SpawnActor<ACTPCentiNode>(SpawnParams);

		Curr-> PrevNode = Prev;
		
		if (Prev)
		{
			Curr->HitSwitches.EmplaceAt(0, FVector(0.f, GameMode->Bounds.Min.X + Curr->MeshScale.Y * 100 * 0.5,GameMode->Bounds.Max.Y - Curr->MeshScale.Y * 100 * 0.5));
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

void UCtpGameLoop::CheckFleaGeneration()
{
	if (GetSpawnedMushroomsCount() <= FMath::FloorToInt(InitialNumberOfMushrooms * .5f) && !bIsFlea)
	{
		GenerateFlea();
	}
	else if (GetSpawnedMushroomsCount() > FMath::FloorToInt(InitialNumberOfMushrooms * .5f))
	{
		bIsFlea = false;
	}
}

void UCtpGameLoop::GenerateFlea()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
 			bIsFlea = true;
			ACTPFlea* Flea = World->SpawnActor<ACTPFlea>(ACTPFlea::StaticClass());
			float SpawnOnY = FMath::RandRange(GameMode->Bounds.Min.X + Flea->MeshScale.X * 100,GameMode->Bounds.Max.X - Flea->MeshScale.X * 100);
			Flea->SetActorLocation(FVector(0,SpawnOnY,GameMode->Bounds.Max.Y) - Flea->MeshScale.Y * 100);
			Flea->HitSwitch = FVector2D(SpawnOnY,GameMode->Bounds.Max.Y - Flea->MeshScale.Y * 200 - Flea->VerticalOffset);
		}
	}
}

void UCtpGameLoop::CheckScorpionGeneration()
{
	if (GetSpawnedMushroomsCount() <= FMath::FloorToInt(InitialNumberOfMushrooms * .65f) && !bIsScorpion)
	{
		GenerateScorpion();
	}
	else if (GetSpawnedMushroomsCount() > FMath::FloorToInt(InitialNumberOfMushrooms * .65f))
	{
		bIsScorpion = false;
	}
}

void UCtpGameLoop::GenerateScorpion()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			bIsScorpion = true;
			ACTPScorpion* Scorpion = World->SpawnActor<ACTPScorpion>(ACTPScorpion::StaticClass());
			if (!Scorpion) return;
			float SpawnOnZ = FMath::RandRange( GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5),GameMode->Bounds.Max.Y);
			Scorpion->bIsLeftDirection = FMath::RandBool();
			if (Scorpion->bIsLeftDirection) Scorpion->SetActorLocation(FVector(0,GameMode->Bounds.Max.X,SpawnOnZ));
			else Scorpion->SetActorLocation(FVector(0,GameMode->Bounds.Min.X,SpawnOnZ));
		}
	}
}

void UCtpGameLoop::CheckSpiderGeneration()
{
	if (CountMushroomInPlayerZone() <= FMath::FloorToInt(InitialNumberOfMushrooms * .15f) && !bIsSpider)
	{
 		GenerateSpider();
	}
	else if (CountMushroomInPlayerZone() > FMath::FloorToInt(InitialNumberOfMushrooms * .15f))
	{
		bIsSpider = false;
	}
}

void UCtpGameLoop::GenerateSpider()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			bIsSpider = true;
			ACTPSpider* Spider = World->SpawnActor<ACTPSpider>(ACTPSpider::StaticClass());
  			float SpawnOnZ = FMath::RandRange(
  				GameMode->Bounds.Min.Y / 1.5f,
  				GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5));
			int randomYPosition = FMath::RandBool() ? 1 : -1;
			// Spawn on right, going left
			if (FMath::RandBool())
			{
				Spider->Direction = FVector2D(-1, randomYPosition);
				Spider->SetActorLocation(FVector(0, GameMode->Bounds.Max.X - Spider->MeshScale.X * 100 * 0.5, SpawnOnZ));
			}
			// Spawn on left, going right
			else
			{
				Spider->Direction = FVector2D(1, randomYPosition);
				Spider->SetActorLocation(FVector(0, GameMode->Bounds.Min.X + Spider->MeshScale.X * 100 * 0.5, SpawnOnZ));
			}
		}
	}
}

int UCtpGameLoop::GetSpawnedMushroomsCount() const
{
	return SpawnedMushroomsCount;
}

void UCtpGameLoop::SetSpawnedMushroomsCount(const int Count)
{
	SpawnedMushroomsCount = Count;
}

void UCtpGameLoop::ResetRound()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	GetWorld()->GetTimerManager().SetTimer(
		ResetTimerHandle,
		this,
		&UCtpGameLoop::OnResetRoundComplete,
		.3f,
		false
	);
}

void UCtpGameLoop::OnResetRoundComplete()
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
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GameMode;
			
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
		bIsFlea = false;
		bIsScorpion = false;
		bIsSpider = false;
		
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}

void UCtpGameLoop::GameOver()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .1f);
	
	GetWorld()->GetTimerManager().SetTimer(
		GameOverTimerHandle,
		this,
		&UCtpGameLoop::OnGameOverComplete,
		.3f,
		false
	);
}

void UCtpGameLoop::OnGameOverComplete()
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

void UCtpGameLoop::RestartGame()
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
		bIsFlea = false;
		bIsScorpion = false;
		bIsSpider = false;
	
		// Generate a new centipede and new mushrooms
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GameMode;
			
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
			if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
				ScoreSystem->ResetScore();
		}
		
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}
