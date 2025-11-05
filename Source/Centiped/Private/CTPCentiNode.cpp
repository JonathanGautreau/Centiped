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
	CollisionBox->SetBoxExtent(FVector(50.f, MeshScale.X * 100 * 0.5f, MeshScale.Y * 100 * 0.5f));
}

void ACTPCentiNode::BeginPlay()
{
	Super::BeginPlay();
}

void ACTPCentiNode::Tick(float DeltaTime)
{
	float ClampedDeltaTime = FMath::Clamp(DeltaTime, 0.0f, 1.f / 30.f);
	SmoothedDeltaTime = FMath::Lerp(ClampedDeltaTime, DeltaTime, 0.1f);
	
	Super::Tick(SmoothedDeltaTime);
}

void ACTPCentiNode::Move(float DeltaTime)
{
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
		FollowPrevNode(DeltaTime);
	}
}

void ACTPCentiNode::FollowPrevNode(float DeltaTime)
{
	if (!PrevNode || HitSwitches.Num() == 0) return;
	
	FVector Current = GetActorLocation();
	float MoveStep = MoveSpeed * DeltaTime;
	
	if (HitSwitches.Num() == 0) return;
		
	FVector Target = HitSwitches[0];
	int BestTargetIndex = 0;
	
	for (int32 i = 1; i < FMath::Min(3, HitSwitches.Num()); ++i)
	{
		FVector PotentialTarget = HitSwitches[i];
		FVector ToPotential = PotentialTarget - Current;
		
		if (ToPotential.Size() <= MoveStep * 2.f)
		{
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct((Target - Current).GetSafeNormal(), ToPotential.GetSafeNormal())));
			if (AngleDeg < 60.f)
			{
				Target = PotentialTarget;
				BestTargetIndex = i;
			}
		}
	}
	
	FVector ToTarget = Target - Current;
	float Distance = ToTarget.Size();
	
	if (Distance <= MoveStep)
	{
		Current = Target;
		MoveStep -= Distance;

		if (BestTargetIndex >= 0 && BestTargetIndex < HitSwitches.Num())
			HitSwitches.RemoveAt(0, BestTargetIndex + 1);
		
		if (MoveStep > 0.f && HitSwitches.Num() > 0)
		{
			Target = HitSwitches[0];
			ToTarget = Target - Current;
			Distance = ToTarget.Size();
			
			if (Distance > 0.f)
			{
				FVector Dir = ToTarget / Distance;
				MovingDirection = FVector2D(FMath::RoundToFloat(Dir.Y), FMath::RoundToFloat(Dir.Z));
				Current += Dir * FMath::Min(MoveStep, Distance);
			}
		}
	}
	else
	{
		FVector Dir =  ToTarget / Distance;
		MovingDirection = FVector2D(FMath::RoundToFloat(Dir.Y),FMath::RoundToFloat(Dir.Z));
		RemainingVerticalOffset = Distance * MovingDirection.Y;
		Current += Dir * MoveStep;
	}

	// ----- Distance adjustment -----
	FVector ToPrev = PrevNode->GetActorLocation() - Current;
	float CurrentDist = ToPrev.Size();

	FVector RecentDir;
	if (HitSwitches.Num() > 0)
		RecentDir = (HitSwitches[0] - Current).GetSafeNormal();
	else
		RecentDir = (Current - PrevNode->GetActorLocation()).GetSafeNormal();
	
	float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RecentDir, ToPrev.GetSafeNormal())));
	
	if (AngleDeg < 25.f)
	{
		if (CurrentDist < Offset - 3)
		{
			FVector Dir = -ToPrev.GetSafeNormal();
			MovingDirection = FVector2D(FMath::RoundToFloat(-ToPrev.GetSafeNormal().Y), FMath::RoundToFloat(-ToPrev.GetSafeNormal().Z));
			float Adjust = (75 - CurrentDist) * 0.5f;
			Current += Dir * Adjust;
		}
		else if (CurrentDist > Offset + 3)
		{
			FVector Dir = ToPrev.GetSafeNormal();
			
			MovingDirection = FVector2D(FMath::RoundToFloat(ToPrev.GetSafeNormal().Y), FMath::RoundToFloat(ToPrev.GetSafeNormal().Z));
			float Adjust = (CurrentDist - 85) * 0.5f;
			Current += Dir * Adjust;
		}
	}
	SetActorLocation(Current);
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

		
				FVector NextHorizontalLocation = InitialLocation + FVector(0.f, -LastMovingDirection.X, -LastMovingDirection.Y) * (Step * 2.f);
   				if (CheckCollisionAt(NextHorizontalLocation))
				{
					NextHorizontalLocation = InitialLocation + FVector(0.f, LastMovingDirection.X, LastMovingDirection.Y) * (Step * 2.f);
            
					if (!CheckCollisionAt(NextHorizontalLocation))
					{
						MovingDirection = LastMovingDirection;
						DetectNextObstacle();
					}
   					else
						MovingDirection = -LastMovingDirection;
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
					NextVerticalLocation.Z -= Offset;

					// Check of the bottom edge of the map
					if (NextVerticalLocation.Z >= GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5)
					{
						MovingDirection = FVector2D(0, -1);
					}
					else
					{
						NextVerticalLocation = InitialLocation;
						NextVerticalLocation.Z += Offset;
						MovingDirection = FVector2D(0, 1);
						bIsFalling = false;
					}
				}
				else
				{
					NextVerticalLocation.Z += Offset;

					// Check the limit at which the centipede must go down
					if (NextVerticalLocation.Z >= GameMode->Bounds.Min.Y / 3)
					{
						NextVerticalLocation = InitialLocation;
						NextVerticalLocation.Z -= Offset;
						MovingDirection = FVector2D(0, -1);
						bIsFalling = true;
					}
					else
					{
						MovingDirection = FVector2D(0, 1);
					}
				}
				
				bIsMovingVertically = true;
				RemainingVerticalOffset = Offset * MovingDirection.Y;

				// add again an offset if space is occupied
				if (CheckCollisionAt(NextVerticalLocation))
				{
					NextVerticalLocation.Z += Offset * MovingDirection.Y;
					RemainingVerticalOffset += Offset * MovingDirection.Y;
				}

				if (bIsCollidingPoison)
				{
					float BottomLimit = GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5;
					NextVerticalLocation = FVector(PivotBeforeTurn.X, PivotBeforeTurn.Y, BottomLimit);
					RemainingVerticalOffset = BottomLimit - PivotBeforeTurn.Z;
					bIsCollidingPoison = false;
					MovingDirection = FVector2D(0, -1);
				}
				
				AddHitSwitch(NextVerticalLocation, NextNode);
			}
		}
		SetActorLocation(NewLocation);
	}
}

void ACTPCentiNode::AddHitSwitch(FVector Position, ACTPCentiNode* Node)
{
	if (!Node || !Node->PrevNode) return;

	FVector Last = Node->HitSwitches.Num() > 0 ? Node->HitSwitches.Last() : Node->PrevNode->GetActorLocation();
	
	if (Node->HitSwitches.Num() == 0 || FVector::Dist(Position, Last) >= Offset - 2)
	{
		Node->HitSwitches.Emplace(Position);
		// DrawDebugSphere(GetWorld(), Position, 20.f, 8, FColor::MakeRandomColor(), false, 5.f);

		if (Node->NextNode)
			AddHitSwitch(Position, Node->NextNode);
		Node->bIsFalling = Node->PrevNode->bIsFalling;
	}
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

	// FColor LineColor = bHit ? FColor::Red : FColor::Green;
	// DrawDebugLine(GetWorld(), Start, End, LineColor, false, 1.0f, 0, 2.0f);

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
	if (ACtpMushroom* Mushroom = Cast<ACtpMushroom>(Hit.GetActor()))
	{
		if(Mushroom->bIsPoison)
			bIsCollidingPoison = true;
	}
	return bHit;
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

void ACTPCentiNode::BecomeHead()
{
	MeshComponent->SetStaticMesh(HeadNodeMesh);
	bIsHead = true;
	
	CleanFuturePositions(this);

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
				NextVerticalLocation.Z -= Offset;
				MovingDirection = FVector2D(0, -1);
			}
			else
			{
				NextVerticalLocation.Z += Offset;

				// Check the limit at which the centipede must go down
				if (NextVerticalLocation.Z >= GameMode->Bounds.Min.Y / 3)
				{
					NextVerticalLocation = InitialLocation;
					NextVerticalLocation.Z -= Offset;
					MovingDirection = FVector2D(0, -1);
					bIsFalling = true;
				}
				else
				{
					MovingDirection = FVector2D(0, 1);
				}
			}
					
			bIsMovingVertically = true;
			RemainingVerticalOffset = Offset * MovingDirection.Y;

			// add again an offset if space is occupied
			if (CheckCollisionAt(NextVerticalLocation))
			{
				NextVerticalLocation.Z += Offset * MovingDirection.Y;
				RemainingVerticalOffset += Offset * MovingDirection.Y;
			}
			AddHitSwitch(NextVerticalLocation, NextNode);
		}
	}
}

void ACTPCentiNode::CleanFuturePositions(ACTPCentiNode* NewHeadNode)
{
	if (!NewHeadNode || !NewHeadNode->NextNode) return;
	
	FVector NewHeadPos = NewHeadNode->GetActorLocation();
	
	ACTPCentiNode* CurrentNode = NewHeadNode->NextNode;
	while (CurrentNode)
	{
		TArray<FVector> CleanedHistory;
		
		for (int32 i = 0; i < CurrentNode->HitSwitches.Num(); ++i)
		{
			FVector HistoryPos = CurrentNode->HitSwitches[i];
			bool bKeepPosition = ShouldKeepPosition(CurrentNode, NewHeadPos, i, NewHeadNode);
			
			if (bKeepPosition)
				CleanedHistory.Add(HistoryPos);
			
			else
				break;
		}
		
		CurrentNode->HitSwitches = CleanedHistory;
		
		if (CurrentNode->HitSwitches.Num() == 0 && CurrentNode->PrevNode)
			CurrentNode->HitSwitches.Add(NewHeadPos);
		
		CurrentNode = CurrentNode->NextNode;
	}
}

bool ACTPCentiNode::ShouldKeepPosition(ACTPCentiNode* Node, const FVector& NewHeadPos, int32 PositionIndex, ACTPCentiNode* NewHeadNode)
{
	float DistanceAlongPath = 0.f;
	FVector CurrentPos = Node->GetActorLocation();
	
	for (int32 i = 0; i <= PositionIndex && i < Node->HitSwitches.Num(); ++i)
	{
		FVector NextPos = Node->HitSwitches[i];
		DistanceAlongPath += FVector::Dist(CurrentPos, NextPos);
		CurrentPos = NextPos;
	}
	
	float DistanceToNewHead = 0.f;
	CurrentPos = Node->GetActorLocation();
	
	ACTPCentiNode* TempNode = Node;
	while (TempNode && TempNode != NewHeadNode)
	{
		if (TempNode->PrevNode)
		{
			DistanceToNewHead += FVector::Dist(TempNode->GetActorLocation(), TempNode->PrevNode->GetActorLocation());
			TempNode = TempNode->PrevNode;
		}
		else
			break;
		
		if (TempNode && FVector::Dist(TempNode->GetActorLocation(), NewHeadPos) < 10.f)
			break;
	}
	return (DistanceToNewHead >= DistanceAlongPath);
}

void ACTPCentiNode::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor); // method from AEnemy
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
