// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpHud.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "CtpPlayerPawn.h"

void ACtpHud::DrawHUD()
{
	Super::DrawHUD();

	const ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(GetOwningPawn());
	if (!Player) return;

	const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;
	const ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem();
	if (!ScoreSystem) return;

	const float LifeCount = Player->GetLife();
	const int32 Score = ScoreSystem->GetScore();

	const FString LifeText = FString::Printf(TEXT("Life count : %.0f"), LifeCount);
	const FString ScoreText = FString::Printf(TEXT("Score : %d"), Score);
	const FLinearColor TextColor = FLinearColor::Black;

	DrawText(LifeText, TextColor, 60.f, 20.f, nullptr, 1.5f, false);
	DrawText(ScoreText, TextColor, 250.f, 20.f, nullptr, 1.5f, false);

	if (bDisplayText)
	{
		const FString GameOverText = FString::Printf(TEXT("Press ENTER to restart the game"));
		DrawText("GAME OVER", TextColor, 180.f, 250.f, nullptr, 2.0f, false);
		DrawText(GameOverText, TextColor, 180.f, 300.f, nullptr, 2.0f, false);
	}
}

void ACtpHud::ShowGameOverText(bool bDisplayGameOverText)
{
	bDisplayText = bDisplayGameOverText;
}
