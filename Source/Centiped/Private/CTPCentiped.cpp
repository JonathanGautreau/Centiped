// Fill out your copyright notice in the Description page of Project Settings.


#include "CTPCentiped.h"
#include "CTPMushroom.h"
#include "CtpGameMode.h"
#include "VisualizeTexture.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ACTPCentiped::ACTPCentiped()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACTPCentiped::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACTPCentiped::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RefreshMoveTimer+=DeltaTime;
	if (RefreshMoveTimer>RefreshMoveTime)
	{
		MoveHead(DeltaTime);
		MoveBody();
		RefreshMoveTimer=0;
	}
	
}

void ACTPCentiped::DevidedCentiped(int Index)
{
	//FActorSpawnParameters SpawnParameters;
	//SpawnParameters.Owner = Centipede[Index];
	//GetWorld()->SpawnActor<ACTPMushroom>(SpawnParameters);

	//Centipede[Index]->Destroy();
	
	
}

void ACTPCentiped::MoveHead(float DeltaTime)
{
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		bool IsBoundsReached=false;
		FVector2D NewLocation = FVector2D(Centipede[0]->GetActorLocation().Y, Centipede[0]->GetActorLocation().Z);
		NewLocation += HeadDirection * DeltaTime * MoveSpeed;
		if (NewLocation.X > GameMode->Bounds.Max.X)
		{
			SwitchDirection(FVector2D(-1,0));
			IsBoundsReached=true;
		}
		else if (NewLocation.X < GameMode->Bounds.Min.X)
		{
			SwitchDirection(FVector2D(1,0));
			IsBoundsReached=true;
		}
		NewLocation = NewLocation.Clamp(
			NewLocation,
			GameMode->Bounds.Min + 0.5f * MeshScale * 100,
			GameMode->Bounds.Max - 0.5f * MeshScale * 100
		);
		Centipede[0]->SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));
		if (IsBoundsReached)
		{
			Centipede[0]->SetActorLocation(FVector(0, NewLocation.X-1, NewLocation.Y));
		}
	}
}

void ACTPCentiped::MoveBody()
{	
	for (auto& i : Centipede)
	{
		if (i!=0)
		{
			i->SetActorLocation(Centipede[Centipede.IndexOfByKey(i)-1]->GetActorLocation());
		}
	}
}

void ACTPCentiped::SwitchDirection(FVector2D NewDirection)
{
	HeadDirection = NewDirection;
}

void ACTPCentiped::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(ACTPMushroom::StaticClass()))
	{		
		Centipede[0]->SetActorLocation(FVector(0,Centipede[0]->GetActorLocation().Y-1, Centipede[0]->GetActorLocation().Y));
		SwitchDirection(HeadDirection-1);
	}
}




