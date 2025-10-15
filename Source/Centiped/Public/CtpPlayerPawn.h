// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CtpBullet.h"
#include "GameFramework/Pawn.h"
#include "CtpPlayerPawn.generated.h"

// Forward declaration avoids circular dependencies 
class UInputComponent;
class UInputAction;
struct FInputActionInstance;

UCLASS()
class CENTIPED_API ACtpPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACtpPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(Category="PlayerPawn", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionInstance& Instance);
	void Shoot(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	TObjectPtr<UInputAction> ShootAction;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	FVector2D MoveDirection = FVector2D::Zero();

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	float MoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	FVector2D MeshScale = FVector2D(.6f, 1.f);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerPawn")
	FVector MuzzleOffset;

protected:
	UPROPERTY(EditDefaultsOnly, Category="PlayerPawn")
	TSubclassOf<class ACtpBullet> ProjectileClass = ACtpBullet::StaticClass();
};
