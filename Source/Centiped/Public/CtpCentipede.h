// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CTPCentiNode.h"
#include "CtpCentipede.generated.h"

UCLASS()
class CENTIPED_API ACtpCentipede : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACtpCentipede();

	UPROPERTY(Category = "Centipede", EditAnywhere)
	int CentiSize = 2;

	UPROPERTY(Category = "Centipede", EditAnywhere)
	FVector2D HeatDirection;

	UPROPERTY(category = "Centipede", EditAnywhere)
	float CentiSpeed;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
};
