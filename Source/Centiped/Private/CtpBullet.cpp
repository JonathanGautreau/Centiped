// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpBullet.h"
#include "CtpGameMode.h"
#include "CTPLog.h"
#include "CtpMushroom.h"


// Sets default values
ACtpBullet::ACtpBullet()
{
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
		
		MeshComponent->SetGenerateOverlapEvents(true);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetCollisionProfileName(UCollisionProfile::DefaultProjectile_ProfileName);
		MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
		MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // Collisions with Mushrooms
		MeshComponent->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap); // Collisions with Centipede
		
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
	
	BulletVelocity(DeltaTime);
	DestroyOutsideBounds();
}

void ACtpBullet::BulletVelocity(float DeltaTime)
{
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	NewLocation.Y += DeltaTime * MoveSpeed;
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
}

void ACtpBullet::DestroyOutsideBounds()
{
	FVector CurrentLocation = GetActorLocation();
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (CurrentLocation.Z > GameMode->Height / 2 + Radius)
		{
			Destroy();
		}
	}
}

void ACtpBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (OtherActor && OtherActor != this)
	{
		if (Cast<ACtpMushroom>(OtherActor))
			Destroy();
		
		UE_LOG(LogCentiped, Warning, TEXT("%s is  overlying : %s"), *this->GetName(), *OtherActor->GetName());
	}
}