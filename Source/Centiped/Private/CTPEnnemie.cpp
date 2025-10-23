// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPEnnemie.h"
#include "CtpMushroom.h"
#include "CtpPlayerPawn.h"
#include "CtpBullet.h"


// Sets default values
ACTPEnnemie::ACTPEnnemie(): Life(0), MoveSpeed(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPEnnemie::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACTPEnnemie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPEnnemie::Move(float DeltaTime)
{
	
}

void ACTPEnnemie::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if (const ACtpMushroom* Mushroom = Cast<ACtpMushroom>(OtherActor))
		{
			HitMushroom(OtherActor);
		}
		if (const ACtpPlayerPawn* PlayerPawn = Cast<ACtpPlayerPawn>(OtherActor))
		{
			HitPLayer(OtherActor);
		}
		if (const ACtpBullet* Bullet = Cast<ACtpBullet>(OtherActor))
		{
			HitBullet(OtherActor);
		}
	}
}

void ACTPEnnemie::HitMushroom(AActor* OtherActor)
{
	
}
void ACTPEnnemie::HitPLayer(AActor* OtherActor)
{
	
}
void ACTPEnnemie::HitBullet(AActor* OtherActor)
{
	
}


