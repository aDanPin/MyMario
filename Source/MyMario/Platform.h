// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Platform.generated.h"

UCLASS()
class MYMARIO_API APlatform : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ===== Компоненты платформы =====
	
	// Статичный меш для визуализации платформы
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform Components")
	class UStaticMeshComponent* PlatformMesh;

	// Коллизия для платформы
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform Components")
	class UBoxComponent* CollisionBox;

	// ===== Параметры платформы =====
	
	// Флаг прозрачности снизу (позволяет игроку запрыгивать на платформу снизу)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	bool bBottomTransparent;

	// Флаг подвижности платформы
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	bool bMovable;

	// Цвет платформы
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	FLinearColor PlatformColor;

	// ===== Параметры движения (для подвижных платформ) =====
	
	// Скорость движения платформы
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float MovementSpeed;

	// Тип движения: 0 = горизонтальное, 1 = вертикальное, 2 = круговое
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	int32 MovementType;

	// Расстояние движения платформы (для линейного движения)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float MovementDistance;

	// Радиус движения платформы (для кругового движения)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float MovementRadius;

	// Пауза на конечных точках (для линейного движения)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float PauseAtEndPoints;

private:
	// Начальная позиция платформы
	FVector StartLocation;

	// Текущее направление движения (для линейного движения)
	int32 CurrentDirection; // 1 = вперед, -1 = назад

	// Текущий угол (для кругового движения)
	float CurrentAngle;

	// Время, проведенное в паузе
	float PauseTimer;

	// Флаг паузы
	bool bIsPaused;

	// Функция обновления позиции подвижной платформы
	void UpdateMovablePlatform(float DeltaTime);

	// Функция проверки, находится ли игрок сверху платформы
	bool IsActorAbovePlatform(AActor* Actor);
};

