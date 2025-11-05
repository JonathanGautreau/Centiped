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

public:
	// ------- Common functions ------- //
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Move(float DeltaTime) override;
	virtual void HitPlayer(ACtpPlayerPawn* Player) override;
	virtual void HitBullet(ACtpBullet* Bullet) override;

	// ------- Specific functions ------- //
	void FollowPrevNode(float DeltaTime);
	void MoveTheHead(float DeltaTime);
	void AddHitSwitch(FVector Position, ACTPCentiNode* Node);
	AActor* DetectNextObstacle();
	bool CheckCollisionAt(FVector Location);
	float FindDistToNextBound(FVector NewLocation) const;
	
	void BecomeHead();
	void CleanFuturePositions(ACTPCentiNode* NewHeadNode);
	bool ShouldKeepPosition(ACTPCentiNode* Node, const FVector& NewHeadPos, int32 PositionIndex, ACTPCentiNode* NewHeadNode);

	// ------- Specific properties ------- //
	UPROPERTY(Category="Centipede", EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool bIsHead;
	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bIsFalling = true;
	UPROPERTY(category="Centipede",EditAnywhere)
	bool bIsCollidingPoison = false;
	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bCentipedeExists = false;
	UPROPERTY(Category="Centipede",EditAnywhere)
	bool bIsMovingVertically = false;

	UPROPERTY(Category="Centipede",EditAnywhere)
	float SmoothedDeltaTime = 0.f;
	UPROPERTY(Category="Centipede",EditAnywhere)
	float RemainingVerticalOffset = 0.f;
	UPROPERTY(category = "Centipede", EditAnywhere)
	float Offset = MeshScale.Y * 100;
	
	UPROPERTY(Category="Centipede",EditAnywhere)
	TArray<FVector> HitSwitches;
	
	UPROPERTY(Category = "Centipede", EditAnywhere)
	FVector2D MovingDirection = FVector2D(-1, 0);
	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D LastMovingDirection = FVector2D(-1, 0);

	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* PrevNode;
	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* NextNode;

	UPROPERTY(category = "Centipede", EditAnywhere)
	UStaticMesh* NormalNodeMesh;
	UPROPERTY(category = "Centipede", EditAnywhere)
	UStaticMesh* HeadNodeMesh;
};