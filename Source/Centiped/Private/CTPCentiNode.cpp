// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPCentiNode.h"

#include "CtpBullet.h"
#include "CtpGameMode.h"
#include "CtpGameLoop.h"
#include "CtpLog.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACTPCentiNode::ACTPCentiNode()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Centiped.SM_Centiped"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
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
		if (MovingDirection.X != 0) //When the centiped move on the left or right
		{
			if (IsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
			else if (IsCollidingPoison)
			{
				if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
				{
					HitSwitch = FVector2D(GetActorLocation().Y,GameMode->Bounds.Min.Y);
				}
			}
			else if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
			if (IsHead) IsAtTheBounds();
			if (NextNode)
			{
				NextNode->IsFalling = IsFalling;
				if (NextNode->HitSwitch==DefaultVector)
				{
					NextNode->HitSwitch = HitSwitch;
				}
				else
				{
					NextNode->HitSwitches.Emplace(HitSwitch);
				}
			}
			LastMovingDirection = MovingDirection;
			if (IsFalling) MovingDirection = FVector2D(0,-1);//Keep the previous direction for later use 
			else MovingDirection = FVector2D(0,1);
			if (IsColliding) NewLocation =FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
			else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
			IsColliding=false;
			
		}
		else		//When the centiped move down
		{
					
			NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * -LastMovingDirection.X,HitSwitch.Y +  VerticalOffset * MovingDirection.Y);
			MovingDirection = -LastMovingDirection; //Use of the previous value to turn back in the good direction
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
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));	
}


float ACTPCentiNode::FindDistToNextHeadHitSwitch() const
{
	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
 	{
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
	{
		if (HitSwitch.Y <= GameMode->Bounds.Min.Y + MeshScale.Y * 100)
		{
			IsFalling = false;			
		}
	}
	else
	{
		if (HitSwitch.Y >= GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5))
		{
			IsFalling = true;
		}
	}
}

void ACTPCentiNode::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor); // method from AEnemy
}

void ACTPCentiNode::HitMushroom(ACtpMushroom* Mushroom)
{
	// Rotate the centipede
	if (IsHead)
	{
		if ( FMath::Abs(Mushroom->GetActorLocation().Z - GetActorLocation().Z)<70)
		{
				IsColliding = true;	
				if (Mushroom->IsPoison)
				{
					IsCollidingPoison = true;
				}			
			IsColliding = true;				
		}
	}
}

void ACTPCentiNode::HitPlayer(ACtpPlayerPawn* Player)
{
	Super::HitPlayer(Player);
}

void ACTPCentiNode::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);
	
	Bullet->Destroy(); // Destroying the bullet here avoids to hit the new mushroom
	// Split the centipede
	if (PrevNode)
	{
		PrevNode->NextNode = nullptr;
	}
	if (NextNode)
	{
		NextNode->PrevNode = nullptr;
		NextNode->IsColliding = true;
	}
	
	// Delete the hit node, then create mushroom
	this->Destroy();

	if (UWorld* World = GetWorld())
	{
		if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(World->GetAuthGameMode()))
		{
			// Spawn a mushroom
			ACtpMushroom* Mushroom = World->SpawnActor<ACtpMushroom>(ACtpMushroom::StaticClass());
			Mushroom->InitializePosition(this->GetActorLocation());

			// Score points
			if (ACTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
			{
				if (IsHead)
					ScoreSystem->SetScore(ScoreSystem->GetScore() +  100);
				else
					ScoreSystem->SetScore(ScoreSystem->GetScore() + 10);
			}
			else
			{
				UE_LOG(LogCentiped, Warning, TEXT("ScoreSystem is  null"));
			}

			// Generate a new centipede if there is no centipede
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				if (Cast<ACTPCentiNode>(*It))
					bCentipedeExists = true;
			}
			if (!bCentipedeExists)
			{
				if (ACtpGameLoop* GameLoop = GameMode->GetGameLoop())
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
			
					GameLoop->GenerateCentipede(World, SpawnParams, GameMode);
				}
			}
		}
	}
}
