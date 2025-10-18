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
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	NewLocation+= MovingDirection*DeltaTime*MoveSpeed;
	DistToNextLoc = DeltaTime * MoveSpeed;
	//DistToNextSwitch;
	if (IsHead)	DistToNextSwitch = FindDistToNextHeadHitSwitch();
	else DistToNextSwitch =FindDistToNextNodeHitSwitch();
	if (DistToNextLoc > DistToNextSwitch)
		{
			if (IsFalling)
			{					
				if (MovingDirection.X != 0)
				{
					if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y+DistToNextSwitch*MovingDirection.X,GetActorLocation().Z);
					LastMovingDirection = MovingDirection;
					MovingDirection = FVector2D(0,-1);
					NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z-(DistToNextLoc-DistToNextSwitch));
					if (NextNode)
					{
						NextNode->HitSwitch = HitSwitch;
					}
					
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Switch dir")));
					}
				}
				
				else
				{
					MovingDirection = FVector2D(-LastMovingDirection.X,0);
					NewLocation = FVector2D(GetActorLocation().Y+(DistToNextLoc-DistToNextSwitch)*MovingDirection.X,HitSwitch.Y-VerticalOffset);
					HitSwitch=FVector2D(2000,2000);
				}
			}
		}

	
		SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));	
}

 float ACTPCentiNode::FindDistToNextHeadHitSwitch() const
 {
 	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
 	{
 		if (MovingDirection.Y!=0)
 			return FMath::Abs(HitSwitch.Y+VerticalOffset*MovingDirection.Y  - GetActorLocation().Z);
 		if (MovingDirection.X==-1)
 			return  FMath::Abs(GameMode->Bounds.Min.X+MeshScale.X - GetActorLocation().Y);
 		if (MovingDirection.X==1)
 			return  FMath::Abs(GameMode->Bounds.Max.X-MeshScale.X - GetActorLocation().Y);
 	}
 	return std::numeric_limits<float>::infinity();
}

float ACTPCentiNode::FindDistToNextNodeHitSwitch() const
{
	if (MovingDirection.Y!=0)
		return FMath::Abs(HitSwitch.Y+VerticalOffset*MovingDirection.Y  - GetActorLocation().Z);
	if (MovingDirection.X!=0)
		return FMath::Abs(HitSwitch.X - GetActorLocation().Y);
	return std::numeric_limits<float>::infinity();
}






