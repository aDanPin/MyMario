#include "DamageCone.h"
#include "Mario.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

ADamageCone::ADamageCone()
{
 	PrimaryActorTick.bCanEverTick = true;

	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
	RootComponent = ConeMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone"));
	if (ConeMeshAsset.Succeeded())
	{
		ConeMesh->SetStaticMesh(ConeMeshAsset.Object);
	}

	ConeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ConeMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ConeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ConeMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ConeMesh->SetGenerateOverlapEvents(true);
	ConeMesh->SetNotifyRigidBodyCollision(true);

	ConeColor = FLinearColor::Red;
}

void ADamageCone::BeginPlay()
{
	Super::BeginPlay();
	
	ConeMesh->OnComponentBeginOverlap.AddDynamic(this, &ADamageCone::OnOverlapBegin);

	if (ConeMesh)
	{
		UMaterialInstanceDynamic* DynMaterial = ConeMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMaterial)
		{
			DynMaterial->SetVectorParameterValue(TEXT("BaseColor"), ConeColor);
		}
	}
}

void ADamageCone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADamageCone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								 bool bFromSweep, const FHitResult& SweepResult)
{
	AMario* Mario = Cast<AMario>(OtherActor);
	if (Mario)
	{
		Mario->TriggerDamage();
	}
}

