#include "Platform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Mario.h"

APlatform::APlatform()
{
 	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(1.0f, 1.0f, 0.1f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
	
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(CollisionBox);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bBottomTransparent = false;
	bMovable = false;
	PlatformColor = FLinearColor::White;
	MovementSpeed = 100.0f;
	MovementType = 0;
	MovementDistance = 200.0f;
	MovementRadius = 100.0f;
	PauseAtEndPoints = 0.0f;

	CurrentDirection = 1;
	CurrentAngle = 0.0f;
	PauseTimer = 0.0f;
	bIsPaused = false;
}

void APlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();

	if (PlatformMesh)
	{
		UMaterialInterface* Material = PlatformMesh->GetMaterial(0);
		if (Material)
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			if (DynamicMaterial)
			{
				DynamicMaterial->SetVectorParameterValue(FName("Color"), PlatformColor);
				DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), PlatformColor);
				PlatformMesh->SetMaterial(0, DynamicMaterial);
			}
		}
		else
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(
				LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial")), this);
			if (DynamicMaterial)
			{
				DynamicMaterial->SetVectorParameterValue(FName("Color"), PlatformColor);
				PlatformMesh->SetMaterial(0, DynamicMaterial);
			}
		}
	}

}

void APlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMovable)
	{
		UpdateMovablePlatform(DeltaTime);
	}

	if (bBottomTransparent)
	{
		AMario* MainCharacter = Cast<AMario>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (MainCharacter)
		{
			if (IsActorAbovePlatform(MainCharacter))
			{
                CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
            }
			else
			{
                CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
			}
		}
	}
}

void APlatform::UpdateMovablePlatform(float DeltaTime)
{
	if (bIsPaused)
	{
		PauseTimer += DeltaTime;
		if (PauseTimer >= PauseAtEndPoints)
		{
			bIsPaused = false;
			PauseTimer = 0.0f;
		}
		return;
	}

	FVector NewLocation = GetActorLocation();

	switch (MovementType)
	{
		case 0:
		{
			float DistanceFromStart = FMath::Abs(NewLocation.Y - StartLocation.Y);
			
			if (DistanceFromStart >= MovementDistance)
			{
				// Достигли конечной точки
				CurrentDirection *= -1;
				
				// Если установлена пауза на конечных точках
				if (PauseAtEndPoints > 0.0f)
				{
					bIsPaused = true;
					PauseTimer = 0.0f;
				}
			}

			NewLocation.Y += CurrentDirection * MovementSpeed * DeltaTime;
			break;
		}
		
		case 1:
		{
			float DistanceFromStart = FMath::Abs(NewLocation.Z - StartLocation.Z);
			
			if (DistanceFromStart >= MovementDistance)
			{
				// Достигли конечной точки
				CurrentDirection *= -1;
				
				// Если установлена пауза на конечных точках
				if (PauseAtEndPoints > 0.0f)
				{
					bIsPaused = true;
					PauseTimer = 0.0f;
				}
			}

			NewLocation.Z += CurrentDirection * MovementSpeed * DeltaTime;
			break;
		}
		
		case 2:
		{
			CurrentAngle += (MovementSpeed / MovementRadius) * DeltaTime;
			
			if (CurrentAngle >= 2.0f * PI)
			{
				CurrentAngle -= 2.0f * PI;
			}

			NewLocation.X = StartLocation.X + MovementRadius * FMath::Cos(CurrentAngle);
			NewLocation.Z = StartLocation.Z + MovementRadius * FMath::Sin(CurrentAngle);
			break;
		}
	}

	SetActorLocation(NewLocation);
}

bool APlatform::IsActorAbovePlatform(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	FVector ActorLocation = Actor->GetActorLocation();

	const UCapsuleComponent* Capsule = Actor->FindComponentByClass<UCapsuleComponent>();
	float CapsuleHalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.0f;
	ActorLocation.Z -= CapsuleHalfHeight/2 + 10.0f;
	FVector PlatformLocation = GetActorLocation();
	FVector BoxExtent = CollisionBox->GetScaledBoxExtent();

	float PlatformTopZ = PlatformLocation.Z + BoxExtent.Z;
	
	float Threshold = 10.0f;
	
	float PlatformMinX = PlatformLocation.X - BoxExtent.X;
	float PlatformMaxX = PlatformLocation.X + BoxExtent.X;
	float PlatformMinY = PlatformLocation.Y - BoxExtent.Y;
	float PlatformMaxY = PlatformLocation.Y + BoxExtent.Y;

	if (ActorLocation.X < PlatformMinX || ActorLocation.X > PlatformMaxX)
	{
		return false;
	}
	if (ActorLocation.Y < PlatformMinY || ActorLocation.Y > PlatformMaxY)
	{
		return false;
	}

    
	return ActorLocation.Z < PlatformTopZ + Threshold;
}

