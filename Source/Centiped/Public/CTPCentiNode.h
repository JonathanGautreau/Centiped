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

	UPROPERTY(EditAnywhere, Category="Centipede")
	FVector2D MeshScale = FVector2D(.6f, 1.f);

	UPROPERTY(Category="Centipede", EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool IsHead;
	
	UPROPERTY(Category = "Centipede", EditAnywhere)
	FVector2D MoveDirection = FVector2D(-1, 0);

	UPROPERTY(category = "Centipede", EditAnywhere)
	float MoveSpeed = 1000.f;

	UPROPERTY(category = "Centipede", EditAnywhere)
	FVector2D HitSwitch = FVector2D::Zero();

	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* PrevNode;
	UPROPERTY(category = "Centipede", EditAnywhere)
	ACTPCentiNode* NextNode;
	
	
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(float DeltaTime);

	void SwitchDirection();
	
		
};
