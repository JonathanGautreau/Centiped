// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPCentiNode.h"
#include "CtpBullet.h"
#include "CtpGameMode.h"
#include "CtpGameLoop.h"
#include "CtpLog.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"
#include "EngineUtils.h"

// Sets default values
ACTPCentiNode::ACTPCentiNode()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Centiped.SM_Centiped"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
		NormalNodeMesh = StaticMeshRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeadStaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Spider.SM_Spider"));
	if (HeadStaticMeshRef.Succeeded())
	{
		HeadNodeMesh = HeadStaticMeshRef.Object;
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
	
	if (!IsHead)
	{
		if (PrevNode == nullptr)
		{
			BecomeHead();		//If no Previous node, this node become a head
		}
	}
	// DeleteOutsideBounds();
}

FVector2d ACTPCentiNode::DetectNextMushroom(float& DistToNextBound)
{
	FVector Start = GetActorLocation();               // Start point of the ray
	FVector End = Start + FVector(0,MovingDirection.X,MovingDirection.Y)*DistToNextBound;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);            // Ignore self

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_WorldStatic,      // Collision channel
		CollisionParams
	);

	if (bHit)
	{
		auto* ActorHitted = HitResult.GetActor();
		if (ACtpMushroom* Mushroom = Cast<ACtpMushroom>(ActorHitted))
		{
			float DistToNextMush = FMath::Abs((ActorHitted->GetActorLocation() - GetActorLocation()).Length());
			if (DistToNextMush < DistToNextBound)
			{
				if (Mushroom->IsPoison)
				{
					
				}
				else
				{
					return FVector2D(ActorHitted->GetActorLocation().Y + MeshScale.X * 100 * -MovingDirection.X,
									  ActorHitted->GetActorLocation().Z + MeshScale.Y * 100 * -MovingDirection.Y);
			
				}
			}
		}
	}
	return DefaultVector;
}

void ACTPCentiNode::Move(float DeltaTime)
{
	FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);

	DistToNextLoc = DeltaTime * MoveSpeed;							//The distance to the next Location
	
	float DistToNextBound = FindDistToNextBound();

	
	if (HitSwitch == DefaultVector)
	{
		HitSwitch = DetectNextMushroom(DistToNextBound);
	}
	if (HitSwitch == DefaultVector)
	{
		HitSwitch = FVector2D(NewLocation.X + DistToNextBound * MovingDirection.X + MeshScale.X * 100 * -MovingDirection.X,
								NewLocation.Y + DistToNextBound * MovingDirection.Y + MeshScale.X * 100 * -MovingDirection.Y);
	}
	//Gives switches to next node
	DistToNextSwitch = FMath::Abs((HitSwitch - NewLocation).Size());
	if (DistToNextSwitch < DistToNextLoc)
	{
		if (MovingDirection.X != 0)
		{
			LastMovingDirection.X = MovingDirection.X;
			if (IsFalling)	MovingDirection = FVector2D(0,-1);
			else MovingDirection = FVector2D(0,1);
			NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
			HitSwitch=DefaultVector;
		}
			
		else
		{
			LastMovingDirection.Y = MovingDirection.Y;
			MovingDirection = FVector2D(-LastMovingDirection.X,0);
			NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch)*MovingDirection.X, HitSwitch.Y);
			HitSwitch=DefaultVector;
		}
	}
	else
	{
		NewLocation += MovingDirection * DistToNextLoc;
	}
	SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));

}
				
		// 		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		// 		{
 	// 				HitSwitch = FVector2D(GetActorLocation().Y,GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5 + VerticalOffset);
		// 			IsFalling = true;
		// 		}
		// 	}
		// 	else if (IsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
		// 	else if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
		//
		// 	if (IsHead && !IsCollidingPoison) IsAtTheBounds();
		//
		// 	if (NextNode)
		// 	{
		// 		NextNode->IsFalling = IsFalling;
		// 		if (NextNode->HitSwitch == DefaultVector)
		// 		{
		// 			NextNode->HitSwitch = HitSwitch;
		// 		}
		// 		else
		// 		{
		// 			NextNode->HitSwitches.Emplace(HitSwitch);
		// 		}
		// 	}
		// 	LastMovingDirection = MovingDirection;
		// 	if (IsFalling) MovingDirection = FVector2D(0,-1);//Keep the previous direction for later use 
		// 	else MovingDirection = FVector2D(0,1);
		// 	if (IsColliding) NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
		// 	else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
		// 	IsColliding = false;
		// 	IsCollidingPoison =	false;
		// 	
		// }
		// else		//When the centiped move down
		// {

			// going down
			// NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * -LastMovingDirection.X,HitSwitch.Y +  VerticalOffset * MovingDirection.Y);
			// MovingDirection = -LastMovingDirection; //Use of the previous value to turn back in the good direction
			// if (IsHead)
			// {
			// 	ACtpGameMode *GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
			// 	HitSwitch = FVector2D(GameMode->Bounds.Max);	// After the changing of direction, move away enough the hitswitch to not get the node lock around it.
			// }
			
			// Give Hitswitches
			// else
			// {
			// 	if (HitSwitches.IsEmpty())
			// 	{
			// 		HitSwitch = DefaultVector;
			// 	}
			// 	else
			// 	{
			// 		HitSwitch = HitSwitches[0];
			// 		HitSwitches.Remove(HitSwitch);
			// 	}
	//if (IsHead)		//To check the distance to the next border of the map
	//	else DistToNextSwitch = FindDistToNextNodeHitSwitch();			//To check the distance to the next switch
	//
	// if (DistToNextLoc > DistToNextSwitch || IsColliding )			//If the location is further than the next switch point or if there is a collision with a mushroom
	// {
	// 	if (MovingDirection.X != 0) //When the centiped move on the left or right
	// 	{
	// 		if (IsCollidingPoison)
	// 		{
	// 			if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	// 			{
	// 				HitSwitch = FVector2D(GetActorLocation().Y,GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5 + VerticalOffset);
	// 				IsFalling = true;
	// 			}
	// 		}
	// 		else if (IsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
	// 		else if (IsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
	//
	// 		if (IsHead && !IsCollidingPoison) IsAtTheBounds();
	//
	// 		if (NextNode)
	// 		{
	// 			NextNode->IsFalling = IsFalling;
	// 			if (NextNode->HitSwitch == DefaultVector)
	// 			{
	// 				NextNode->HitSwitch = HitSwitch;
	// 			}
	// 			else
	// 			{
	// 				NextNode->HitSwitches.Emplace(HitSwitch);
	// 			}
	// 		}
	// 		LastMovingDirection = MovingDirection;
	// 		
	// 		if (IsFalling) MovingDirection = FVector2D(0,-1);//Keep the previous direction for later use 
	// 		else MovingDirection = FVector2D(0,1);
	// 		
	// 		if (IsColliding) NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
	// 		else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
	//
	// 		IsColliding = false;
	// 		IsCollidingPoison =	false;
	// 		
	// 	}
	// 	else		//When the centiped move down
	// 	{
	// 				
	// 		NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * -LastMovingDirection.X,HitSwitch.Y +  VerticalOffset * MovingDirection.Y);
	// 		MovingDirection = -LastMovingDirection; //Use of the previous value to turn back in the good direction
	// 		if (IsHead)
	// 		{
	// 			ACtpGameMode *GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
	// 			HitSwitch = FVector2D(GameMode->Bounds.Max);	// After the changing of direction, move away enough the hitswitch to not get the node lock around it.
	// 		}
	// 		else
	// 		{
	// 			if (HitSwitches.IsEmpty())
	// 			{
	// 				HitSwitch = DefaultVector;
	// 			}
	// 			else
	// 			{
	// 				HitSwitch = HitSwitches[0];
	// 				HitSwitches.Remove(HitSwitch);
	// 			}
	// 		}
	// 	}
	// }




float ACTPCentiNode::FindDistToNextBound() const
{
	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
 	{
		if (MovingDirection.Y == -1)
			return  FMath::Abs(GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5 - GetActorLocation().Z);

		if (MovingDirection.Y == 1)
			return  FMath::Abs(GameMode->Bounds.Max.Y - MeshScale.Y * 100 * 0.5 - GetActorLocation().Z);
		
		if (MovingDirection.X == -1)
 			return  FMath::Abs(GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5 - GetActorLocation().Y);

		if (MovingDirection.X == 1)
 			return  FMath::Abs(GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5 - GetActorLocation().Y);
 	}
	
 	return FLT_MAX;
}

float ACTPCentiNode::FindDistToNextNodeHitSwitch() const
{
	
	if (MovingDirection.Y != 0)
		return FMath::Abs(HitSwitch.Y + VerticalOffset * MovingDirection.Y - GetActorLocation().Z);

	if (MovingDirection.X != 0)
		return FMath::Abs(HitSwitch.X - GetActorLocation().Y);
	
	return FLT_MAX;
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

void ACTPCentiNode::BecomeHead()
{
	MeshComponent->SetStaticMesh(HeadNodeMesh);
	IsHead = true;
}

void ACTPCentiNode::DeleteOutsideBounds()
{
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (FMath::Abs(GetActorLocation().Y) > GameMode->Bounds.Max.X || FMath::Abs(GetActorLocation().Z) > GameMode->Bounds.Max.Y)
		{
			this->Destroy();
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
		// When moving horizontally, ignore vertical collisions
		// When moving vertically, ignore horizontal collisions
		if ((FMath::Abs(Mushroom->GetActorLocation().Z - GetActorLocation().Z) < 70 && MovingDirection.X != 0) ||
			(FMath::Abs(Mushroom->GetActorLocation().Y - GetActorLocation().Y) < 70 && MovingDirection.Y != 0))
		{
			IsColliding = true;
			
			if (Mushroom->IsPoison)
			{
				IsCollidingPoison = true;
			}			
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
			float ClampedYLocation = FMath::Clamp(this->GetActorLocation().Y, GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5, GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5);

			// Snap Z location on the nearest row
			float ApproximativeRow = (GameMode->Bounds.Max.Y - this->GetActorLocation().Z - Mushroom->MeshScale.Y * 100 * 0.5) / GameMode->SquareSize.Y;
			float CorrectedZLocation = GameMode->Bounds.Max.Y - (FMath::RoundToInt(ApproximativeRow) * GameMode->SquareSize.Y) - Mushroom->MeshScale.Y * 100 * 0.5;

			Mushroom->InitializePosition(FVector(GetActorLocation().X, ClampedYLocation, CorrectedZLocation));

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
