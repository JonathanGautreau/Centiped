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

	void SetPlayerInitialPosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlayerMovements(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(Category="PlayerPawn", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(Category="PlayerPawn", VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	int LifeLeft = 3;

public:
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionInstance& Instance);
	void Shoot(const FInputActionInstance& Instance);
	void RestartGame(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	TObjectPtr<UInputAction> ShootAction;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	TObjectPtr<UInputAction> RestartAction;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	FVector2D MoveDirection = FVector2D::Zero();

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	float MoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	float ReloadTimeForShoot = .25f;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	float TimerLeftBeforeShoot;

	UPROPERTY(editAnywhere, Category="PlayerPawn")
	bool bIsPlayerCanShoot = true;

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	FVector2D MeshScale = FVector2D(0.8f, 1.2f);

	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	bool bIsOverlappedByEnemy = false;

	int GetLife() const { return LifeLeft; };
	void SetLife(const int NewLifeLeft) { LifeLeft = NewLifeLeft; };
	void LoseLife();
	void GainLife();

protected:
	UPROPERTY(EditDefaultsOnly, Category="PlayerPawn")
	TSubclassOf<ACtpBullet> ProjectileClass = ACtpBullet::StaticClass();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
