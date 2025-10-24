// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPFlea.h"
#include "CTPScoreSystem.h"
#include "CtpGameMode.h"
#include "CTPScoreSystem.h"
#include "CtpMushroom.h"
#include "CtpBullet.h"
#include "CtpPlayerPawn.h"

// Sets default values
ACTPFlea::ACTPFlea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Flea.SM_Flea"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	MeshScale = FVector(.4f,.4f,.4f);
	MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
	MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACTPFlea::BeginPlay()
{
	Super::BeginPlay();
	Life = 2;
	MoveSpeed=1000;
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	HitSwitch = FVector2D(GetActorLocation().X,GetActorLocation().Y-120);
	
}

// Called every frame
void ACTPFlea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move(DeltaTime);
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
		HitSwitch.Y-= FMath::RandRange(80,160);
	}
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
	if (const ACtpGameMode* Gamemode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (HitSwitch.Y < Gamemode->Bounds.Min.Y+MeshScale.Y*250)
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

	HitSwitch.Y = Mushroom->GetActorLocation().Z - FMath::RandRange(80,160);
}

void ACTPFlea::HitPLayer(ACtpPlayerPawn* Player)
{
	Super::HitPLayer(Player);
}

void ACTPFlea::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);

	if (Life == 0 )
	{
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
			{
				ScoreSystem->SetScore(ScoreSystem->GetScore() + 200);
			}
		}
	}
}


