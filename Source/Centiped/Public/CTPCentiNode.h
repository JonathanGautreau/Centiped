// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTPCentiNode.generated.h"

UCLASS()
class CENTIPED_API ACTPCentiNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACTPCentiNode();

	
	UPROPERTY(Category="Centipede", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category="Centipede")
	FVector2D MeshScale = FVector2D(.6f, 1.f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
	
};
