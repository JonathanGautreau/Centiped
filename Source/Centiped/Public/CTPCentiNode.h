// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CTPCentiNode.generated.h"

UCLASS()
class CENTIPED_API ACTPCentiNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPCentiNode();
	
	UPROPERTY(Category="Centipede", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(Category="Centipede", EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool IsHead;

	UPROPERTY(Category="Centipede",EditAnywhere)
	bool IsFalling = true;

	UPROPERTY(Category="Centipede",EditAnywhere)
	bool IsColliding;

	UPROPERTY(category = "Centipede", EditAnywhere)
	float MoveSpeed = 500.f;
	
	UPROPERTY(EditAnywhere, Category="Centipede")
	FVector2D MeshScale = FVector2D(.8f, .8f);
	
	UPROPERTY(category = "Centipede", EditAnywhere)
	float VerticalOffset = MeshScale.Y*100;
	
	UPROPERTY(Category = "Centipede", EditAnywhere)
	FVector2D MovingDirection = FVector2D(-1, 0);

	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D LastMovingDirection = FVector2D(-1, 0);

	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D HitSwitch = FVector2D::Zero();

	UPROPERTY(category = "Centipede", EditAnywhere)
	TArray<FVector2D> HitSwitches;

	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D DefaultVector;

	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* PrevNode;
	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* NextNode;

	UPROPERTY(category = "Centipede", EditAnywhere)
	float DistToNextLoc;

	UPROPERTY(category = "Centipede", EditAnywhere)
	float DistToNextSwitch;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void Move(float DeltaTime);
	
	float FindDistToNextHeadHitSwitch() const;

	float FindDistToNextNodeHitSwitch() const;

	virtual void Destroyed() override;
};
