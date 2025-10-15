// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CtpGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACtpGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACtpGameMode();

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category="Game")
	float Height = 1920;
	
	UPROPERTY(VisibleAnywhere, Category="Game")
	float Width = 1600;
	
	UPROPERTY(EditAnywhere, Category="Game")
	FBox2D Bounds = FBox2D(FVector2D(-(Width / 2), -(Height / 2)), FVector2D(Width / 2, Height / 2));
};
