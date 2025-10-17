// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPCentiNode.h"
#include "CtpGameMode.h"
#include "CtpLog.h"


// Sets default values
ACTPCentiNode::ACTPCentiNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/Centiped.Centiped"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
	MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACTPCentiNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACTPCentiNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PrevNode == nullptr)
	{
		IsHead=true;
	}
	Move(DeltaTime);
}

void ACTPCentiNode::Move(float DeltaTime)
{
	if (IsHead)
	{
		FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
		NewLocation += MoveDirection * DeltaTime * MoveSpeed;

		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (NewLocation.X < GameMode->Bounds.Min.X-MeshScale.X)
			{
				// TODO
				//NewLocation= FVector2D(,DeltaTime * MoveSpeed+GetActorLocation().Y);
				SwitchDirection();
				NextNode->HitSwitch = NewLocation;
				NewLocation = FVector2D(NewLocation.X, NewLocation.Y-40);
			}
		}
		SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
	}
	else
	{
		FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
		NewLocation += MoveDirection * DeltaTime * MoveSpeed;
		float DistToNextLoc = DeltaTime * MoveSpeed;
		float DistToNextSwitch = FMath::Abs(HitSwitch.X - GetActorLocation().X);
		if (DistToNextLoc > DistToNextSwitch)
		{
			NewLocation = FVector2D(HitSwitch.X, DistToNextLoc-DistToNextSwitch+GetActorLocation().Y);
			SwitchDirection();
			if (NextNode)
			{
				NextNode->HitSwitch = HitSwitch;
			}
		}
		//AddActorLocalOffset(FVector(0, NewLocation.X,NewLocation.Y));
		SetActorLocation(FVector(0,NewLocation.X,NewLocation.Y));
	}
}

void ACTPCentiNode::SwitchDirection()
{
	
	MoveDirection.X = -MoveDirection.X;
	
}


