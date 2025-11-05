// Fill out your copyright notice in the Description page of Project Settings.

#include "CTPFlea.h"
#include "CTPScoreSystem.h"
#include "CtpGameMode.h"
#include "CtpMushroom.h"
#include "CtpBullet.h"
#include "CtpPlayerPawn.h"

// Sets default values
ACTPFlea::ACTPFlea()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Flea.SM_Flea"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
	// ------- Override properties ------- //
	MeshScale = FVector2D(.4f,.4f);
	CollisionBox->SetBoxExtent(FVector(50.f, MeshScale.X * 100 * 0.5f, MeshScale.Y * 100 * 0.5f));
	MoveSpeed = 1000;
	Life = 2;
}

// Called when the game starts or when spawned
void ACTPFlea::BeginPlay()
{
	Super::BeginPlay();
	
	HitSwitch = FVector2D(GetActorLocation().X,GetActorLocation().Y - 120);
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
		HitSwitch.Y -= FMath::RandRange(VerticalOffset, VerticalOffset * 2);
	}
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (HitSwitch.Y < GameMode->Bounds.Min.Y + MeshScale.Y * 250)
		{
			Destroy();
		}
	}
}

void ACTPFlea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ACTPFlea::HitMushroom(ACtpMushroom* Mushroom)
{
	Super::HitMushroom(Mushroom);

	HitSwitch.Y = Mushroom->GetActorLocation().Z - FMath::RandRange(VerticalOffset, VerticalOffset*2);
}

void ACTPFlea::HitPlayer(ACtpPlayerPawn* Player)
{
	Super::HitPlayer(Player);
}

void ACTPFlea::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);
	
 	Life--;

	if (Life == 0 )
	{
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
			{
				ScoreSystem->SetScore(ScoreSystem->GetScore() + 200);
			}
			if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
				GameLoop->bIsFlea = false;
			Destroy();
		}
	}
}


