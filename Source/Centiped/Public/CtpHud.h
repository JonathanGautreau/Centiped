// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CtpHud.generated.h"

/**
 * 
 */
UCLASS()
class CENTIPED_API ACtpHud : public AHUD
{
	GENERATED_BODY()
	
public:
	ACtpHud();
	void ShowGameOverText(bool bDisplayGameOverText);
	
protected:
	virtual void DrawHUD() override;

private:
	UPROPERTY(EditAnywhere, Category="HUD")
	bool bDisplayText = false;

	UPROPERTY(EditAnywhere, Category="HUD")
	UFont* CustomFont;
};