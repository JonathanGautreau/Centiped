// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CTPGameLoop.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACtpGameLoop : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACtpGameLoop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
