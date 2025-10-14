// Fill out your copyright notice in the Description page of Project Settings.

#include "UpDownConeDamage.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Конструктор
AUpDownConeDamage::AUpDownConeDamage()
{
	// Включаем Tick для обновления опускания
	PrimaryActorTick.bCanEverTick = true;

	// Устанавливаем параметры движения по умолчанию
	WaitTime = 3.0f;      // 3 секунды ожидания перед подъемом
	FallTime = 1.5f;      // 1.5 секунды на опускание

	// Инициализация переменных
	MeshHeight = 0.0f;
	FallProgress = 0.0f;
	bIsFalling = false;
}

// Called when the game starts or when spawned
void AUpDownConeDamage::BeginPlay()
{
	Super::BeginPlay();
	
	// Сохраняем начальную позицию
	InitialPosition = GetActorLocation();

	// Получаем высоту меша
	MeshHeight = GetMeshHeight();

	// Рассчитываем целевую позицию (поднимаемся на высоту меша)
	TargetPosition = InitialPosition + FVector(0.0f, 0.0f, MeshHeight);

	UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Initialized at %s, MeshHeight: %f"), 
		*InitialPosition.ToString(), MeshHeight);

	// Запускаем первый цикл
	StartRiseCycle();
}

// Called every frame
void AUpDownConeDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Обновляем опускание, если оно происходит
	if (bIsFalling)
	{
		UpdateFalling(DeltaTime);
	}
}

// Функция запуска цикла подъема
void AUpDownConeDamage::StartRiseCycle()
{
	// Устанавливаем таймер на WaitTime секунд, после чего произойдет подъем
	GetWorldTimerManager().SetTimer(CycleTimerHandle, this, &AUpDownConeDamage::RiseAndStartFall, 
									 WaitTime, false);
	
	UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Waiting %f seconds before rise"), WaitTime);
}

// Функция мгновенного подъема и начала опускания
void AUpDownConeDamage::RiseAndStartFall()
{
	// Мгновенно поднимаемся в верхнюю точку (RiseTime = 0)
	SetActorLocation(TargetPosition);
	
	UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Instantly risen to %s"), *TargetPosition.ToString());

	// Начинаем процесс опускания
	bIsFalling = true;
	FallProgress = 0.0f;
}

// Функция обновления опускания
void AUpDownConeDamage::UpdateFalling(float DeltaTime)
{
	// Увеличиваем прогресс опускания
	FallProgress += DeltaTime / FallTime;
	FallProgress = FMath::Clamp(FallProgress, 0.0f, 1.0f);

	// Интерполируем позицию от целевой до начальной
	FVector NewLocation = FMath::Lerp(TargetPosition, InitialPosition, FallProgress);
	SetActorLocation(NewLocation);

	// Если опускание завершено
	if (FallProgress >= 1.0f)
	{
		bIsFalling = false;
		SetActorLocation(InitialPosition); // Гарантируем точное положение
		
		UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Returned to initial position"));

		// Запускаем следующий цикл
		StartRiseCycle();
	}
}

// Получить высоту меша конуса
float AUpDownConeDamage::GetMeshHeight()
{
	if (ConeMesh && ConeMesh->GetStaticMesh())
	{
		// Получаем границы меша
		FBoxSphereBounds Bounds = ConeMesh->GetStaticMesh()->GetBounds();
		
		// Возвращаем высоту меша с учетом масштаба
		FVector Scale = ConeMesh->GetComponentScale();
		float Height = Bounds.BoxExtent.Z * 2.0f * Scale.Z;
		
		return Height;
	}

	// Если меш не найден, возвращаем значение по умолчанию
	return 100.0f;
}

