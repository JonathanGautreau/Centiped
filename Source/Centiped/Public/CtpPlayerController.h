// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "CtpPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACtpPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACtpPlayerController();

	virtual  void BeginPlay() override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	UPROPERTY()
	TObjectPtr<ACameraActor> CameraActor;

public:
	UPROPERTY(EditAnywhere, Category="Centipede")
	TSoftObjectPtr<UInputMappingContext> InputMapping =
		TSoftObjectPtr<UInputMappingContext>(FSoftObjectPath(TEXT("/Game/Centiped/Inputs/IMC_Default.IMC_Default")));
};
