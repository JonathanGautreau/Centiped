// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CtpBullet.h"
#include "GameFramework/Actor.h"
#include "CtpPlayerPawn.h"
#include "CtpMushroom.h"
#include "CTPEnemy.generated.h"

UCLASS()
class CENTIPED_API ACTPEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPEnemy();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void  NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void Move(float Deltatime);
	virtual void HitMushroom(ACtpMushroom* OtherActor);
	virtual void HitPLayer(ACtpPlayerPawn* OtherActor);
	virtual void HitBullet(ACtpBullet* OtherActor);

	UPROPERTY(Category="Enemy", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	int Life = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	FVector2D MeshScale = FVector2D(.8f, .8f);

	UPROPERTY(category = "Enemy", EditAnywhere)
	FVector2D HitSwitch = FVector2D::Zero();
};
