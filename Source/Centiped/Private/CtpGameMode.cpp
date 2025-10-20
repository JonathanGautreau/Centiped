// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpGameMode.h"
#include "CTPGameLoop.h"
#include "CtpHud.h"
#include "CTPLog.h"
#include "CTPScoreSystem.h"
#include "Centiped/Public/CtpPlayerController.h"
#include "Centiped/Public/CtpPlayerPawn.h"
#include "EngineUtils.h"

ACtpGameMode::ACtpGameMode()
{
	PlayerControllerClass = ACtpPlayerController::StaticClass();
	DefaultPawnClass = ACtpPlayerPawn::StaticClass();
	HUDClass = ACtpHud::StaticClass();
}

void ACtpGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	if (UWorld* World = GetWorld())
	{
		if (!GameLoop)
		{
			GameLoop = World->SpawnActor<ACtpGameLoop>(ACtpGameLoop::StaticClass());
			if (!GameLoop)
			{
				UE_LOG(LogTemp, Error, TEXT("GameLoop wasn't instantiated correctly"));
			}
		}
		
		if (!ScoreSystem)
		{
			ScoreSystem = World->SpawnActor<ACTPScoreSystem>(ACTPScoreSystem::StaticClass());
			if (!ScoreSystem)
			{
				UE_LOG(LogTemp, Error, TEXT("ScoreSystem wasn't instantiated correctly"));
			}
			else
			{
				ScoreSystem->ResetScore();
			}
		}
	}
}
