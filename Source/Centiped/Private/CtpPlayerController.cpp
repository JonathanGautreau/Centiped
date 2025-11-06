// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpPlayerController.h"
#include "Centiped/Public/CtpGameMode.h"
#include "Centiped/Public/CtpLog.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

ACtpPlayerController::ACtpPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(
		TEXT("/Game/Centiped/Inputs/IMC_Default.IMC_Default"));
	if (InputMappingContextRef.Succeeded())
	{
		InputMapping = InputMappingContextRef.Object;
		UE_LOG(LogCentiped, Log, TEXT("Input Mapping Context loaded successfully in constructor"));
	}
	else
	{
		UE_LOG(LogCentiped, Error,
		       TEXT("Failed to load Input Mapping Context from /Game/Centiped/Inputs/IMC_Default.IMC_Default"));
	}
}

void ACtpPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	CameraActor = GetWorld()->SpawnActor<ACameraActor>(SpawnParameters);
	// CameraActor = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass());

	if (CameraActor)
	{
		if (UCameraComponent* CameraComponent = CameraActor->GetCameraComponent())
		{
			if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
			{
				CameraComponent->SetProjectionMode(ECameraProjectionMode::Type::Orthographic);
				CameraComponent->SetOrthoWidth(static_cast<float>(GameMode->WidthScreen));
				CameraComponent->SetWorldLocation(FVector(-500, 0, 0));
				CameraComponent->SetAspectRatio(GameMode->WidthScreen / static_cast<float>(GameMode->HeightScreen));

				CameraActor->SetActorLocation(FVector(-500.f, 0.f, 0.f));
				SetViewTarget(CameraActor);
			}
		}
	}
}

void ACtpPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACtpPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogCentiped, Log, TEXT("SetupInputComponent called"));

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!InputMapping.IsNull())
			{
				if (const UInputMappingContext* Context = InputMapping.LoadSynchronous())
				{
					InputSystem->AddMappingContext(Context, 0);
					return;
				}
			}
		}
	}

	UE_LOG(LogCentiped, Error, TEXT("Failed to add InputMapping : %s"), *InputMapping->GetPathName());
}
