// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpGameMode.h"
#include "Centiped/Public/CtpPlayerController.h"
#include "Centiped/Public/CtpPlayerPawn.h"

ACtpGameMode::ACtpGameMode()
{
	PlayerControllerClass = ACtpPlayerController::StaticClass();
	DefaultPawnClass = ACtpPlayerPawn::StaticClass();
}

void ACtpGameMode::BeginPlay()
{
	Super::BeginPlay();

	
}

