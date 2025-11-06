// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CTPScoreSystem.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API UCTPScoreSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	int GetScore() const;
	void SetScore(int NewScore);
	void ResetScore();
	void ScoreMushrooms();

private:
	UPROPERTY(Category="ScoreSystem", VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	int Score;
};
