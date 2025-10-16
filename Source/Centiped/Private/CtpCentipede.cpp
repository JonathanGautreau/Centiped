// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpCentipede.h"
#include "CtpGameMode.h"


// Sets default values
ACtpCentipede::ACtpCentipede()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACtpCentipede::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	ACTPCentiNode* Prev = nullptr;
	for (int i = 0; i < CentiSize ; ++i )
	{
		ACTPCentiNode* Curr = GetWorld()->SpawnActor<ACTPCentiNode>(SpawnParameters);
		
		Curr->PrevNode = Prev;
		if (Prev)
		{
			Prev->NextNode = Curr;
		}
		Prev = Curr;
	}
	
	
}

// Called every frame
void ACtpCentipede::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}




