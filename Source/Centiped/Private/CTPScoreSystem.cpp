// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPScoreSystem.h"
#include "CTPLog.h"
#include "GameFramework/PlayerController.h"
#include "CtpPlayerPawn.h"

class ACtpPlayerPawn;

ACTPScoreSystem::ACTPScoreSystem()
{
}

void ACTPScoreSystem::BeginPlay()
{
	Super::BeginPlay();
}

int ACTPScoreSystem::GetScore() const
{
	return Score;
}

void ACTPScoreSystem::SetScore(const int NewScore)
{
	// Add one player life every 10 000 points
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
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

void ACTPScoreSystem::ResetScore()
{
	SetScore(0);
}
