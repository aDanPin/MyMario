#pragma once

#include "CoreMinimal.h"
#include "DamageCone.h"
#include "UpDownConeDamage.generated.h"

UCLASS()
class MYMARIO_API AUpDownConeDamage : public ADamageCone
{
	GENERATED_BODY()
	
public:
	AUpDownConeDamage();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
	float WaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
	float FallTime;

private:
	FTimerHandle CycleTimerHandle;

	FTimerHandle FallTimerHandle;

	FVector InitialPosition;

	FVector TargetPosition;

	float MeshHeight;

	float FallProgress;

	bool bIsFalling;

	void StartRiseCycle();

	void RiseAndStartFall();

	void UpdateFalling(float DeltaTime);

	float GetMeshHeight();
};

