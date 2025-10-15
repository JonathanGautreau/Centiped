// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CtpMushroom.generated.h"

UCLASS()
class CENTIPED_API ACtpMushroom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACtpMushroom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(Category="PlayerPawn", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	
	UPROPERTY(EditAnywhere, Category="PlayerPawn")
	FVector2D MeshScale = FVector2D(1.f, .6f);
};
