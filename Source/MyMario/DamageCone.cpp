// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageCone.h"
#include "Mario.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ADamageCone::ADamageCone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Создаем компонент статичного меша для конуса
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
	RootComponent = ConeMesh;

	// Загружаем меш конуса из стандартных ассетов движка
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone"));
	if (ConeMeshAsset.Succeeded())
	{
		ConeMesh->SetStaticMesh(ConeMeshAsset.Object);
	}

	// Настройка коллизии
	ConeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ConeMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ConeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ConeMesh->SetGenerateOverlapEvents(true);
	ConeMesh->SetNotifyRigidBodyCollision(true); // Включить уведомления о физических столкновениях

	// Устанавливаем красный цвет по умолчанию
	ConeColor = FLinearColor::Red;
}

// Called when the game starts or when spawned
void ADamageCone::BeginPlay()
{
	Super::BeginPlay();
	
	// Привязываем функцию обработки пересечения
	ConeMesh->OnComponentBeginOverlap.AddDynamic(this, &ADamageCone::OnOverlapBegin);

	// Применяем красный цвет к конусу
	if (ConeMesh)
	{
		UMaterialInstanceDynamic* DynMaterial = ConeMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMaterial)
		{
			DynMaterial->SetVectorParameterValue(TEXT("BaseColor"), ConeColor);
		}
	}
}

// Called every frame
void ADamageCone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADamageCone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								 bool bFromSweep, const FHitResult& SweepResult)
{
	// Проверяем, что пересеклись с Mario
	AMario* Mario = Cast<AMario>(OtherActor);
	if (Mario)
	{
		// Вызываем функцию получения урона у Mario
		Mario->TriggerDamage();
	}
}

