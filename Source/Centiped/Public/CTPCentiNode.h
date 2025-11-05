// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTPEnemy.h"
#include "CtpPlayerPawn.h"
#include "GameFramework/Actor.h"
#include "CTPCentiNode.generated.h"

UCLASS()
class CENTIPED_API ACTPCentiNode : public ACTPEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPCentiNode();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void DeleteOutsideBounds();

public:
	// ------- Common functions ------- //
	virtual void Tick(float DeltaTime) override;
	void FollowPrevNode(float DeltaTime);
	AActor* DetectNextObstacle();
	void AddHitSwitch(FVector Position, ACTPCentiNode* Node);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Move(float DeltaTime) override;
	virtual void HitMushroom(ACtpMushroom* Mushroom) override;
	virtual void HitPlayer(ACtpPlayerPawn* Player) override;
	void ClearHitSwitches(ACTPCentiNode* Node);
	void ResetHeadProperties(ACTPCentiNode* Node);
	virtual void HitBullet(ACtpBullet* Bullet) override;

	// ------- Specific functions ------- //
	FVector2D DetectNextMushroom(float& DistToNextBound);

	void GiveSwitchToTheNextNode(FVector NewHitSwitch);
	void SetNextHitswitch();

	float FindDistToNextBound(FVector NewLocation) const;
	float FindDistToNextHitSwitch() const;
	void SetNewHeadPosition(ACTPCentiNode* Node);
	void IsAtTheBounds();
	void BecomeHead();
	
	void MoveTheHead(float DeltaTime);
	bool CheckCollisionAt(FVector Location);

	// ------- Specific properties ------- //
	UPROPERTY(Category="Centipede", EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool bIsHead;

	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bIsFalling = true;

	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bIsColliding;

	UPROPERTY(category="Centipede",EditAnywhere)
	bool bIsCollidingPoison;
	
	UPROPERTY(category = "Centipede", EditAnywhere)
	float VerticalOffset = MeshScale.Y * 100;
	
	UPROPERTY(Category = "Centipede", EditAnywhere)
	FVector2D MovingDirection = FVector2D(-1, 0);
	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D LastMovingDirection = FVector2D(-1, 0);
	
	UPROPERTY(category = "Centipede", EditAnywhere)
	TArray<FVector2D> HitSwitches;
	
	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D DefaultVector;

	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* PrevNode;
	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* NextNode;

	UPROPERTY(category = "Centipede", EditAnywhere)
	UStaticMesh* NormalNodeMesh;

	UPROPERTY(category = "Centipede", EditAnywhere)
	UStaticMesh* HeadNodeMesh;
	
	UPROPERTY(category = "Centipede", EditAnywhere)
	float DistToNextLoc;

	UPROPERTY(category = "Centipede", EditAnywhere)
	float DistToNextSwitch;

	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bCentipedeExists = false;

	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bIsMovingVertically = false;

	UPROPERTY(Category="Centipede",EditAnywhere)
	float RemainingVerticalOffset = 0.f;

	UPROPERTY(Category="Centipede",EditAnywhere)
	int SegmentIndex = 0;

	UPROPERTY(Category="Centipede",EditAnywhere)
	int FollowIndex = 0;
	UPROPERTY(Category="Centipede",EditAnywhere)
	float DistanceAlongPath = 0.f;
	UPROPERTY(Category="Centipede",EditAnywhere)
	float MaxCatchupDistance = 20.f;
	UPROPERTY(Category="Centipede",EditAnywhere)
	int CurrentHitSwitchIndex = 0;
	UPROPERTY(Category="Centipede",EditAnywhere)
	TArray<FVector> PositionHistory;
	UPROPERTY(Category="Centipede",EditAnywhere)
	float SmoothedDeltaTime = 0.f;
	UPROPERTY(Category="Centipede",EditAnywhere)
	FVector NewHeadPosition;
};
