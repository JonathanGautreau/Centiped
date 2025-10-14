// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpCentipede.h"


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
	
}

// Called every frame
void ACtpCentipede::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACtpCentipede::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

