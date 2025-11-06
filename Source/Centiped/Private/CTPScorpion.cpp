// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPScorpion.h"

#include "CtpGameMode.h"
#include "CtpMushroom.h"
#include "FMODStudioModule.h"
#include "fmod_studio.hpp"


// Sets default values
ACTPScorpion::ACTPScorpion()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		StaticMeshRef(TEXT("/Game/Centiped/Meshes/Scorpion.Scorpion"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}

	// ------- Override properties ------- //
	MeshScale = FVector2D(.6f, .6f);
	CollisionBox->SetBoxExtent(FVector(50.f, MeshScale.X * 100 * 0.5f, MeshScale.Y * 100 * 0.5f));
	MoveSpeed = 850;
	Life = 1;
}

// Called when the game starts or when spawned
void ACTPScorpion::BeginPlay()
{
	Super::BeginPlay();
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
	if (bIsLeftDirection) NewDirection = FVector2D(-1, 0);
	else NewDirection = FVector2D(1, 0);
	NewLocation += NewDirection * Deltatime * MoveSpeed;
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));

	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (FMath::Abs(NewLocation.X) > GameMode->Bounds.Max.X)
		{
			for (auto Mush : MushToPoison)
			{
				Mush->BecomePoison();

				if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop()) GameLoop->PoisonedMush.Emplace(Mush);
			}
			MushToPoison.Empty();
			Destroy();
			if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
				GameLoop->bIsScorpion = false;
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

void ACTPScorpion::HitPlayer(ACtpPlayerPawn* Player)
{
	Super::HitPlayer(Player);
}

void ACTPScorpion::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);

	FMOD::Studio::EventDescription* AudioDesc_SCORPIONDESTROY =nullptr;
	FMOD::Studio::EventInstance* AudioInst_SCORPIONDESTROY =nullptr;
	IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime)->getEvent("event:/Ennemy/Scorpion_Destroy", &AudioDesc_SCORPIONDESTROY);
	AudioDesc_SCORPIONDESTROY->createInstance(&AudioInst_SCORPIONDESTROY);
	AudioInst_SCORPIONDESTROY->start();
	AudioInst_SCORPIONDESTROY->release();
	AudioDesc_SCORPIONDESTROY = nullptr;
	AudioInst_SCORPIONDESTROY = nullptr;
	
	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UCTPScoreSystem* Score = GameMode->GetScoreSystem())
		{
			Score->SetScore(Score->GetScore() + 1000);
		}
		if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
			GameLoop->bIsScorpion = false;
		Destroy();
	}
}
