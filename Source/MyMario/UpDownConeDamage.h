// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageCone.h"
#include "UpDownConeDamage.generated.h"

/**
 * Класс конуса урона, который циклически движется вверх и вниз
 * Каждые 3 секунды поднимается на высоту своего меша, затем 1.5 секунды опускается обратно
 */
UCLASS()
class MYMARIO_API AUpDownConeDamage : public ADamageCone
{
	GENERATED_BODY()
	
public:
	// Конструктор
	AUpDownConeDamage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ===== Параметры движения =====
	
	// Время ожидания перед подъемом (в секундах)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
	float WaitTime;

	// Время опускания (в секундах)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
	float FallTime;

private:
	// Таймер для управления циклом движения
	FTimerHandle CycleTimerHandle;

	// Таймер для опускания
	FTimerHandle FallTimerHandle;

	// Начальная позиция конуса
	FVector InitialPosition;

	// Целевая позиция (верхняя точка)
	FVector TargetPosition;

	// Высота подъема (высота меша)
	float MeshHeight;

	// Прогресс опускания (от 0 до 1)
	float FallProgress;

	// Находится ли конус в процессе опускания
	bool bIsFalling;

	// Функция запуска цикла подъема
	void StartRiseCycle();

	// Функция мгновенного подъема и начала опускания
	void RiseAndStartFall();

	// Функция обновления опускания
	void UpdateFalling(float DeltaTime);

	// Получить высоту меша конуса
	float GetMeshHeight();
};

