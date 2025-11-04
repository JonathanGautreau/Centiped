// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTPEnemy.h"
#include "CtpPlayerPawn.h"
#include "CtpMushroom.h"
#include "CtpBullet.h"
#include "GameFramework/Actor.h"
#include "CTPFlea.generated.h"

UCLASS()
class CENTIPED_API ACTPFlea : public ACTPEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPFlea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ------- Specific properties ------- //

public:
	// ------- Common functions ------- //
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Move(float DeltaTime) override;
	virtual void HitMushroom(ACtpMushroom* Mushroom) override;
	virtual void HitPlayer(ACtpPlayerPawn* Player) override;
	virtual void HitBullet(ACtpBullet* Bullet) override;

	// ------- Specific properties ------- //
	UPROPERTY(category = "Flea", EditAnywhere)
	float VerticalOffset = 80.f;

	UPROPERTY(category = "Flea", EditAnywhere)
	UStaticMesh* DamagedMesh;
};
