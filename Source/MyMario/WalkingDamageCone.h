#pragma once

#include "CoreMinimal.h"
#include "DamageCone.h"
#include "WalkingDamageCone.generated.h"

UCLASS()
class MYMARIO_API AWalkingDamageCone : public ADamageCone
{
	GENERATED_BODY()
	
public:	
	AWalkingDamageCone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Walking Damage Cone Components")
	class USphereComponent* SphereCollider;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	float ObstacleDetectionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking Damage Cone Settings")
	float JumpFromAboveThreshold;

private:
	int32 CurrentDirection;

	bool CheckForObstacle();

	void ChangeDirection();

	UFUNCTION()
	void OnWalkingOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							   bool bFromSweep, const FHitResult& SweepResult);

	void DeactivateCone();
};

