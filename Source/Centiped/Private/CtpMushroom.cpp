// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpMushroom.h"
#include "CtpBullet.h"
#include "CTPCentiNode.h"
#include "CtpGameMode.h"
#include "CTPLog.h"
#include "CTPScoreSystem.h"
#include "EngineUtils.h"
#include "FMODStudioModule.h"
#include "fmod_studio.hpp"

// Sets default values
ACtpMushroom::ACtpMushroom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		StaticMeshRef(TEXT("/Game/Centiped/Meshes/Mushroom.Mushroom"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
		MeshMushroom = StaticMeshRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRefBase2(
		TEXT("/Game/Centiped/Meshes/Mushroom2.Mushroom2"));
	if (StaticMeshRefBase2.Succeeded())
	{
		MeshMushroomDamaged = StaticMeshRefBase2.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRefBase3(
		TEXT("/Game/Centiped/Meshes/Mushroom3.Mushroom3"));
	if (StaticMeshRefBase3.Succeeded())
	{
		MeshMushroomHeavilyDamaged = StaticMeshRefBase3.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NormalMatInstanceRef(
		TEXT("/Game/Centiped/Materials/MI_DefaultMat_Green.MI_DefaultMat_Green"));
	if (NormalMatInstanceRef.Succeeded())
	{
		MatInstNormalMushroom = NormalMatInstanceRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PoisonMatInstanceRef(
		TEXT("/Game/Centiped/Materials/MI_DefaultMat_White.MI_DefaultMat_White"));
	if (PoisonMatInstanceRef.Succeeded())
	{
		MatInstPoisonnedMushroom = PoisonMatInstanceRef.Object;
	}


	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // Collisions with Player
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Collisions with Bullets
	MeshComponent->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap); // Collisions with Centipede

	MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
	MeshComponent->SetDefaultCustomPrimitiveDataVector4(0, FVector4(0.2f, 0.2f, 0, 1.0f));
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACtpMushroom::BeginPlay()
{
	Super::BeginPlay();

	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
		{
			GameLoop->SetSpawnedMushroomsCount(GameLoop->GetSpawnedMushroomsCount() + 1);
		}
	}
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

void ACtpMushroom::BecomeNormal()
{
	bIsPoison = false;
	MeshComponent->SetMaterial(0, MatInstNormalMushroom);
}

void ACtpMushroom::BecomePoison()
{
	bIsPoison = true;
	MeshComponent->SetMaterial(0, MatInstPoisonnedMushroom);
}

void ACtpMushroom::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor && OtherActor != this)
	{
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (Cast<ACtpBullet>(OtherActor))
			{
				// MeshScale = FVector2D(MeshScale.X - .2f, MeshScale.Y - .2f);
				// MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
				Life--;

				
				FMOD::Studio::EventDescription* AudioDesc_MUSHROOMDESTROY =nullptr;
				FMOD::Studio::EventInstance* AudioInst_MUSHROOMDESTROY =nullptr;
				IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime)->getEvent("event:/Mushroom/Mushroom_Destroy", &AudioDesc_MUSHROOMDESTROY);
				AudioDesc_MUSHROOMDESTROY->createInstance(&AudioInst_MUSHROOMDESTROY);
				AudioInst_MUSHROOMDESTROY->setParameterByName("LifeLeft",Life);
				AudioInst_MUSHROOMDESTROY->start();
				AudioInst_MUSHROOMDESTROY->release();
				AudioDesc_MUSHROOMDESTROY = nullptr;
				AudioInst_MUSHROOMDESTROY = nullptr;
	

				if (Life == 2)
				{
					MeshComponent->SetStaticMesh(MeshMushroomDamaged);
				}
				else if (Life == 1)
				{
					MeshComponent->SetStaticMesh(MeshMushroomHeavilyDamaged);
				}

				if (Life == 0)
				{
					if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
					{
						ScoreSystem->SetScore(ScoreSystem->GetScore() + 1);
					}
					else
					{
						UE_LOG(LogCentiped, Warning, TEXT("ScoreSystem is  null"));
					}
					Destroy();
					CheckOnDestroyed();
					// MeshScale = FVector2D(.8f, .8f);
					// MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
				}
			}
		}
	}
}

void ACtpMushroom::CheckOnDestroyed()
{
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
		{
			GameLoop->PoisonedMush.Remove(this);
			GameLoop->SetSpawnedMushroomsCount(GameLoop->GetSpawnedMushroomsCount() - 1);
			GameLoop->CheckFleaGeneration();
			GameLoop->CheckScorpionGeneration();
			GameLoop->CheckSpiderGeneration();
			for (TActorIterator<AActor> It(GetWorld()); It; ++It)
			{
				if (ACTPCentiNode* Head = Cast<ACTPCentiNode>(*It))
					if (Head->bIsHead && Head->MovingDirection.X != 0)
						Head->DetectNextObstacle();
			}
		}
	}
}
