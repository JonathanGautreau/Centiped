// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPEnemy.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"
#include "CtpBullet.h"
#include "CtpGameMode.h"


// Sets default values
ACTPEnemy::ACTPEnemy(): MoveSpeed(500), Life(1)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPEnemy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACTPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPEnemy::Move(float DeltaTime)
{
	
}

void ACTPEnemy::NotifyActorBeginOverlap(AActor* OtherActor)
{
 	Super::NotifyActorBeginOverlap(OtherActor); // method from AActor
	
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

void ACTPEnemy::HitMushroom(ACtpMushroom* Mushroom)
{
	
}

void ACTPEnemy::HitPLayer(ACtpPlayerPawn* Player)
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
void ACTPEnemy::HitBullet(ACtpBullet* Bullet)
{
	
}


