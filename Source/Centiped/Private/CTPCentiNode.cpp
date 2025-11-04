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
	float ClampedDeltaTime = FMath::Clamp(DeltaTime, 0.0f, 1.f / 30.f);
	SmoothedDeltaTime = FMath::Lerp(ClampedDeltaTime, DeltaTime, 0.1f);
	
	if (!bIsHead)
	{
		if (PrevNode == nullptr)
		{
			MeshComponent->SetStaticMesh(HeadNodeMesh);
			bIsHead = true;		//If no Previous node, this node become a head
		}
	}
	
	if (bIsHead)
	{
		MoveTheHead(SmoothedDeltaTime);
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
		FollowPrevNode(SmoothedDeltaTime);
	}
}

void ACTPCentiNode::FollowPrevNode(float DeltaTime)
{
	if (!PrevNode || PositionHistory.Num() == 0) return;
	
	FVector Current = GetActorLocation();
	float MoveStep = MoveSpeed * DeltaTime;
	
	while (PositionHistory.Num() > 0)
	{
		for (int32 i = 0; i < PositionHistory.Num(); ++i)
		{
			float DistWithNewHead = (NewHeadPosition - PositionHistory[i]).Size();

			if (DistWithNewHead < 10)
			{
				if (i > 0)
				{
					PositionHistory.RemoveAt(0, i);
				}
				break;
			}
		}
		
		FVector Target = PositionHistory[0];
		FVector ToTarget = Target - Current;
		float Distance = ToTarget.Size();
		
		if (Distance <= MoveStep)
		{
			Current = Target;
			MoveStep -= Distance;
			
			PositionHistory.RemoveAt(0);

			if (MoveStep <= 0.f)
				break;
		}
		else
		{
			FVector Dir =  ToTarget / Distance;
			MovingDirection = FVector2D(FMath::RoundToFloat(Dir.Y),FMath::RoundToFloat(Dir.Z));
			RemainingVerticalOffset = Distance * MovingDirection.Y;
			Current += Dir * MoveStep;
			break;
		}
	}

	// ----- Distance correction -----
	FVector ToPrev = PrevNode->GetActorLocation() - Current;
	float CurrentDist = ToPrev.Size();

	FVector RecentDir;
	if (PositionHistory.Num() > 0)
		RecentDir = (PositionHistory[0] - Current).GetSafeNormal();
	else
		RecentDir = (Current - PrevNode->GetActorLocation()).GetSafeNormal();
	
	float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RecentDir, ToPrev.GetSafeNormal())));
	float TurnAngleThreshold = 20.f;
	
	if (AngleDeg < TurnAngleThreshold)
	{
		if (CurrentDist < VerticalOffset - 2)
		{
			FVector Dir = -ToPrev.GetSafeNormal();
			MovingDirection = FVector2D(FMath::RoundToFloat(-ToPrev.GetSafeNormal().Y), FMath::RoundToFloat(-ToPrev.GetSafeNormal().Z));
			float Adjust = (75 - CurrentDist) * 0.5f;
			Current += Dir * Adjust;
		}
		else if (CurrentDist > VerticalOffset + 2)
		{
			FVector Dir = ToPrev.GetSafeNormal();
			
			MovingDirection = FVector2D(FMath::RoundToFloat(ToPrev.GetSafeNormal().Y), FMath::RoundToFloat(ToPrev.GetSafeNormal().Z));
			float Adjust = (CurrentDist - 85) * 0.5f;
			Current += Dir * Adjust;
		}
	}
	SetActorLocation(Current);
}

AActor* ACTPCentiNode::DetectNextObstacle()
{
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0.f, MovingDirection.X, 0) * 2000.f;
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 1.0f, 0, 2.0f);

	if (bHit)
	{
		FVector NextMushroomLocation = FVector(HitResult.Location.X, HitResult.Location.Y - MeshScale.X * 100 * 0.5f * MovingDirection.X, HitResult.Location.Z);
		AddHitSwitch(NextMushroomLocation, NextNode);
	}
	else
	{
		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			AddHitSwitch(FVector(GetActorLocation().X, GameMode->Bounds.Max.X * MovingDirection.X - MeshScale.X * 100 * 0.5f * MovingDirection.X, GetActorLocation().Z), NextNode);
		}
	}
	return HitResult.GetActor();
}

void ACTPCentiNode::AddHitSwitch(FVector Position, ACTPCentiNode* Node)
{
	if (!Node || !Node->PrevNode) return;

	FVector Last = Node->PositionHistory.Num() > 0 ? Node->PositionHistory.Last() : Node->PrevNode->GetActorLocation();
	
	if (Node->PositionHistory.Num() == 0 || FVector::Dist(Position, Last) >= VerticalOffset - 2)
	{
		Node->PositionHistory.Emplace(Position);
		DrawDebugSphere(GetWorld(), Position, 20.f, 8, FColor::MakeRandomColor(), false, 5.f);

		if (Node->NextNode)
			AddHitSwitch(Position, Node->NextNode);
		Node->bIsFalling = Node->PrevNode->bIsFalling;
	}
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

				FVector TestDirection = FVector(0.f, -LastMovingDirection.X, 0.f);
				FVector NextHorizontalLocation = InitialLocation + TestDirection * (MoveSpeed * DeltaTime * 2.0f);
		
				// FVector NextHorizontalLocation = InitialLocation + FVector(0.f, -LastMovingDirection.X, -LastMovingDirection.Y) * Step;
   				if (CheckCollisionAt(NextHorizontalLocation))
				{
					NextHorizontalLocation = InitialLocation + FVector(0.f, LastMovingDirection.X, LastMovingDirection.Y) * Step;
					// NextHorizontalLocation = InitialLocation + FVector(0.f, LastMovingDirection.X, LastMovingDirection.Y) * (MoveSpeed * DeltaTime * 0.5f);
            
					if (!CheckCollisionAt(NextHorizontalLocation))
					{
						MovingDirection = LastMovingDirection;
						DetectNextObstacle();
					}
   					else
   					{
						MovingDirection = -LastMovingDirection;
   					}
				}
				else
				{
					MovingDirection = -LastMovingDirection;
					DetectNextObstacle();
				}
			}
			else
			{
				// Vertical progressive movement
				InitialLocation.Z += Step * MovingDirection.Y;
				RemainingVerticalOffset -= Step * MovingDirection.Y;
				SetActorLocation(InitialLocation);
			}
			// we don't want to continue to the collision's checks
			return;
		}
		
		// ------- Collisions handling ------- //
		FVector NewLocation = InitialLocation + FVector(0.f, MovingDirection.X, MovingDirection.Y) * Step;
		if ((CheckCollisionAt(NewLocation) || FindDistToNextBound(NewLocation) < 5) && !bIsMovingVertically)
		{
			// if current move is horizontal
			if (MovingDirection.X != 0 || MovingDirection == FVector2D::ZeroVector)
			{
				LastMovingDirection = MovingDirection;
				FVector PivotBeforeTurn = GetActorLocation();
				AddHitSwitch(PivotBeforeTurn, NextNode);
				
				FVector NextVerticalLocation = PivotBeforeTurn;
				if (bIsFalling)
				{
					NextVerticalLocation.Z -= VerticalOffset;

					// Check of the bottom edge of the map
					if (NextVerticalLocation.Z >= GameMode->Bounds.Min.Y)
					{
						MovingDirection = FVector2D(0, -1);
					}
					else
					{
						NextVerticalLocation = InitialLocation;
						NextVerticalLocation.Z += VerticalOffset;
						MovingDirection = FVector2D(0, 1);
						bIsFalling = false;
					}
				}
				else
				{
					NextVerticalLocation.Z += VerticalOffset;

					// Check the limit at which the centipede must go down
					if (NextVerticalLocation.Z >= GameMode->Bounds.Min.Y / 3)
					{
						NextVerticalLocation = InitialLocation;
						NextVerticalLocation.Z -= VerticalOffset;
						MovingDirection = FVector2D(0, -1);
						bIsFalling = true;
					}
					else
					{
						MovingDirection = FVector2D(0, 1);
					}
				}
				
				bIsMovingVertically = true;
				RemainingVerticalOffset = VerticalOffset * MovingDirection.Y;

				// add again an offset if space is occupied
				if (CheckCollisionAt(NextVerticalLocation))
				{
					NextVerticalLocation.Z += VerticalOffset * MovingDirection.Y;
					RemainingVerticalOffset += VerticalOffset * MovingDirection.Y;
				}
				
				AddHitSwitch(NextVerticalLocation, NextNode);
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
		FCollisionShape::MakeBox(FVector(5.f, MeshScale.X * 100.f * 0.5f - 2, MeshScale.Y * 100.f * 0.5f - 2)),
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
		if (NextNode)
		{
			GiveSwitchToTheNextNode(NewLocation);
		}
		SetNextHitswitch();
	}
	else
	{
		NewLocation += FVector(0,MovingDirection.X,MovingDirection.Y) * Step;
	}
	SetActorLocation(NewLocation);
}

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
} 

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

void ACTPCentiNode::SetNewHeadPosition(ACTPCentiNode* Node)
{
	Node->NewHeadPosition = GetActorLocation();
	if (Node->NextNode)
		SetNewHeadPosition(Node->NextNode);
	
}

void ACTPCentiNode::BecomeHead()
{
	MeshComponent->SetStaticMesh(HeadNodeMesh);
	bIsHead = true;

	// ClearHitSwitches(this);
	SetNewHeadPosition(this);
	

	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FVector InitialLocation = GetActorLocation();

		if (MovingDirection.Y != 0)
		{
			bIsMovingVertically = false;
			RemainingVerticalOffset = 0.f;
			MovingDirection = LastMovingDirection;
			DetectNextObstacle();
			return;
		}
		
		if (MovingDirection.X != 0)
		{
			LastMovingDirection = MovingDirection;
			FVector PivotBeforeTurn = GetActorLocation();
			AddHitSwitch(PivotBeforeTurn, NextNode);
					
			FVector NextVerticalLocation = PivotBeforeTurn;
			if (bIsFalling)
			{
				NextVerticalLocation.Z -= VerticalOffset;
				MovingDirection = FVector2D(0, -1);
			}
			else
			{
				NextVerticalLocation.Z += VerticalOffset;

				// Check the limit at which the centipede must go down
				if (NextVerticalLocation.Z >= GameMode->Bounds.Min.Y / 3)
				{
					NextVerticalLocation = InitialLocation;
					NextVerticalLocation.Z -= VerticalOffset;
					MovingDirection = FVector2D(0, -1);
					bIsFalling = true;
				}
				else
				{
					MovingDirection = FVector2D(0, 1);
				}
			}
					
			bIsMovingVertically = true;
			RemainingVerticalOffset = VerticalOffset * MovingDirection.Y;

			// add again an offset if space is occupied
			if (CheckCollisionAt(NextVerticalLocation))
			{
				NextVerticalLocation.Z += VerticalOffset * MovingDirection.Y;
				RemainingVerticalOffset += VerticalOffset * MovingDirection.Y;
			}
			AddHitSwitch(NextVerticalLocation, NextNode);
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

void ACTPCentiNode::ClearHitSwitches(ACTPCentiNode* Node)
{
	FVector pos = Node->GetActorLocation();
	if (pos != FVector::ZeroVector)
	{
		if (Node->PositionHistory.Num() > 0)
			Node->PositionHistory.Pop();
		if (Node->NextNode)
			ClearHitSwitches(Node->NextNode);
	}
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
		NextNode->BecomeHead();
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
			bCentipedeExists = false;
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
