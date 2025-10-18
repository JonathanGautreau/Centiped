// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpMushroom.h"

#include "CtpGameMode.h"
#include "CTPLog.h"
#include "CTPScoreSystem.h"


// Sets default values
ACtpMushroom::ACtpMushroom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Mushrooms.SM_Mushrooms"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}

	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	
	MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
	MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACtpMushroom::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACtpMushroom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACtpMushroom::InitializePosition(const FVector& InitialPosition)
{
	SetActorLocation(InitialPosition);
}

void ACtpMushroom::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// if (OtherActor == ACtpBullet)

	if (OtherActor && OtherActor != this)
	{
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			// if (OtherActor == ACtpBullet)
			Life--;
			
			if (Life == 0)
			{
				// if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
				// {
				// 	ScoreSystem->SetScore(ScoreSystem->GetScore() + 1);
				// }
				// else
				// {
				// 	UE_LOG(LogCentiped, Warning, TEXT("ScoreSystem is  null"));
				// }
				Destroy();
			}
			
			UE_LOG(LogCentiped, Warning, TEXT("Mushroom is  overlying : %s"), *OtherActor->GetName());
		}
	}
}