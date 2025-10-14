// Fill out your copyright notice in the Description page of Project Settings.

#include "WalkingDamageCone.h"
#include "Mario.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWalkingDamageCone::AWalkingDamageCone()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Создаем сферический коллайдер
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	if (SphereCollider)
	{
		// Устанавливаем радиус сферы
		SphereCollider->InitSphereRadius(100.0f);
		
		// Прикрепляем к корневому компоненту
		SphereCollider->SetupAttachment(RootComponent);
		
		// Настройка коллизии для сферы
		SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		SphereCollider->SetGenerateOverlapEvents(true);
	}

	// Устанавливаем параметры по умолчанию
	WalkSpeed = 200.0f;
	ObstacleDetectionDistance = 100.0f;
	bIsActive = true;
	JumpFromAboveThreshold = 50.0f; // Минимальная разница в высоте для прыжка сверху
	CurrentDirection = 1; // Начинаем с движения вправо
}

// Called when the game starts or when spawned
void AWalkingDamageCone::BeginPlay()
{
	Super::BeginPlay();
	


	// Привязываем обработчик пересечений для сферического коллайдера
	if (SphereCollider)
	{
		SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AWalkingDamageCone::OnWalkingOverlapBegin);
		
		UE_LOG(LogTemp, Log, TEXT("WalkingDamageCone: SphereCollider initialized with radius %f"), 
			SphereCollider->GetScaledSphereRadius());
	}
}

// Called every frame
void AWalkingDamageCone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Если конус неактивен, не двигаемся
	if (!bIsActive)
	{
		return;
	}

	// Проверяем наличие препятствия впереди
	if (CheckForObstacle())
	{
		// Если обнаружено препятствие, меняем направление
		ChangeDirection();
	}
	
	// Вычисляем движение
	FVector CurrentLocation = GetActorLocation();
	// Движение по оси Y (влево-вправо в 2D игре)
	FVector Movement = FVector(0.0f, CurrentDirection * WalkSpeed * DeltaTime, 0.0f);
	
	// Применяем движение
	SetActorLocation(CurrentLocation + Movement);
}

bool AWalkingDamageCone::CheckForObstacle()
{
	if (!ConeMesh)
	{
		return false;
	}

	// Получаем текущую позицию
	FVector StartLocation = GetActorLocation();
	
	// Конечная точка луча в направлении движения
	FVector EndLocation = StartLocation + FVector(0.0f, CurrentDirection * ObstacleDetectionDistance, 0.0f);

	// Параметры трассировки
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Игнорируем себя

	// Выполняем трассировку
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_WorldStatic, // Проверяем только статичные объекты (стены, платформы)
		QueryParams
	);


	if (bHit && HitResult.GetActor()->IsA(AMario::StaticClass()))
	{
		return false;
	}

	// Опционально: рисуем отладочную линию (можно закомментировать)
	#if WITH_EDITOR
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, 
				  bHit ? FColor::Red : FColor::Green, 
				  false, 0.1f, 0, 2.0f);
	#endif

	return bHit;
}

void AWalkingDamageCone::ChangeDirection()
{
	// Меняем направление на противоположное
	CurrentDirection *= -1;

	// Поворачиваем меш конуса в направлении движения
	if (ConeMesh)
	{
		FRotator NewRotation = ConeMesh->GetRelativeRotation();
		if (CurrentDirection == 1) // Вправо
		{
			NewRotation.Yaw = 0.0f;
		}
		else // Влево
		{
			NewRotation.Yaw = 180.0f;
		}
		ConeMesh->SetRelativeRotation(NewRotation);
	}
}

void AWalkingDamageCone::OnWalkingOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
											   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
											   bool bFromSweep, const FHitResult& SweepResult)
{
	// Проверяем, что пересеклись с Mario
	AMario* Mario = Cast<AMario>(OtherActor);
	if (Mario && bIsActive)
	{
		// Получаем позиции для проверки прыжка сверху
		FVector MarioLocation = Mario->GetActorLocation();
		FVector ConeLocation = GetActorLocation();
		
		// Проверяем, находится ли Mario выше конуса
		float HeightDifference = MarioLocation.Z - ConeLocation.Z;
		
		// Проверяем, движется ли Mario вниз (падает)
		FVector MarioVelocity = Mario->GetVelocity();
		bool bIsFalling = MarioVelocity.Z < 0.0f;

		// Если Mario выше и падает - это прыжок сверху
		if (HeightDifference > JumpFromAboveThreshold && bIsFalling)
		{
			// Деактивируем конус
			DeactivateCone();
			
			// Опционально: можно дать Mario небольшой отскок
			Mario->LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, true);
		}
		else
		{
			// Обычное столкновение - наносим урон
			Mario->TriggerDamage();
		}
	}	
}

void AWalkingDamageCone::DeactivateCone()
{
	bIsActive = false;
	
	// Отключаем видимость
	if (ConeMesh)
	{
		ConeMesh->SetVisibility(false);
		ConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Отключаем коллизию сферы
	if (SphereCollider)
	{
		SphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Опционально: можно добавить эффект исчезновения или звук
	// Или через некоторое время уничтожить актор
	// SetLifeSpan(1.0f); // Уничтожить через 1 секунду
}

