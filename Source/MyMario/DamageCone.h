// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageCone.generated.h"

UCLASS()
class MYMARIO_API ADamageCone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageCone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ===== Компоненты конуса =====
	
	// Статичный меш для визуализации конуса
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage Cone Components")
	class UStaticMeshComponent* ConeMesh;

	// ===== Параметры конуса =====
	
	// Цвет конуса (по умолчанию красный)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Cone Settings")
	FLinearColor ConeColor;

private:
	// Функция обработки пересечения с другим актором
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
};

