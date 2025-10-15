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
		
	if(!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(.1f);
		RootComponent = CollisionComponent;
	}

	if(!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		// ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		// ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 1000.0f;
		// ProjectileMovementComponent->bRotationFollowsVelocity = true;
		// ProjectileMovementComponent->bShouldBounce = true;
		// ProjectileMovementComponent->Bounciness = 0.3f;
		// ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if(!BulletMeshComponent)
	{
		BulletMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Bullet.SM_Bullet"));
		if(StaticMeshRef.Succeeded())
		{
			BulletMeshComponent->SetStaticMesh(StaticMeshRef.Object);
		}
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

	// if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	// {
		// FVector LaunchDirection = FVector(0, 1, 0);
	// 	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	// 	NewLocation += FVector2D::Zero() * DeltaTime * MoveSpeed;
	//
	// 	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
	// }
}

void ACtpBullet::CreateBullet(ACtpBullet& Bullet)
{
}

void ACtpBullet::FireInDirection(const FVector& ShootDirection)
{
	UE_LOG(LogCentiped, Log, TEXT("Bullet fired"))
    ProjectileMovementComponent->Velocity = ShootDirection * MoveSpeed;
}

