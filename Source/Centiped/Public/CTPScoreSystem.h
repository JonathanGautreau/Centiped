// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CTPScoreSystem.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API UCTPScoreSystem : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="ScoreSystem", VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	int Score;

public:
	int GetScore() const;
	void SetScore(int NewScore);
	void ResetScore();
};
