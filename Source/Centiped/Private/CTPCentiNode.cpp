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
		IsHead=true;			//If no Previous node, this node become a head
	}
	Move(DeltaTime);			//Move Every Tick
}

void ACTPCentiNode::Move(float DeltaTime)
{
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
	NewLocation += MovingDirection * DeltaTime * MoveSpeed;
	DistToNextLoc = DeltaTime * MoveSpeed;							//The distance to the next Location
	if (IsHead)	DistToNextSwitch = FindDistToNextHeadHitSwitch();	//To check the distance to the next border of the map
	else DistToNextSwitch = FindDistToNextNodeHitSwitch();			//To check the distance to the next switch

	if (DistToNextLoc > DistToNextSwitch || IsColliding )			//If the location is further than the next switch point or if there is a collision with a mushroom
	{
		if (IsFalling)		//TODO when the centiped climb up when it reach the bottom left of the bounds + find a when to avoid or resolve the issues
		{					
			if (MovingDirection.X != 0) //When the centiped move on the left or right
			{
				if (IsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
				else if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
				if (NextNode)
				{
					if (NextNode->HitSwitch==DefaultVector)
					{
						NextNode->HitSwitch = HitSwitch;
					}
					else
					{
						NextNode->HitSwitches.Emplace(HitSwitch);
					}
				}
				LastMovingDirection = MovingDirection;		//Keep the previous direction for later use 
				MovingDirection = FVector2D(0,-1);
				if (IsColliding) NewLocation =FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
				else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
				IsColliding=false;
				
			}
			else		//When the centiped move down
			{
				MovingDirection = -LastMovingDirection;		//Use of the previous value to turn back in the good direction
				NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * MovingDirection.X,HitSwitch.Y - VerticalOffset);
				if (IsHead)
				{
					ACtpGameMode *GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
					HitSwitch = FVector2D(GameMode->Bounds.Max);	// After the changing of direction, move away enough the hitswitch to not get the node lock around it.
				}
				else
				{
					if (HitSwitches.IsEmpty())
					{
						HitSwitch = DefaultVector;
					}
					else
					{
						HitSwitch = HitSwitches[0];
						HitSwitches.Remove(HitSwitch);
					}
				}
 			}
		}
		else
		{
			{					
				if (MovingDirection.X != 0) //When the centiped move on the left or right
				{
					if (IsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
					else if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
					if (NextNode)
					{
						if (NextNode->HitSwitch==DefaultVector)
						{
							NextNode->HitSwitch = HitSwitch;
						}
						else
						{
							NextNode->HitSwitches.Emplace(HitSwitch);
						}
					}
					LastMovingDirection = MovingDirection;		//Keep the previous direction for later use 
					MovingDirection = FVector2D(0,1);
					if (IsColliding) NewLocation =FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
					else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
					IsColliding=false;
				
				}
				else		//When the centiped move up
				{
					MovingDirection = -LastMovingDirection;		//Use of the previous value to turn back in the good direction
					NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * MovingDirection.X,HitSwitch.Y + VerticalOffset);
					if (IsHead)
					{
						ACtpGameMode *GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
						HitSwitch = FVector2D(GameMode->Bounds.Max);	// After the changing of direction, move away enough the hitswitch to not get the node lock around it.
					}
					else
					{
						if (HitSwitches.IsEmpty())
						{
							HitSwitch = DefaultVector;
						}
						else
						{
							HitSwitch = HitSwitches[0];
							HitSwitches.Remove(HitSwitch);
						}
					}
				}
			}
		}
		
	}
	
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));	
}

 float ACTPCentiNode::FindDistToNextHeadHitSwitch()
 {
 	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
 	{
 		IsAtTheBounds();
 		if (MovingDirection.Y != 0)
 		{ 			
 			return FMath::Abs(HitSwitch.Y + VerticalOffset * MovingDirection.Y - GetActorLocation().Z);;
 		} 			
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

void ACTPCentiNode::IsAtTheBounds()
{
	ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
	if (IsFalling)
		if (HitSwitch.Y <= GameMode->Bounds.Max.Y + MeshScale.Y * 100 * 0.5+20)
			if (HitSwitch.X >= GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5+20 && HitSwitch.X <= GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5+20)
				IsFalling = false;
	else
		if (HitSwitch.Y >= GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5))
			if (HitSwitch.X >= GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5+20 && HitSwitch.X <= GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5+20)
				IsFalling = true;	
}


void ACTPCentiNode::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogCentiped, Warning, TEXT("%s is  overlying : %s"), *this->GetName(), *OtherActor->GetName());
		if (IsHead)
		{
			if (const ACtpMushroom* Mushroom = Cast<ACtpMushroom>(OtherActor))
			{
				
				if ( FMath::Abs(Mushroom->GetActorLocation().Z-GetActorLocation().Z)<20)
				{
					IsColliding = true;				
					
					UE_LOG(LogCentiped, Warning, TEXT("Mushroom detected"));
				}
			}
		}

		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{	
			if (ACtpPlayerPawn* Player =  Cast<ACtpPlayerPawn>(OtherActor))
			{
				/**
				 * TODO
				 * Hit player :
				 *	- Remove one player ife
				 *	- Count mushrooms for score
				 *	- reset level
				 */
				if (Player->IsInSafeFrame)
				{
					Player->LoseLife();
					if (Player->LifeLeft==0)
					{
						
					}
				}
								
				UE_LOG(LogCentiped, Warning, TEXT("Player detected"));
			}
			if (ACtpBullet* Bullet = Cast<ACtpBullet>(OtherActor))
			{
 				UE_LOG(LogCentiped, Warning, TEXT("Bullet detected"));
				if (PrevNode)
				{
					PrevNode->NextNode=nullptr;
				}
				if (NextNode)
				{
					NextNode->PrevNode=nullptr;
					NextNode->IsColliding = true;
				}
				this->Destroy();
			}
		}
	}
}

void ACTPCentiNode::Destroyed()
{
	if (UWorld* World = GetWorld())
	{
		ACtpMushroom* Mushroom = World->SpawnActor<ACtpMushroom>(ACtpMushroom::StaticClass());
		Mushroom->InitializePosition(this->GetActorLocation());
	}
	
}
