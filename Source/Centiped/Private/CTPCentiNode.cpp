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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/Node.Node"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
		NormalNodeMesh = StaticMeshRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeadStaticMeshRef(TEXT("/Game/Centiped/Meshes/Head.Head"));
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
	if (!bIsHead)
	{
		if (PrevNode == nullptr)
		{
			BecomeHead();		//If no Previous node, this node become a head
		}
	}
	
	if (bIsHead)
	{
		MoveTheHead(DeltaTime);
		if (MovingDirection.X == 1)
		{
			SetActorRotation(FRotator(0, 0, 180));
		}
		else if (MovingDirection.X == -1)
		{
			SetActorRotation(FRotator(0, 0, 0));
		}
		else if (MovingDirection.Y == 1)
		{
			SetActorRotation(FRotator(0, 0, 90));
		}
		else if (MovingDirection.Y == -1)
		{
			SetActorRotation(FRotator(0, 0, 270));
		}
	}
	else
	{
		Super::Tick(DeltaTime);
	}
	// DeleteOutsideBounds();
}






void ACTPCentiNode::MoveTheHead(float DeltaTime)
{
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FVector InitialLocation = GetActorLocation();
		float Step = MoveSpeed * DeltaTime;

		// ------- Vertical step handling ------- //
		if (bIsMovingVertically)
		{

			if (FMath::Abs(RemainingVerticalOffset) <= Step)
			{
				InitialLocation.Z += RemainingVerticalOffset;
				
				SetActorLocation(InitialLocation);
				
				
				bIsMovingVertically = false;
				RemainingVerticalOffset = 0.f;

				// ordre de priorité : -LastMovingDirection sinon LastMovingDirection sinon on descend
				
				// je viens de finir de descendre, je veux inverser la direction horizontale
				// mais je dois checker si ya une collision juste a côté
				
				FVector NextHorizontalLocation = InitialLocation + FVector(0.f, -LastMovingDirection.X, -LastMovingDirection.Y) * Step;
				if (CheckCollisionAt(NextHorizontalLocation))
				{
					// si oui, je fais le même check dans la direction opposée
					NextHorizontalLocation = InitialLocation + FVector(0.f, LastMovingDirection.X, LastMovingDirection.Y) * Step;

					// si c'est pas bloqué, je privilégie la direction opposée à celle où je devais aller
					if (!CheckCollisionAt(NextHorizontalLocation))
					{
						MovingDirection = LastMovingDirection;
						GiveSwitchToTheNextNode(InitialLocation);
					}



						
					// si c'est toujours bloqué alors je redescend d'une step
					// pas besoin d'ajouter quoi que ce soit l'algo fait en sorte de descendre
					
				}
				else
				{
					MovingDirection = -LastMovingDirection;
					GiveSwitchToTheNextNode(InitialLocation);
				}
			}
			else
			{
				// Déplacement progressif vertical
				InitialLocation.Z += Step * MovingDirection.Y;
				RemainingVerticalOffset -= Step * MovingDirection.Y;

				SetActorLocation(InitialLocation);
			}
			return;
		}

		
		// ------- Collisions handling ------- //
		FVector NewLocation = InitialLocation + FVector(0.f, MovingDirection.X, MovingDirection.Y) * Step;
		if ((CheckCollisionAt(NewLocation) || FindDistToNextBound(NewLocation) < 5) && !bIsMovingVertically)
		{
			// if current move is horizontal
			if (MovingDirection.X != 0)
			{
				LastMovingDirection = MovingDirection;
				GiveSwitchToTheNextNode(NewLocation);
				
				
				FVector DownLocation = InitialLocation;
				DownLocation.Z -= VerticalOffset;
				IsAtTheBounds();
				
				if (DownLocation.Z >= GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5f)
					MovingDirection = FVector2D(0, -1);
				else
					MovingDirection = FVector2D(0, 1);
				
				bIsMovingVertically = true;
				RemainingVerticalOffset = VerticalOffset * MovingDirection.Y;
				
				if (CheckCollisionAt(DownLocation))
					RemainingVerticalOffset += VerticalOffset * MovingDirection.Y;

				
			}

			// if current move is vertical
			else if (MovingDirection.Y != 0)
			{
				MovingDirection = -LastMovingDirection;
			}
		}

		// Limits of the game zone
		//NewLocation.Y = FMath::Clamp(NewLocation.Y, GameMode->Bounds.Min.X + 0.5f * MeshScale.X * 100, GameMode->Bounds.Max.X - 0.5f * MeshScale.X * 100);
		//NewLocation.Z = FMath::Clamp(NewLocation.Z, GameMode->Bounds.Min.Y + 0.5f * MeshScale.Y * 100, GameMode->Bounds.Max.Y - 0.5f * MeshScale.Y * 100);
		
		SetActorLocation(NewLocation);
	}	
}

bool ACTPCentiNode::CheckCollisionAt(FVector Location)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit =  GetWorld()->SweepSingleByChannel(
		Hit,
		GetActorLocation(),
		Location,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(FVector(5.f, MeshScale.X * 100.f * 0.5f - 2 , MeshScale.Y * 100.f * 0.5f - 2)),
		Params
	);

	return bHit;
}

void ACTPCentiNode::GiveSwitchToTheNextNode(FVector NewHitSwitch)
{
	if (NextNode)
	{
		//NextNode->GiveSwitchToTheNextNode(NewHitSwitch);

		FVector2D NewHitSwitch2D = FVector2D(NewHitSwitch.Y, NewHitSwitch.Z);
		if (NextNode->HitSwitch == DefaultVector)
		{
			NextNode->HitSwitch = NewHitSwitch2D;
		}
		else
		{
			NextNode->HitSwitches.Emplace(NewHitSwitch2D);
		}
		NextNode->bIsFalling = bIsFalling;
	}
}

void ACTPCentiNode::Move(float DeltaTime)
{
	FVector InitialLocation = GetActorLocation();
	FVector NewLocation = GetActorLocation();
	float Step = MoveSpeed * DeltaTime;
	DistToNextSwitch = FindDistToNextHitSwitch();
	
	if (DistToNextSwitch < Step)
	{
		if (bIsMovingVertically)
		{
			MovingDirection = PrevNode->MovingDirection;
			bIsMovingVertically = false;
			NewLocation = FVector(0,HitSwitch.X + MovingDirection.X * (Step - DistToNextSwitch),HitSwitch.Y );
			
		}
		else
		{
			//LastMovingDirection = MovingDirection;
			bIsMovingVertically = true;
			
			if (bIsFalling)	
				MovingDirection = FVector2D(0,-1);
			
			else
				MovingDirection = FVector2D(0,1);

			
			NewLocation = FVector(0,HitSwitch.X,HitSwitch.Y + MovingDirection.Y * (Step - DistToNextSwitch));
		}
		
		GiveSwitchToTheNextNode(NewLocation);
		SetNextHitswitch();
	}
	else
	{
		NewLocation += FVector(0,MovingDirection.X,MovingDirection.Y) * Step;
	}
	SetActorLocation(NewLocation);
	
	// FVector InitialLocation = GetActorLocation();
	// FVector2D NewLocation;
	// DistToNextLoc = DeltaTime * MoveSpeed;							//The distance to the next Location
	//
	// float DistToNextBound = FindDistToNextBound();
	//
	// // If Hitswitch's set find the next Location of the hitswitche (either in front of a mushroom, or the next border), excecution not clean at all
	// if (HitSwitch == DefaultVector)
	// {
	// 	HitSwitch = DetectNextMushroom(DistToNextBound);
	// }
	// if (HitSwitch == DefaultVector)
	// {
	// 	HitSwitch = FVector2D(NewLocation.X + DistToNextBound * MovingDirection.X ,
	// 							NewLocation.Y + DistToNextBound * MovingDirection.Y );
	// }
	//
	// //check if the node reach the hitswitch and make the node change direction or just make it moveon the next point
	// DistToNextSwitch = FMath::Abs((HitSwitch - NewLocation).Size());
	// if (DistToNextSwitch < DistToNextLoc)
	// {
	// 	if (MovingDirection.X != 0)
	// 	{
	// 		LastMovingDirection.X = MovingDirection.X;
	// 		if (bIsFalling)	MovingDirection = FVector2D(0,-1);
	// 		else MovingDirection = FVector2D(0,1);
	// 		NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
	// 		HitSwitch=DefaultVector;
	// 	}
	// 		
	// 	else
	// 	{
	// 		LastMovingDirection.Y = MovingDirection.Y;
	// 		MovingDirection = FVector2D(-LastMovingDirection.X,0);
	// 		NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch)*MovingDirection.X, HitSwitch.Y);
	// 		HitSwitch=DefaultVector;
	// 	}
	// }
	// else
	// {
	// 	NewLocation += MovingDirection * DistToNextLoc;
	// }
	// SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));

} //TO ClEAN / DELETE THE COMMENTED PART

void ACTPCentiNode::SetNextHitswitch()
{
	if (HitSwitches.Num() > 0)
	{
		HitSwitch = HitSwitches[0];
		HitSwitches.RemoveAt(0);
	}
	else
	{
		HitSwitch = DefaultVector;
	}
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
				if (Mushroom->bIsPoison)
				{
					
				}
				else
				{
					return FVector2D(ActorHitted->GetActorLocation().Y + MeshScale.X * 100.f * -MovingDirection.X,
									  ActorHitted->GetActorLocation().Z + MeshScale.Y * 100.f * -MovingDirection.Y);
			
				}
			}
		}
	}
	return DefaultVector;
} //TO DELETE
				
		// 		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		// 		{
 	// 				HitSwitch = FVector2D(GetActorLocation().Y,GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5 + VerticalOffset);
		// 			bIsFalling = true;
		// 		}
		// 	}
		// 	else if (bIsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
		// 	else if (bIsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
		//
		// 	if (bIsHead && !bIsCollidingPoison) IsAtTheBounds();
		//
		// 	if (NextNode)
		// 	{
		// 		NextNode->bIsFalling = bIsFalling;
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
		// 	if (bIsFalling) MovingDirection = FVector2D(0,-1);//Keep the previous direction for later use 
		// 	else MovingDirection = FVector2D(0,1);
		// 	if (bIsColliding) NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
		// 	else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
		// 	bIsColliding = false;
		// 	bIsCollidingPoison =	false;
		// 	
		// }
		// else		//When the centiped move down
		// {

			// going down
			// NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * -LastMovingDirection.X,HitSwitch.Y +  VerticalOffset * MovingDirection.Y);
			// MovingDirection = -LastMovingDirection; //Use of the previous value to turn back in the good direction
			// if (bIsHead)
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
	//if (bIsHead)		//To check the distance to the next border of the map
	//	else DistToNextSwitch = FindDistToNextNodeHitSwitch();			//To check the distance to the next switch
	//
	// if (DistToNextLoc > DistToNextSwitch || bIsColliding )			//If the location is further than the next switch point or if there is a collision with a mushroom
	// {
	// 	if (MovingDirection.X != 0) //When the centiped move on the left or right
	// 	{
	// 		if (bIsCollidingPoison)
	// 		{
	// 			if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	// 			{
	// 				HitSwitch = FVector2D(GetActorLocation().Y,GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5 + VerticalOffset);
	// 				bIsFalling = true;
	// 			}
	// 		}
	// 		else if (bIsColliding) HitSwitch = FVector2D(GetActorLocation().Y, GetActorLocation().Z); //In case of the colliding take the actual position (head only)
	// 		else if (bIsHead) HitSwitch = FVector2D(GetActorLocation().Y + DistToNextSwitch * MovingDirection.X,GetActorLocation().Z); //Otherwise set to the theorical next hitswitch
	//
	// 		if (bIsHead && !bIsCollidingPoison) IsAtTheBounds();
	//
	// 		if (NextNode)
	// 		{
	// 			NextNode->bIsFalling = bIsFalling;
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
	// 		if (bIsFalling) MovingDirection = FVector2D(0,-1);//Keep the previous direction for later use 
	// 		else MovingDirection = FVector2D(0,1);
	// 		
	// 		if (bIsColliding) NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z+DistToNextLoc*MovingDirection.Y);
	// 		else NewLocation = FVector2D(HitSwitch.X, GetActorLocation().Z + (DistToNextLoc - DistToNextSwitch)*MovingDirection.Y);
	//
	// 		bIsColliding = false;
	// 		bIsCollidingPoison =	false;
	// 		
	// 	}
	// 	else		//When the centiped move down
	// 	{
	// 				
	// 		NewLocation = FVector2D(GetActorLocation().Y + (DistToNextLoc - DistToNextSwitch) * -LastMovingDirection.X,HitSwitch.Y +  VerticalOffset * MovingDirection.Y);
	// 		MovingDirection = -LastMovingDirection; //Use of the previous value to turn back in the good direction
	// 		if (bIsHead)
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




float ACTPCentiNode::FindDistToNextBound(FVector NewLocation) const
{
	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
 	{
		if (MovingDirection.Y == -1)
			return  FMath::Abs(GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5 - NewLocation.Z);

		if (MovingDirection.Y == 1)
			return  FMath::Abs(GameMode->Bounds.Max.Y - MeshScale.Y * 100 * 0.5 - NewLocation.Z);
		
		if (MovingDirection.X == -1)
 			return  FMath::Abs(GameMode->Bounds.Min.X + MeshScale.X * 100 * 0.5 - NewLocation.Y);

		if (MovingDirection.X == 1)
 			return  FMath::Abs(GameMode->Bounds.Max.X - MeshScale.X * 100 * 0.5 - NewLocation.Y);
 	}
	
 	return FLT_MAX;
}

float ACTPCentiNode::FindDistToNextHitSwitch() const
{
	
	if (MovingDirection.Y != 0)
		return FMath::Abs(HitSwitch.Y - GetActorLocation().Z);
	
	if (MovingDirection.X != 0)
		return FMath::Abs(HitSwitch.X - GetActorLocation().Y);
	
	return FMath::Abs((HitSwitch - FVector2D(GetActorLocation().Y,GetActorLocation().Z)).Length());
}

void ACTPCentiNode::IsAtTheBounds()
{
	ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode());
	if (bIsFalling)
	{
		if (HitSwitch.Y <= GameMode->Bounds.Min.Y + MeshScale.Y * 100)
		{
			bIsFalling = false;			
		}
	}
	else
	{
		if (HitSwitch.Y >= GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5))
		{
			bIsFalling = true;
		}
	}
}

void ACTPCentiNode::BecomeHead()
{
	MeshComponent->SetStaticMesh(HeadNodeMesh);
	bIsHead = true;
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
	if (bIsHead)
	{
		// When moving horizontally, ignore vertical collisions
		// When moving vertically, ignore horizontal collisions
		if ((FMath::Abs(Mushroom->GetActorLocation().Z - GetActorLocation().Z) < 70 && MovingDirection.X != 0) ||
			(FMath::Abs(Mushroom->GetActorLocation().Y - GetActorLocation().Y) < 70 && MovingDirection.Y != 0))
		{
			bIsColliding = true;
			
			if (Mushroom->bIsPoison)
			{
				bIsCollidingPoison = true;
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
		NextNode->bIsColliding = true;
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
			if (UCTPScoreSystem* ScoreSystem = GameMode->GetScoreSystem())
			{
				if (bIsHead)
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
				if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
			
					GameLoop->GenerateCentipede(World, SpawnParams, GameMode);
				}
			}
		}
	}
}
