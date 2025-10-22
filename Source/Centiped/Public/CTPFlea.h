// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTPEnnemie.h"
#include "GameFramework/Actor.h"
#include "CTPFlea.generated.h"

UCLASS()
class CENTIPED_API ACTPFlea : public ACTPEnnemie
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPFlea();

	FVector2D HitSwitch;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Move(float DeltaTime) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void HitMushroom(AActor* OtherActor) override;
	virtual void HitPLayer(AActor* OtherActor) override;
	virtual void HitBullet(AActor* OtherActor) override;
};
