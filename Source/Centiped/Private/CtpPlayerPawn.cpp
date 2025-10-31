// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpPlayerPawn.h"
#include "CTPCentiNode.h"
#include "CTPEnemy.h"
#include "CtpHud.h"
#include "CtpMushroom.h"
#include "Centiped/Public/CTPLog.h"
#include "EnhancedInputComponent.h"
#include "Centiped/Public/CtpGameMode.h"
#include "Centiped/Public/CtpBullet.h"


// Sets default values
ACtpPlayerPawn::ACtpPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/SM_Player.SM_Player"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}

	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	MeshComponent->SetCollisionObjectType(ECC_Pawn);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // Collisions with Mushrooms
	MeshComponent->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap); // Collisions with Centipede
	
	MeshComponent->SetRelativeScale3D(FVector(1, MeshScale.X, MeshScale.Y));
	MeshComponent->SetDefaultCustomPrimitiveDataVector4(0,FVector4(0.2f, 0.2f, 0, 1.0f));
	MeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Game/Centiped/Inputs/IA_Move.IA_Move"));
	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
		UE_LOG(LogCentiped, Log, TEXT("MoveAction loaded successfully in constructor"));
	}
	else
	{
		UE_LOG(LogCentiped, Error, TEXT("Failed to load MoveAction from /Game/Centiped/Inputs/IA_Move"));
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> ShootActionRef(TEXT("/Game/Centiped/Inputs/IA_Shoot.IA_Shoot"));
	if (ShootActionRef.Succeeded())
	{
		ShootAction = ShootActionRef.Object;
		UE_LOG(LogCentiped, Log, TEXT("ShootAction loaded successfully in constructor"));
	}
	else
	{
		UE_LOG(LogCentiped, Error, TEXT("Failed to load ShootAction from /Game/Centiped/Inputs/IA_Shoot"));
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RestartActionRef(TEXT("/Game/Centiped/Inputs/IA_Restart.IA_Restart"));
	if (RestartActionRef.Succeeded())
	{
		RestartAction = RestartActionRef.Object;
		UE_LOG(LogCentiped, Log, TEXT("RestartAction loaded successfully in constructor"));
	}
	else
	{
		UE_LOG(LogCentiped, Error, TEXT("Failed to load RestartAction from /Game/Centiped/Inputs/IA_Restart"));
	}
}

// Called when the game starts or when spawned
void ACtpPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerInitialPosition();
}

// Called every frame
void ACtpPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsOverlappingMushroom)
	{
		LastSafeLocation = GetActorLocation();
	}
	bIsOverlappingMushroom = false;

	PlayerMovements(DeltaTime);
}

void ACtpPlayerPawn::SetPlayerInitialPosition()
{
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		SetActorLocation(FVector(0, 0, GameMode->Bounds.Min.Y + MeshScale.Y * 100 * 0.5f));
	}
	else
	{
		SetActorLocation(FVector(0, 0, -820));
	}
}

void ACtpPlayerPawn::PlayerMovements(float DeltaTime)
{
	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
		NewLocation += MoveDirection * DeltaTime * MoveSpeed;

		NewLocation = NewLocation.Clamp(
			NewLocation,
			GameMode->Bounds.Min + 0.5f * MeshScale * 100,
			FVector2D(
				GameMode->Bounds.Max.X - 0.5f * MeshScale.X * 100,
				GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * FMath::RoundToInt(GameMode->Rows * 0.7f)) - round(GameMode->SquareSize.Y * 0.5)
			));
		SetActorLocation(FVector(0, NewLocation.X, NewLocation.Y));

		MoveDirection = FVector2D::Zero();
	}
}

// Called to bind functionality to input
void ACtpPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogCentiped, Log, TEXT("SetupPlayerInputComponent called on Pawn"));

	if (!MoveAction)
	{
		UE_LOG(LogCentiped, Error, TEXT("MoveAction is null! Make sure IA_Move is assigned in Blueprint"));
		return;
	}
	if (!ShootAction)
	{
		UE_LOG(LogCentiped, Error, TEXT("ShootAction is null! Make sure IA_Shoot is assigned in Blueprint"));
		return;
	}
	if (!RestartAction)
	{
		UE_LOG(LogCentiped, Error, TEXT("RestartAction is null! Make sure IA_Shoot is assigned in Blueprint"));
		return;
	}

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input)
	{
		UE_LOG(LogCentiped, Error, TEXT("EnhancedInputComponent not found"));
		return;
	}

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACtpPlayerPawn::Move);
	Input->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ACtpPlayerPawn::Shoot);
	Input->BindAction(RestartAction, ETriggerEvent::Triggered, this, &ACtpPlayerPawn::RestartGame);
}

void ACtpPlayerPawn::Move(const FInputActionInstance& Instance)
{
	MoveDirection = Instance.GetValue().Get<FVector2D>().GetSafeNormal();
}

void ACtpPlayerPawn::Shoot(const FInputActionInstance& Instance)
{
	UE_LOG(LogCentiped, Log, TEXT("Shoot"));

	if (bIsOverlappedByEnemy)
		return;

	if (ProjectileClass)
	{
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			const FVector InitialPosition = FVector(0, GetActorLocation().Y, GetActorLocation().Z + MeshScale.Y * 100 * 0.5f);
			World->SpawnActor<ACtpBullet>(ProjectileClass, InitialPosition, FRotator(), SpawnParams);
		}
	}
}

void ACtpPlayerPawn::RestartGame(const FInputActionInstance& Instance)
{
	if (GetLife() != 0)
		return;
	
	if (UWorld* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!PlayerController) return;
		
		if (ACtpHud* HUD = Cast<ACtpHud>(PlayerController->GetHUD()))
		{
			HUD->ShowGameOverText(false);
		}
	}

	if (ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UCtpGameLoop* GameLoop = GameMode->GetGameLoop())
		{
			GameLoop->RestartGame();
		}
	}
}

void ACtpPlayerPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogCentiped, Log, TEXT("%s is  overlapping : %s"), *this->GetName(), *OtherActor->GetName());
		if (Cast<ACtpMushroom>(OtherActor))
		{
			bIsOverlappingMushroom = true;
			SetActorLocation(LastSafeLocation);
			// GetWorld()->SweepMultiByChannel()
			// GetWorld()->LineTraceMultiByChannel()
		}
	}
}

void ACtpPlayerPawn::LoseLife()
{
	SetLife(GetLife() - 1);
}

void ACtpPlayerPawn::GainLife()
{
	SetLife(GetLife() + 1);
}
