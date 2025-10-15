#include "WalkingDamageCone.h"
#include "Mario.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

AWalkingDamageCone::AWalkingDamageCone()
{
 	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	if (SphereCollider)
	{
		SphereCollider->InitSphereRadius(100.0f);
		
		SphereCollider->SetupAttachment(RootComponent);
		
		SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		SphereCollider->SetGenerateOverlapEvents(true);
	}

	WalkSpeed = 200.0f;
	ObstacleDetectionDistance = 100.0f;
	bIsActive = true;
	JumpFromAboveThreshold = 50.0f;
	CurrentDirection = 1;
}

void AWalkingDamageCone::BeginPlay()
{
	Super::BeginPlay();
	


	if (SphereCollider)
	{
		SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AWalkingDamageCone::OnWalkingOverlapBegin);
		
		UE_LOG(LogTemp, Log, TEXT("WalkingDamageCone: SphereCollider initialized with radius %f"), 
			SphereCollider->GetScaledSphereRadius());
	}
}

void AWalkingDamageCone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive)
	{
		return;
	}

	if (CheckForObstacle())
	{
		ChangeDirection();
	}
	
	FVector CurrentLocation = GetActorLocation();
	FVector Movement = FVector(0.0f, CurrentDirection * WalkSpeed * DeltaTime, 0.0f);
	
	SetActorLocation(CurrentLocation + Movement);
}

bool AWalkingDamageCone::CheckForObstacle()
{
	if (!ConeMesh)
	{
		return false;
	}

	FVector StartLocation = GetActorLocation();
	
	FVector EndLocation = StartLocation + FVector(0.0f, CurrentDirection * ObstacleDetectionDistance, 0.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_WorldDynamic,
		QueryParams
	);

	// Если попали в Марио - игнорируем
	if (bHit && HitResult.GetActor() && HitResult.GetActor()->IsA(AMario::StaticClass()))
	{
		return false;
	}

	#if WITH_EDITOR
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, 
				  bHit ? FColor::Red : FColor::Green, 
				  false, 0.1f, 0, 2.0f);
	#endif

	return bHit;
}

void AWalkingDamageCone::ChangeDirection()
{
	CurrentDirection *= -1;

	if (ConeMesh)
	{
		FRotator NewRotation = ConeMesh->GetRelativeRotation();
		if (CurrentDirection == 1)
		{
			NewRotation.Yaw = 0.0f;
		}
		else
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
	AMario* Mario = Cast<AMario>(OtherActor);
	if (Mario && bIsActive)
	{
		FVector MarioLocation = Mario->GetActorLocation();
		FVector ConeLocation = GetActorLocation();
		
		float HeightDifference = MarioLocation.Z - ConeLocation.Z;
		
		FVector MarioVelocity = Mario->GetVelocity();
		bool bIsFalling = MarioVelocity.Z < 0.0f;

		if (HeightDifference > JumpFromAboveThreshold && bIsFalling)
		{
			DeactivateCone();
			
			Mario->LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, true);
		}
		else
		{
			Mario->TriggerDamage();
		}
	}	
}

void AWalkingDamageCone::DeactivateCone()
{
	bIsActive = false;
	
	if (ConeMesh)
	{
		ConeMesh->SetVisibility(false);
		ConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (SphereCollider)
	{
		SphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}

