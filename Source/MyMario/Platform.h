#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Platform.generated.h"

UCLASS()
class MYMARIO_API APlatform : public APawn
{
	GENERATED_BODY()

public:
	APlatform();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform Components")
	class UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform Components")
	class UBoxComponent* CollisionBox;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	bool bBottomTransparent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	bool bMovable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	FLinearColor PlatformColor;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	int32 MovementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float MovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float MovementRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings", meta = (EditCondition = "bMovable"))
	float PauseAtEndPoints;

private:
	FVector StartLocation;

	int32 CurrentDirection;

	float CurrentAngle;

	float PauseTimer;

	bool bIsPaused;

	void UpdateMovablePlatform(float DeltaTime);

	bool IsActorAbovePlatform(AActor* Actor);
};

