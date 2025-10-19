// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPCentiNode.h"

#include "CtpBullet.h"
#include "CtpGameMode.h"
#include "CtpLog.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"


// Sets default values
ACTPCentiNode::ACTPCentiNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Centiped.SM_Centiped"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
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
	NewLocation += MovingDirection * DeltaTime * MoveSpeed;
	DistToNextLoc = DeltaTime * MoveSpeed;
	//DistToNextSwitch;
	if (IsHead)	DistToNextSwitch = FindDistToNextHeadHitSwitch();
	else DistToNextSwitch = FindDistToNextNodeHitSwitch();

	if (DistToNextLoc > DistToNextSwitch)
	{
		if (IsFalling)
		{					
			if (MovingDirection.X != 0)
			{
				if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z);
				LastMovingDirection = MovingDirection;
				MovingDirection = FVector2D(0,-1);
				NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z - (DistToNextLoc - DistToNextSwitch));
				if (NextNode)
				{
					NextNode->HitSwitch = HitSwitch;
				}
				
				if (GEngine)
				{
					// GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Switch dir")));
				}
			}
			else
			{
				MovingDirection = FVector2D(-LastMovingDirection.X,0);
				NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * MovingDirection.X,HitSwitch.Y - VerticalOffset);
				HitSwitch = FVector2D(2000,2000);
			}
		}
	}
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));	
}

 float ACTPCentiNode::FindDistToNextHeadHitSwitch() const
 {
 	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
 	{
 		if (MovingDirection.Y != 0)
 			return FMath::Abs(HitSwitch.Y + VerticalOffset * MovingDirection.Y - GetActorLocation().Z);
 		if (MovingDirection.X == -1)
 			return  FMath::Abs(GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5 - GetActorLocation().Y);
 		if (MovingDirection.X == 1)
 			return  FMath::Abs(GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5 - GetActorLocation().Y);
 	}
 	return std::numeric_limits<float>::infinity();
}

float ACTPCentiNode::FindDistToNextNodeHitSwitch() const
{
	if (MovingDirection.Y != 0)
		return FMath::Abs(HitSwitch.Y + VerticalOffset * MovingDirection.Y - GetActorLocation().Z);
	if (MovingDirection.X != 0)
		return FMath::Abs(HitSwitch.X - GetActorLocation().Y);
	
	return std::numeric_limits<float>::infinity();
}

void ACTPCentiNode::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogCentiped, Warning, TEXT("%s is  overlying : %s"), *this->GetName(), *OtherActor->GetName());
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (const ACtpMushroom* Mushroom = Cast<ACtpMushroom>(OtherActor))
			{
				if (FMath::Abs(Mushroom->GetActorLocation().Y - GetActorLocation().Y) < 20)
				{
					/**
					 * TODO
					 * Rotate the centipede as it was a border screen
					 */
					UE_LOG(LogCentiped, Warning, TEXT("Mushroom detected"));
				}
			}
			if (Cast<ACtpPlayerPawn>(OtherActor))
			{
				/**
				 * TODO
				 * Hit player :
				 *	- Remove one player ife
				 *	- Count mushrooms for score
				 *	- reset level
				 */
				UE_LOG(LogCentiped, Warning, TEXT("Player detected"));
			}
			if (ACtpBullet* Bullet = Cast<ACtpBullet>(OtherActor))
			{
				/**
				 * TODO
				 * Hit centipede :
				 *  - create mushroom on current node position
				 *	- transform prev node into head
				 *  - delete current node
				 */
 				UE_LOG(LogCentiped, Warning, TEXT("Bullet detected"));
				// this->PrevNode->NextNode = nullptr;
				// if (UWorld* World = GetWorld())
				// {
				// 	ACtpMushroom* Mushroom = World->SpawnActor<ACtpMushroom>(ACtpMushroom::StaticClass());
				// 	Mushroom->InitializePosition(this->GetActorLocation());
				// }
				// this->Destroy();
			}
		}
	}
}
