// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPCentiNode.h"


// Sets default values
ACTPCentiNode::ACTPCentiNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/Centiped.Centiped"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACTPCentiNode::BeginPlay()
{
	Super::BeginPlay();
	
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;

	GetWorld()->SpawnActor<ACTPCentiNode>(SpawnParameters);
		
		

	
	
}

// Called every frame
void ACTPCentiNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




