// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTPCentiNode.h"

#include "CTPCentiped.generated.h"





UCLASS()
class CENTIPED_API ACTPCentiped : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPCentiped();

	UPROPERTY(Category="Centiped",EditAnywhere)
	TArray<ACTPCentiNode*> Centipede;

	UPROPERTY( Category = "Centiped",EditAnywhere)
	FVector2D HeadDirection;

	UPROPERTY(Category="Centiped",EditAnywhere)
	float MoveSpeed;

	UPROPERTY(Category="Centiped",EditAnywhere)
	float RefreshMoveTime = .5f;
	UPROPERTY(Category="Centiped",EditAnywhere)
	float RefreshMoveTimer;

	UPROPERTY(EditAnywhere, Category="Centipede")
	FVector2D MeshScale = FVector2D(.6f, 1.f);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DevidedCentiped(int Index);

	void MoveHead(float DeltaTime);

	void MoveBody();

	void SwitchDirection(FVector2D NewDirection);
	
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
};
