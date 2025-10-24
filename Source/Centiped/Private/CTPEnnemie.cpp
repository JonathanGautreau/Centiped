// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPEnnemie.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"
#include "CtpBullet.h"
#include "CtpGameMode.h"


// Sets default values
ACTPEnnemie::ACTPEnnemie(): Life(0), MoveSpeed(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPEnnemie::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACTPEnnemie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPEnnemie::Move(float DeltaTime)
{
	
}

void ACTPEnnemie::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if (ACtpMushroom* Mushroom = Cast<ACtpMushroom>(OtherActor))
		{
			HitMushroom(Mushroom);
		}
		if (ACtpPlayerPawn* PlayerPawn = Cast<ACtpPlayerPawn>(OtherActor))
		{
			HitPLayer(PlayerPawn);
		}
		if (ACtpBullet* Bullet = Cast<ACtpBullet>(OtherActor))
		{
			HitBullet(Bullet);
		}
	}
}

void ACTPEnnemie::HitMushroom(ACtpMushroom* Mushroom)
{
	
}
void ACTPEnnemie::HitPLayer(ACtpPlayerPawn* Player)
{
	if (Player->bIsOverlappingCentipede)
		return;
	
	// Loose one life
	Player->LoseLife();
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
 		// Score mushrooms
 		if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
 			ScoreSystem->ScoreMushrooms();
	
 		// Reset round/game
 		if (ACtpGameLoop* GameLoop = GameMode->GetGameLoop())
 		{
 			if (Player->GetLife() == 0)
 			{
 				GameLoop->GameOver();
 			}
 		else
 		{
 			GameLoop->ResetRound();
 		}
 	}
 }

}
void ACTPEnnemie::HitBullet(ACtpBullet* Bullet)
{
	Life--;
	if (Life == 0) Destroy();
}


