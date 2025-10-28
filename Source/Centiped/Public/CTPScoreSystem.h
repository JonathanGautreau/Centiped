// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTPScoreSystem.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACTPScoreSystem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPScoreSystem();
	void ScoreMushrooms();

private:
	UPROPERTY(Category="ScoreSystem", VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	int Score;

public:
	int GetScore() const;
	void SetScore(int NewScore);
	void ResetScore();
};
