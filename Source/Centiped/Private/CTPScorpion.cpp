// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPScorpion.h"

#include "CtpGameMode.h"
#include "CtpMushroom.h"


// Sets default values
ACTPScorpion::ACTPScorpion()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Scorpion.SM_Scorpion"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
	// ------- Override properties ------- //
	MeshScale = FVector2D(.6f,.6f);
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
	if (bIsLeftDirection) NewDirection = FVector2D(-1,0);
	else NewDirection = FVector2D(1,0);
	NewLocation += NewDirection * Deltatime * MoveSpeed;
	SetActorLocation(FVector(0,NewLocation.X,NewLocation.Y));
	
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
