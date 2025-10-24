// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPScorpion.h"
#include "CtpMushroom.h"


// Sets default values
ACTPScorpion::ACTPScorpion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPScorpion::BeginPlay()
{
	Super::BeginPlay();
	IsLeftDirection = FMath::RandBool();
	if (IsLeftDirection)
	{
		SetActorLocation(FVector(GetActorLocation().X,-GetActorLocation().Y,GetActorLocation().Z));
	}
}

// Called every frame
void ACTPScorpion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPScorpion::Move(float Deltatime)
{
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	FVector2D NewDirection = FVector2D::ZeroVector;
	if (IsLeftDirection) NewDirection = FVector2D(-1,0);
	if (IsLeftDirection) NewDirection = FVector2D(1,0);
	NewLocation += NewDirection * Deltatime * MoveSpeed;
	SetActorLocation(FVector(0,NewLocation.X,NewLocation.Y));
	
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (NewLocation.X>GameMode->Bounds.Max.X || NewLocation.X<GameMode->Bounds.Min.X)
		{
			for (auto Mush : MushToPoison)
			{
				Mush->IsPoison = true;
				if (ACtpGameLoop* GameLoop = GameMode->GetGameLoop()) GameLoop->PoisonedMush.Emplace();
			}
			Destroy();
		}
	}
}


void ACTPScorpion::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ACTPScorpion::HitMushroom(ACtpMushroom* Mushroom)
{
	Super::HitMushroom(Mushroom);
	
	MushToPoison.Emplace(Mushroom);
}


void ACTPScorpion::HitPLayer(ACtpPlayerPawn* Player)
{
	Super::HitPLayer(Player);
}

void ACTPScorpion::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);

	if (Life == 0)
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (ACTPScoreSystem* Score = GameMode->GetScoreSystem())
			{
				Score->SetScore(Score->GetScore() + 1000);
			}
		}
	}
		
}





