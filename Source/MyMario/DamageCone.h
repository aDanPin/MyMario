#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageCone.generated.h"

UCLASS()
class MYMARIO_API ADamageCone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageCone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage Cone Components")
	class UStaticMeshComponent* ConeMesh;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Cone Settings")
	FLinearColor ConeColor;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
};

