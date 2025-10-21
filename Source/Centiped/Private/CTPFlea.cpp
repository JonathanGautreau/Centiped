// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPFlea.h"
#include "CTPScoreSystem.h"
#include "CtpGameMode.h"
#include "CTPScoreSystem.h"
#include "CtpMushroom.h"

// Sets default values
ACTPFlea::ACTPFlea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPFlea::BeginPlay()
{
	Super::BeginPlay();
	Life = 2;
	MoveSpeed=1000;
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	HitSwitch = FVector2D(GetActorLocation().X,GameMode->Bounds.Max.Y-120);
	
}

// Called every frame
void ACTPFlea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPFlea::Move(float DeltaTime)
{
	Super::Move(DeltaTime);
	
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	NewLocation.Y -= DeltaTime * MoveSpeed;
	int Row = 0;
	if (NewLocation.Y < HitSwitch.Y)
	{
		if (UWorld* World = GetWorld())
		{
			ACtpMushroom* Mushroom = World->SpawnActor<ACtpMushroom>(ACtpMushroom::StaticClass());
			Mushroom->InitializePosition(FVector(GetActorLocation().X,GetActorLocation().Y,HitSwitch.Y));
		}
		//HitSwitch.Y-= 160.f;
	}
	
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
}


void ACTPFlea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ACTPFlea::HitMushroom(AActor* OtherActor)
{
	Super::HitMushroom(OtherActor);

	HitSwitch.Y-= 160.f;
}

void ACTPFlea::HitPLayer(AActor* OtherActor)
{
	Super::HitPLayer(OtherActor);

	if (const ACTPScoreSystem* Score = Cast<ACTPScoreSystem>(OtherActor))
	{
		//Score.RestCount;
	}
	
}

void ACTPFlea::HitBullet(AActor* OtherActor)
{
	Super::HitBullet(OtherActor);

	if (--Life == 0 )
	{
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
			{
				ScoreSystem->SetScore(ScoreSystem->GetScore() + 200);
			}
		}
		this->Destroy();
	}
}


