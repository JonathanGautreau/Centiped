// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CtpBullet.generated.h"

UCLASS()
class CENTIPED_API ACtpBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACtpBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void BulletVelocity(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(Category="Bullet", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UPROPERTY(EditAnywhere, Category="Bullet")
	float MoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category="Bullet")
	float Radius = .2f;
};
