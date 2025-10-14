// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageCone.h"
#include "WalkingDamageCone.generated.h"

/**
 * Ходячий конус урона, который движется влево-вправо и меняет направление при столкновении.
 * Деактивируется при прыжке игрока сверху.
 */
UCLASS()
class MYMARIO_API AWalkingDamageCone : public ADamageCone
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalkingDamageCone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ===== Компоненты =====
	
	// Сферический коллайдер для обнаружения столкновений
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Walking Damage Cone Components")
	class USphereComponent* SphereCollider;

	// ===== Параметры движения =====
	
	// Скорость движения
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	float WalkSpeed;

	// Расстояние луча для обнаружения препятствий
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	float ObstacleDetectionDistance;

	// Активен ли конус
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	bool bIsActive;

	// Высота, при которой считается прыжок сверху (разница в Z-координате)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	float JumpFromAboveThreshold;

private:
	// Текущее направление движения (1 = вправо, -1 = влево)
	int32 CurrentDirection;

	// Функция проверки препятствий
	bool CheckForObstacle();

	// Функция смены направления
	void ChangeDirection();

	// Переопределяем функцию пересечения для обработки прыжка сверху
	UFUNCTION()
	void OnWalkingOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							   bool bFromSweep, const FHitResult& SweepResult);

	// Функция деактивации конуса
	void DeactivateCone();
};

