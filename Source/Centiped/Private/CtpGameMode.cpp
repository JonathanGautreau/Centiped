// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpGameMode.h"

#include "CTPGameLoop.h"
#include "Centiped/Public/CtpPlayerController.h"
#include "Centiped/Public/CtpPlayerPawn.h"
#include "CtpCentipede.h"
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

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	Centipede = GetWorld()->SpawnActor<ACtpCentipede>(SpawnParameters);
	
	if (UWorld* World = GetWorld())
	{
		World->SpawnActor<ACtpGameLoop>(ACtpGameLoop::StaticClass());
	}
}
