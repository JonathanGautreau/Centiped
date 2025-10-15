// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpBullet.h"
#include "CtpGameMode.h"
#include "CTPLog.h"


// Sets default values
ACtpBullet::ACtpBullet()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if(!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if(!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Bullet.SM_Bullet"));
		if(StaticMeshRef.Succeeded())
		{
			MeshComponent->SetStaticMesh(StaticMeshRef.Object);
		}
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
		MeshComponent->SetGenerateOverlapEvents(true);
		MeshComponent->SetRelativeScale3D(FVector(Radius, Radius, Radius));
		MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
		MeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void ACtpBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACtpBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector CurrentLocation = GetActorLocation();
	BulletVelocity(DeltaTime);

	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (CurrentLocation.Z > GameMode->Height)
		{
			Destroy();
		}
	}
}

void ACtpBullet::BulletVelocity(float DeltaTime)
{
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	NewLocation.Y += DeltaTime * MoveSpeed;
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
}

