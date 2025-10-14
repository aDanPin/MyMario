// Fill out your copyright notice in the Description page of Project Settings.

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

// Sets default values
APlatform::APlatform()
{
 	// Set this pawn to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Создание коллизионного бокса (корневой компонент)
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(1.0f, 1.0f, 0.1f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
	
	// Создание статичного меша
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(CollisionBox);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Инициализация параметров по умолчанию
	bBottomTransparent = false;
	bMovable = false;
	PlatformColor = FLinearColor::White; // Белый цвет по умолчанию
	MovementSpeed = 100.0f;
	MovementType = 0; // Горизонтальное движение
	MovementDistance = 200.0f;
	MovementRadius = 100.0f;
	PauseAtEndPoints = 0.0f;

	// Инициализация внутренних переменных
	CurrentDirection = 1;
	CurrentAngle = 0.0f;
	PauseTimer = 0.0f;
	bIsPaused = false;
}

// Called when the game starts or when spawned
void APlatform::BeginPlay()
{
	Super::BeginPlay();
	
	// Сохраняем начальную позицию
	StartLocation = GetActorLocation();

	// Применяем цвет к платформе
	if (PlatformMesh)
	{
		// Создаем динамический материал
		UMaterialInterface* Material = PlatformMesh->GetMaterial(0);
		if (Material)
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			if (DynamicMaterial)
			{
				// Устанавливаем цвет через параметр "Color" или "BaseColor"
				DynamicMaterial->SetVectorParameterValue(FName("Color"), PlatformColor);
				DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), PlatformColor);
				PlatformMesh->SetMaterial(0, DynamicMaterial);
			}
		}
		else
		{
			// Если материала нет, создаем простой цветной материал
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(
				LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial")), this);
			if (DynamicMaterial)
			{
				DynamicMaterial->SetVectorParameterValue(FName("Color"), PlatformColor);
				PlatformMesh->SetMaterial(0, DynamicMaterial);
			}
		}
	}

	// Обработчики коллизии больше не нужны - логика в Tick()
}

// Called every frame
void APlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Обновляем позицию, если платформа подвижная
	if (bMovable)
	{
		UpdateMovablePlatform(DeltaTime);
	}

	// Получаем главного персонажа
	if (bBottomTransparent)
	{
		AMario* MainCharacter = Cast<AMario>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (MainCharacter)
		{
			// Если нижняя часть персонажа выше верхней границы платформы - он полностью прошел
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
	// Если платформа на паузе
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
		case 0: // Горизонтальное движение (по оси X)
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
		
		case 1: // Вертикальное движение (по оси Z)
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
		
		case 2: // Круговое движение
		{
			CurrentAngle += (MovementSpeed / MovementRadius) * DeltaTime;
			
			// Нормализуем угол
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

	// Получаем позиции актора и платформы
	FVector ActorLocation = Actor->GetActorLocation();

	// Если актор имеет капсульный компонент, учитываем его половину высоты (half height) при расчетах
	const UCapsuleComponent* Capsule = Actor->FindComponentByClass<UCapsuleComponent>();
	float CapsuleHalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.0f;
	ActorLocation.Z -= CapsuleHalfHeight/2 + 10.0f;
	FVector PlatformLocation = GetActorLocation();
	FVector BoxExtent = CollisionBox->GetScaledBoxExtent();

	// Проверяем, находится ли актор выше верхней границы платформы
	float PlatformTopZ = PlatformLocation.Z + BoxExtent.Z;
	
	// Используем небольшой порог для определения "сверху"
	float Threshold = 10.0f;
	
	// Проверяем, что персонаж по X и Y находится в пределах левой и правой границы платформы
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

