// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CtpBullet.h"
#include "GameFramework/Actor.h"
#include "CtpPlayerPawn.h"
#include "CtpMushroom.h"
#include "CTPEnnemie.generated.h"

UCLASS()
class CENTIPED_API ACTPEnnemie : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPEnnemie();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	virtual void  NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void HitMushroom(ACtpMushroom* OtherActor);
	
	virtual void HitPLayer(ACtpPlayerPawn* OtherActor);
	
	virtual void HitBullet(ACtpBullet* OtherActor);
	
	// Méthodes communes
	virtual void Move(float Deltatime);     // Peut être redéfinie
	
	// Propriétés communes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Centiped")
	float Life;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Centiped")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Centiped")
	FVector MeshScale;

	UPROPERTY(Category="Centiped", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
};
