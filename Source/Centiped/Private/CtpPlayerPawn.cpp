// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpPlayerPawn.h"

#include "CtpMushroom.h"
#include "Centiped/Public/CTPLog.h"
#include "EnhancedInputComponent.h"
#include "Centiped/Public/CtpGameMode.h"
#include "Centiped/Public/CtpBullet.h"
#include "Kismet/GameplayStatics.h"


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
}

// Called when the game starts or when spawned
void ACtpPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		SetActorLocation(FVector(0, 0, -(GameMode->Height / 3.0f)));
	}
	else
	{
		SetActorLocation(FVector(0, 0, -900));
	}
	
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GenerateAvailableCells(GameMode);
			// A lot of mushrooms
			SpawnMushrooms(World, GameMode, 20, 0, GameMode->Rows - 20);
			// Some mushrooms
			SpawnMushrooms(World, GameMode, 8, GameMode->Rows - 21, GameMode->Rows - 6);
		}
	}
}

// Called every frame
void ACtpPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (const ACtpGameMode* GameMode = Cast<ACtpGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FVector2D NewLocation = FVector2D(GetActorLocation().Y, GetActorLocation().Z);
		NewLocation += MoveDirection * DeltaTime * MoveSpeed;

		NewLocation = NewLocation.Clamp(
			NewLocation,
			GameMode->Bounds.Min + 0.5f * MeshScale * 100,
			FVector2D(
				GameMode->Bounds.Max.X - 0.5f * MeshScale.X * 100,
				GameMode->Bounds.Max.Y - round(GameMode->SquareSize.Y * 28) - round(GameMode->SquareSize.Y * 0.5)
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

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input)
	{
		UE_LOG(LogCentiped, Error, TEXT("EnhancedInputComponent not found"));
		return;
	}

	UE_LOG(LogCentiped, Log, TEXT("Binding MoveAction: %s"), *MoveAction->GetName());
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACtpPlayerPawn::Move);
	UE_LOG(LogCentiped, Log, TEXT("Input binding complete"));

	UE_LOG(LogCentiped, Log, TEXT("Binding ShootAction: %s"), *ShootAction->GetName());
	Input->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ACtpPlayerPawn::Shoot);
	UE_LOG(LogCentiped, Log, TEXT("Input binding complete"));
}

void ACtpPlayerPawn::Move(const FInputActionInstance& Instance)
{
	MoveDirection = Instance.GetValue().Get<FVector2D>().GetSafeNormal();
}

void ACtpPlayerPawn::Shoot(const FInputActionInstance& Instance)
{
	UE_LOG(LogCentiped, Log, TEXT("Shoot"));

	if (ProjectileClass)
	{
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
 
			World->SpawnActor<ACtpBullet>(ProjectileClass, GetActorLocation(), FRotator(), SpawnParams);
		}
	}
}

void ACtpPlayerPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
}

void ACtpPlayerPawn::SpawnMushrooms(UWorld* World, const ACtpGameMode* GameMode, int NumberOfMushrooms, int RowMin, int RowMax)
{
	int SpawnedMushrooms = 0;
	
	while (SpawnedMushrooms < NumberOfMushrooms && AvailableCells.Num() > 0)
	{
		// Chose location
		const int Col = FMath::RandRange(0, GameMode->Columns - 1);
		const int Row = FMath::RandRange(RowMin, RowMax);

		// Remove chosen location from AvailableCells array
		AvailableCells.Remove(FIntPoint(Row, Col));

		// Safe zone for the player
		if (Col < 13 && Col > 7 && Row > 35)
		{
			SpawnedMushrooms--;
			UE_LOG(LogCentiped, Log, TEXT("SpawnedMushrooms : %d"), SpawnedMushrooms);
			continue;
		}
				
		// Create mushrooms
		ACtpMushroom* Mushroom = World->SpawnActor<ACtpMushroom>(ACtpMushroom::StaticClass());

		// Define position of the mushroom
		const int x = round(GameMode->Bounds.Min.X) + round(Mushroom->MeshScale.X * 100 * Col) + round(Mushroom->MeshScale.X * 0.5 * 100);
		const int y = round(GameMode->Bounds.Max.Y) - round(Mushroom->MeshScale.Y * 100 * Row) - round(Mushroom->MeshScale.Y * 0.5 * 100);
		Mushroom->InitializePosition(FVector(0, x, y));

		SpawnedMushrooms++;
	}
}

void ACtpPlayerPawn::GenerateAvailableCells(const ACtpGameMode* GameMode)
{
	AvailableCells.Empty();
	
	for (int Row = 0; Row < GameMode->Rows; ++Row)
	{
		for (int32 Col = 0; Col < GameMode->Columns; ++Col)
		{
			AvailableCells.Add(FIntPoint(Row, Col));
		}
	}
}
