// Fill out your copyright notice in the Description page of Project Settings.


#include "CtpMushroom.h"
#include "CtpGameMode.h"


// Sets default values
ACtpMushroom::ACtpMushroom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
	// PrimaryActorTick.bStartWithTickEnabled = true;

	// SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	// MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Mushrooms.SM_Mushrooms"));
	// if (StaticMeshRef.Succeeded())
	// {
	// 	MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	// }

	// MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	// MeshComponent->SetGenerateOverlapEvents(true);
	// MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
	// MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
	// MeshComponent->SetupAttachment(RootComponent);

	// if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	// {
	// 	int PosX = FMath::RandRange(-(static_cast<int>(round(GameMode->Width)) / 2),  static_cast<int>(round(GameMode->Width)) / 2);
	// 	int PosY = FMath::RandRange(-(static_cast<int>(round(GameMode->Width)) / 2),  static_cast<int>(round(GameMode->Height)) / 2);
	// 	SetActorLocation(FVector(0, PosX, PosY));
	// }
}

// Called when the game starts or when spawned
void ACtpMushroom::BeginPlay()
{
	Super::BeginPlay();

	// if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	// {
	// 	for (int i = 0; i < 20; ++i)
	// 	{
	// 		int PosX = FMath::RandRange(-(static_cast<int>(round(GameMode->Width)) / 2),  static_cast<int>(round(GameMode->Width)) / 2);
	// 		int PosY = FMath::RandRange(-(static_cast<int>(round(GameMode->Width)) / 2),  static_cast<int>(round(GameMode->Height)) / 2);
	// 		SetActorLocation(FVector(0, PosX, PosY));
	// 		if (UWorld* World = GetWorld())
	// 		{
	// 			FActorSpawnParameters SpawnParams;
	// 			SpawnParams.Owner = this;
	//  
	// 			World->SpawnActor<ACtpMushroom>(StaticClass(), GetActorLocation(), FRotator(), SpawnParams);
	// 		}
	// 	}
	// }
}

// Called every frame
void ACtpMushroom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

