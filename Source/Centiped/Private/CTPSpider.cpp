// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPSpider.h"


// Sets default values
ACTPSpider::ACTPSpider()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Spider.SM_Spider"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
	// ------- Override properties ------- //
	MeshScale = FVector2D(.4f,.4f);
	MoveSpeed = 250;
	Life = 1;

	// ------- Specific properties ------- //
	LastLayerPoint = MeshScale.Y * 100 * 2;
	FirstLayerPoint = MeshScale.X * 100 * 4;
	DistToPlayer = 0;
	IsLeftDirection = false;
}

// Called when the game starts or when spawned
void ACTPSpider::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACTPSpider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPSpider::Move(float Deltatime)
{
}

void ACTPSpider::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ACTPSpider::HitMushroom(ACtpMushroom* Mushroom)
{
	Super::HitMushroom(Mushroom);
}

void ACTPSpider::HitPlayer(ACtpPlayerPawn* Player)
{
	Super::HitPlayer(Player);
}

void ACTPSpider::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);

	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (!PlayerController) return;
			
			if (ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(PlayerController->GetPawn()))
			{
				if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
				{
					HitSwitch = FVector2D(Player->GetActorLocation().Y - GetActorLocation().Y, Player->GetActorLocation().Z - GetActorLocation().Z);
					DistToPlayer = HitSwitch.Size();
					if (DistToPlayer > FirstLayerPoint)
					{
						ScoreSystem->SetScore(ScoreSystem->GetScore() + 300);
					}
					else if (DistToPlayer > LastLayerPoint)
					{
						ScoreSystem->SetScore(ScoreSystem->GetScore() + 600);
					}
					else
					{
						ScoreSystem->SetScore(ScoreSystem->GetScore() + 900);
					}
				}
			}
		}
	}
}
