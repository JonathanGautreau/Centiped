// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPEnemy.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"
#include "CtpBullet.h"
#include "CtpGameMode.h"
#include "CtpLog.h"


// Sets default values
ACTPEnemy::ACTPEnemy(): MoveSpeed(500)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ACTPEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// ----- Collider -----
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECC_Vehicle);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Collisions with Player
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // Collisions with Mushrooms
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Collisions with Bullets

	// ----- Mesh visuel -----
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Juste visuel
	MeshComponent->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));
	MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
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

	Move(DeltaTime);
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
			GEngine->AddOnScreenDebugMessage(1,1,FColor::Red,"hitdetected");
			HitMushroom(Mushroom);
		}
		if (ACtpPlayerPawn* PlayerPawn = Cast<ACtpPlayerPawn>(OtherActor))
		{
			HitPlayer(PlayerPawn);
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

void ACTPEnemy::HitPlayer(ACtpPlayerPawn* Player)
{
	if (Player->bIsOverlappedByEnemy)
		return;
	
	Player->bIsOverlappedByEnemy = true;
	
	// Loose one life
	Player->LoseLife();
	
	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
 		// Score mushrooms
 		if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
 			ScoreSystem->ScoreMushrooms();
	
 		// Reset round/game
 		if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
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
