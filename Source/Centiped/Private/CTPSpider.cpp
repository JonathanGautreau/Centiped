// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPSpider.h"


// Sets default values
ACTPSpider::ACTPSpider()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Spider.SM_Spider"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}
	
	// ------- Override properties ------- //
	MeshScale = FVector2D(.4f,.4f);
	MoveSpeed = 1000;
	Life = 1;
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




