// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpHud.h"

#include "CtpGameMode.h"
#include "CtpLog.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "CtpPlayerPawn.h"
#include "CTPScoreSystem.h"

ACtpHud::ACtpHud()
{
	static ConstructorHelpers::FObjectFinder<UFont> FontRef(TEXT("/Game/Centiped/Font/ARCADE_I_Font.ARCADE_I_Font"));
	if (FontRef.Succeeded())
	{
		CustomFont = FontRef.Object;
	}
	else
	{
		UE_LOG(LogCentiped, Warning, TEXT("CustomFont not found at /Game/Centiped/Font/ARCADE_I_Font.ARCADE_I_Font"));
		CustomFont = nullptr;
	}
}

void ACtpHud::DrawHUD()
{
	Super::DrawHUD();

	const ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(GetOwningPawn());
	if (!Player) return;
	const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;
	const UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem();
	if (!ScoreSystem) return;

	const float LifeCount = Player->GetLife();
	const int32 Score = ScoreSystem->GetScore();

	const FString LifeText = FString::Printf(TEXT("LIFE: %.0f"), LifeCount);
	const FText FormattedScore = FText::AsNumber(Score);
	const FString ScoreText = FString::Printf(TEXT("SCORE: %s"), *FormattedScore.ToString());
	const FLinearColor TextColor = FLinearColor::White;

	DrawText(LifeText, TextColor, 60.f, 20.f, CustomFont, 1.f, false);
	DrawText(ScoreText, TextColor, 350.f, 20.f, CustomFont, 1.f, false);

	if (bDisplayText)
	{
		FString Text1 = TEXT("GAME OVER");
		FString Text2 = TEXT("Insert a coin");
		FString Text3 = TEXT("to restart the game");
		float Text1Width;
		float Text2Width;
		float Text3Width;
		float TextHeight;

		Canvas->StrLen(CustomFont, Text1, Text1Width, TextHeight);
		Canvas->StrLen(CustomFont, Text2, Text2Width, TextHeight);
		Canvas->StrLen(CustomFont, Text3, Text3Width, TextHeight);

		float ScreenX = Canvas->SizeX / 2.f;

		float X1 = ScreenX - (Text1Width / 2.f);
		float X2 = ScreenX - (Text2Width / 2.f);
		float X3 = ScreenX - (Text3Width / 2.f);
		FLinearColor BackgroundColor = FLinearColor(0.f, 0.f, 0.f, 0.7f);
		
		DrawRect(BackgroundColor, X3 - 10, Canvas->SizeY / 2.f - 60, Text3Width + 20, 150);
		DrawText(Text1, TextColor, X1, Canvas->SizeY / 2.f - 50, CustomFont, 1.0f, false);
		DrawText(Text2, TextColor, X2, Canvas->SizeY / 2.f, CustomFont, 1.0f, false);
		DrawText(Text3, TextColor, X3, Canvas->SizeY / 2.f + 50, CustomFont, 1.0f, false);
	}
}

void ACtpHud::ShowGameOverText(bool bDisplayGameOverText)
{
	bDisplayText = bDisplayGameOverText;
}

FString AddThousandSpaces(const FString& Number)
{
	FString Formatted;
	int32 Len = Number.Len();
	int32 Count = 0;

	for (int32 i = Len - 1; i >= 0; --i)
	{
		Formatted.InsertAt(0, Number[i]);
		Count++;
		if (Count == 3 && i != 0)
		{
			Formatted.InsertAt(0, TEXT(' '));
			Count = 0;
		}
	}

	return Formatted;
}
