// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPSpider.h"


// Sets default values
ACTPSpider::ACTPSpider()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPSpider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACTPSpider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTPSpider::Move(float Deltatime)
{
	
}

void ACTPSpider::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ACTPSpider::HitMushroom(ACtpMushroom* Mushroom)
{
	Super::HitMushroom(Mushroom);
}

void ACTPSpider::HitPlayer(ACtpPlayerPawn* Player)
{
	Super::HitPlayer(Player);
}

void ACTPSpider::HitBullet(ACtpBullet* Bullet)
{
	Super::HitBullet(Bullet);
}




