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

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ACTPEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// ------- Collisions ------- //
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	MeshComponent->SetCollisionObjectType(ECC_Vehicle);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // Collisions with Mushrooms
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Collisions with Bullets
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Collisions with Player
	
	MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
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
		UE_LOG(LogCentiped, Log, TEXT("%s is  overlapping : %s"), *this->GetName(), *OtherActor->GetName());
		if (ACtpMushroom* Mushroom = Cast<ACtpMushroom>(OtherActor))
		{
			UE_LOG(LogCentiped, Log, TEXT("Mushroom detected"));
			HitMushroom(Mushroom);
		}
		if (ACtpPlayerPawn* PlayerPawn = Cast<ACtpPlayerPawn>(OtherActor))
		{
			UE_LOG(LogCentiped, Log, TEXT("Player detected"));
			HitPlayer(PlayerPawn);
		}
		if (ACtpBullet* Bullet = Cast<ACtpBullet>(OtherActor))
		{
			UE_LOG(LogCentiped, Log, TEXT("Bullet detected"));
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
