// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpGameMode.h"
#include "CtpGameLoop.h"
#include "CtpHud.h"
#include "CTPLog.h"
#include "CTPScoreSystem.h"
#include "Centiped/Public/CtpPlayerController.h"
#include "Centiped/Public/CtpPlayerPawn.h"

ACtpGameMode::ACtpGameMode()
{
	PlayerControllerClass = ACtpPlayerController::StaticClass();
	DefaultPawnClass = ACtpPlayerPawn::StaticClass();
	HUDClass = ACtpHud::StaticClass();
}

void ACtpGameMode::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	if (UWorld* World = GetWorld())
	{
		GameLoop = World->GetSubsystem<UCtpGameLoop>();
		if (!GameLoop)
		{
			UE_LOG(LogCentiped, Error, TEXT("GameLoop wasn't instantiated correctly"));
		}
		
		ScoreSystem = World->GetSubsystem<UCTPScoreSystem>();
		if (ScoreSystem)
		{
			ScoreSystem->ResetScore();
		}
		else
		{
			UE_LOG(LogCentiped, Error, TEXT("ScoreSubsystem wasn't instantiated correctly"));
		}
	}
}
