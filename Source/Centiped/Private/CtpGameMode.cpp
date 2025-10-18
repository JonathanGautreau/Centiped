// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpGameMode.h"

#include "CTPGameLoop.h"
#include "CTPLog.h"
#include "CTPScoreSystem.h"
#include "Centiped/Public/CtpPlayerController.h"
#include "Centiped/Public/CtpPlayerPawn.h"
#include "EngineUtils.h"

ACtpGameMode::ACtpGameMode()
{
	PlayerControllerClass = ACtpPlayerController::StaticClass();
	DefaultPawnClass = ACtpPlayerPawn::StaticClass();
}

void ACtpGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
	}
	
	if (UWorld* World = GetWorld())
	{
		ACtpGameLoop* GameLoop = World->SpawnActor<ACtpGameLoop>(ACtpGameLoop::StaticClass());
		if (!GameLoop)
		{
			UE_LOG(LogTemp, Error, TEXT("GameLoop wasn't instantiated correctly"));
		}
	}
	
	if (!ScoreSystem)
	{
		ScoreSystem = NewObject<UCTPScoreSystem>(this, UCTPScoreSystem::StaticClass());
		if (ScoreSystem)
		{
			ScoreSystem->ResetScore();
		}
	}
}
