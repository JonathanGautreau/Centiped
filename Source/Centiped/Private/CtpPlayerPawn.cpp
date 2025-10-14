// Fill out your copyright notice in the Description page of Project Settings.


#include "Centiped/Public/CtpPlayerPawn.h"
#include "Centiped/Public/CTPLog.h"
#include "EnhancedInputComponent.h"
#include "Centiped/Public/CtpGameMode.h"
#include "Kismet/GameplayStatics.h"
// #include "InputAction.h"
// #include "InputTriggers.h"


// Sets default values
ACtpPlayerPawn::ACtpPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	// TODO : add Cube.Cube in UE5
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Game/Centiped/Meshes/Cube.Cube"));
	if (StaticMeshRef.Succeeded())
	{
		MeshComponent->SetStaticMesh(StaticMeshRef.Object);
	}

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	MeshComponent->SetGenerateOverlapEvents(true);
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
			GameMode->Bounds.Max + 0.5f * MeshScale * 100
		);

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
	// TODO
	MoveDirection = Instance.GetValue().Get<FVector2D>().GetSafeNormal();
}

void ACtpPlayerPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
}

