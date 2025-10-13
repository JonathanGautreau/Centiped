// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpPlayerController.h"
#include "Camera/CameraActor.h"

ACtpPlayerController::ACtpPlayerController()
{
	
}

void ACtpPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	CameraActor = GetWorld()->SpawnActor<ACameraActor>(SpawnParameters);
	CameraActor.
}

void ACtpPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACtpPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	}
}
