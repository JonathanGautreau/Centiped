// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CtpPlayerPawn.generated.h"

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
	UPROPERTY(Category="Centipede", VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionInstance& Instance);
	void Shoot(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category="Centipede")
	TObjectPtr<UInputAction> MoveAction;
};
