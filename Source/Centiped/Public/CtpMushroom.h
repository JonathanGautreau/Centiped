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
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UPROPERTY(Category="Mushroom", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializePosition(const FVector& InitialPosition);
	void CheckOnDestroyed();
	void BecomePoison();
	void BecomeNormal();

	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	
	UPROPERTY(EditAnywhere, Category="Mushroom")
	FVector2D MeshScale = FVector2D(.8f, .8f);
	
	UPROPERTY(editAnywhere, Category="Mushroom")
	UStaticMesh* MeshMushroom;
	UPROPERTY(editAnywhere, Category="Mushroom")
	UStaticMesh* MeshMushroomDamaged;
	UPROPERTY(editAnywhere, Category="Mushroom")
	UStaticMesh* MeshMushroomHeavilyDamaged;

	UPROPERTY(EditAnywhere, Category="Mushroom")
	UMaterialInstance* MatInstNormalMushroom;
	UPROPERTY(EditAnywhere, Category="Mushroom")
	UMaterialInstance* MatInstPoisonnedMushroom;

	UPROPERTY(EditAnywhere, Category="Mushroom")
	int Life = 3;

	UPROPERTY(editAnywhere, Category="Mushroom")
	bool bIsPoison;
};
