// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPScoreSystem.h"
#include "CTPLog.h"
#include "CtpMushroom.h"
#include "GameFramework/PlayerController.h"
#include "CtpPlayerPawn.h"
#include "EngineUtils.h"

class ACtpPlayerPawn;


int UCTPScoreSystem::GetScore() const
{
	return Score;
}

void UCTPScoreSystem::SetScore(const int NewScore)
{
	// Add one player life every 10 000 points
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;
	ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(PlayerController->GetPawn());
	if (!Player) return;
	
	if (FMath::FloorToInt(NewScore / 10000.0f) > FMath::FloorToInt(Score / 10000.0f))
		Player->GainLife();
		
	// Max value for standard difficulty
	if (NewScore > 999999)
		Score = 0;
	else
		Score = NewScore;
	
	UE_LOG(LogCentiped, Log, TEXT("New Score: %d"), Score);
}

void UCTPScoreSystem::ResetScore()
{
	SetScore(0);
}

void UCTPScoreSystem::ScoreMushrooms()
{
	int NumberOfRemainingMushrooms = 0;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (Cast<ACtpMushroom>(*It))
			NumberOfRemainingMushrooms++;
	}
	SetScore(GetScore() + NumberOfRemainingMushrooms * 5);
}
