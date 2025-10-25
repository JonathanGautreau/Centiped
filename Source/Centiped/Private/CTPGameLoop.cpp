// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPGameLoop.h"
#include "CTPCentiNode.h"
#include "CtpHud.h"
#include "CtpMushroom.h"
#include "EngineUtils.h"
#include "Centiped/Public/CTPLog.h"
#include "Centiped/Public/CtpGameMode.h"
#include "CTPFlea.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ACtpGameLoop::ACtpGameLoop()
{
	PrimaryActorTick.bCanEverTick = true;
	
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
	if (GetSpawnedMushroomsCount() <= FMath::FloorToInt(InitialNumberOfMushrooms / 2.f) && !isFlea)
	{
		GenerateFlea();
	}
	else if (GetSpawnedMushroomsCount() > FMath::FloorToInt(InitialNumberOfMushrooms / 2.f))
	{
		isFlea = false;
	}
}

void ACtpGameLoop::GenerateFlea()
{
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
 			isFlea = true;
			ACTPFlea* Flea = World->SpawnActor<ACTPFlea>(ACTPFlea::StaticClass());
			float SpawnOnY = FMath::RandRange(GameMode->Bounds.Min.X + Flea->MeshScale.X * 100,GameMode->Bounds.Max.X - Flea->MeshScale.X * 100);
			Flea->SetActorLocation(FVector(0,SpawnOnY,GameMode->Bounds.Max.Y) - Flea->MeshScale.Y * 100);
			Flea->HitSwitch = FVector2D(SpawnOnY,GameMode->Bounds.Max.Y - Flea->MeshScale.Y * 200 - Flea->VerticalOffset);
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
	// GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,FString::Printf(TEXT("Set Spawned Mushrooms count %d "), SpawnedMushroomsCount));
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
		// Destroy all CentiNodes and Bullets
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (Cast<ACTPCentiNode>(*It))
				It->Destroy();
			if (Cast<ACtpBullet>(*It))
				It->Destroy();
		}
	
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
		// Destroy all CentiNodes and Bullets and Mushrooms
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (Cast<ACTPCentiNode>(*It))
				It->Destroy();
			if (Cast<ACtpBullet>(*It))
				It->Destroy();
			if (Cast<ACtpMushroom>(*It))
				It->Destroy();
		}
	
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
