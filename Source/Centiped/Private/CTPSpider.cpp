// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPSpider.h"

#include "CtpGameMode.h"
#include "CtpLog.h"
#include "FMODStudioModule.h"
#include "fmod_studio.hpp"


// Sets default values
ACTPSpider::ACTPSpider()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/Spider.Spider"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
	// ------- Override properties ------- //
	MeshScale = FVector2D(.6f,.6f);
	CollisionBox->SetBoxExtent(FVector(50.f, MeshScale.X * 100 * 0.5f, MeshScale.Y * 100 * 0.5f));
	Life = 1;

	// ------- Specific properties ------- //
	LastLayerPoint = MeshScale.Y * 100 * 2;
	FirstLayerPoint = MeshScale.X * 100 * 4;
	DistToPlayer = 0;
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
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			FVector NewLocation = GetActorLocation();
			
			if (NewLocation.Z < GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5)
			{
				RandomSpeedOnX();
				Direction.Y = -Direction.Y;
			}
			if (NewLocation.Z > GameMode->Bounds.Min.Y / 4)
			{
				RandomSpeedOnX();
				Direction.Y = -Direction.Y;
			}
			if (NewLocation.Y < GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5)
			{
				RandomSpeedOnX();
				Direction.X = -Direction.X;
			}
			if (NewLocation.Y > GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5)
			{
				RandomSpeedOnX();
				Direction.X = -Direction.X;
			}
			
			NewLocation.Y += Direction.X * Speed.X * Deltatime;
			NewLocation.Z += Direction.Y * Speed.Y * Deltatime;
			
			SetActorLocation(NewLocation);
		}
	}
}

void ACTPSpider::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ACTPSpider::HitMushroom(ACtpMushroom* Mushroom)
{
	Super::HitMushroom(Mushroom);

	if (FMath::RandBool())
		Mushroom->Destroy();
}

void ACTPSpider::HitPlayer(ACtpPlayerPawn* Player)
{
	Super::HitPlayer(Player);
}

void ACTPSpider::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);

	FMOD::Studio::EventDescription* AudioDesc_SPIDERDESTROY =nullptr;
	FMOD::Studio::EventInstance* AudioInst_SPIDERDESTROY =nullptr;
	IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime)->getEvent("event:/Ennemy/Spider_Destroy", &AudioDesc_SPIDERDESTROY);
	AudioDesc_SPIDERDESTROY->createInstance(&AudioInst_SPIDERDESTROY);
	AudioInst_SPIDERDESTROY->start();
	AudioInst_SPIDERDESTROY->release();
	AudioDesc_SPIDERDESTROY = nullptr;
	AudioInst_SPIDERDESTROY = nullptr;
	
	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (!PlayerController) return;
			
			if (ACtpPlayerPawn* Player = Cast<ACtpPlayerPawn>(PlayerController->GetPawn()))
			{
				if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
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
				if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
					GameLoop->bIsSpider = false;
				Destroy();
			}
		}
	}
}

void ACTPSpider::RandomSpeedOnX()
{
	Speed.X = FMath::RandRange(0, 250);
}
