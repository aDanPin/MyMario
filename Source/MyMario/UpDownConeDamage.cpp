#include "UpDownConeDamage.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

AUpDownConeDamage::AUpDownConeDamage()
{
	PrimaryActorTick.bCanEverTick = true;

	WaitTime = 3.0f;
	FallTime = 1.5f;

	MeshHeight = 0.0f;
	FallProgress = 0.0f;
	bIsFalling = false;
}

void AUpDownConeDamage::BeginPlay()
{
	Super::BeginPlay();
	
	InitialPosition = GetActorLocation();

	MeshHeight = GetMeshHeight();

	TargetPosition = InitialPosition + FVector(0.0f, 0.0f, MeshHeight);

	UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Initialized at %s, MeshHeight: %f"), 
		*InitialPosition.ToString(), MeshHeight);

	StartRiseCycle();
}

void AUpDownConeDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFalling)
	{
		UpdateFalling(DeltaTime);
	}
}

void AUpDownConeDamage::StartRiseCycle()
{
	GetWorldTimerManager().SetTimer(CycleTimerHandle, this, &AUpDownConeDamage::RiseAndStartFall, 
									 WaitTime, false);
	
	UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Waiting %f seconds before rise"), WaitTime);
}

void AUpDownConeDamage::RiseAndStartFall()
{
	SetActorLocation(TargetPosition);
	
	UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Instantly risen to %s"), *TargetPosition.ToString());

	bIsFalling = true;
	FallProgress = 0.0f;
}

void AUpDownConeDamage::UpdateFalling(float DeltaTime)
{
	FallProgress += DeltaTime / FallTime;
	FallProgress = FMath::Clamp(FallProgress, 0.0f, 1.0f);

	FVector NewLocation = FMath::Lerp(TargetPosition, InitialPosition, FallProgress);
	SetActorLocation(NewLocation);

	if (FallProgress >= 1.0f)
	{
		bIsFalling = false;
		SetActorLocation(InitialPosition);
		
		UE_LOG(LogTemp, Log, TEXT("UpDownConeDamage: Returned to initial position"));

		StartRiseCycle();
	}
}

float AUpDownConeDamage::GetMeshHeight()
{
	if (ConeMesh && ConeMesh->GetStaticMesh())
	{
		FBoxSphereBounds Bounds = ConeMesh->GetStaticMesh()->GetBounds();
		
		FVector Scale = ConeMesh->GetComponentScale();
		float Height = Bounds.BoxExtent.Z * 2.0f * Scale.Z;
		
		return Height;
	}

	return 100.0f;
}

