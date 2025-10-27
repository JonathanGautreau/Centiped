// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTPEnemy.h"
#include "GameFramework/Actor.h"
#include "CTPSpider.generated.h"

UCLASS()
class CENTIPED_API ACTPSpider : public ACTPEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPSpider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ------- Common functions ------- //
	virtual void Move(float Deltatime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void HitMushroom(ACtpMushroom* Mushroom) override;
	virtual void HitPlayer(ACtpPlayerPawn* Player) override;
	virtual void HitBullet(ACtpBullet* Bullet) override;

	// ------- Specific properties ------- //
	UPROPERTY(EditAnywhere, category ="Spider")
	float DistToPlayer;

	UPROPERTY(EditAnywhere, category ="Spider")
	float FirstLayerPoint;

	UPROPERTY(EditAnywhere, category ="Spider")
	float LastLayerPoint;

	UPROPERTY(EditAnywhere,Category="Spider")
	bool  IsLeftDirection;
	
};
