#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Mario.generated.h"

UENUM(BlueprintType)
enum class EStateOfCharacter : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Jumping UMETA(DisplayName = "Jumping"),
	DoubleJumping UMETA(DisplayName = "DoubleJumping"),
	Falling UMETA(DisplayName = "Falling"),
	FallAfterDoubleJump UMETA(DisplayName = "FallAfterDoubleJump"),
	Dashing UMETA(DisplayName = "Dashing"),
	Floating UMETA(DisplayName = "Floating"),
	Damage UMETA(DisplayName = "Damage"),
	Dead UMETA(DisplayName = "Dead")
};

USTRUCT(BlueprintType)
struct FCharacterState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	int32 Lives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	EStateOfCharacter CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsDPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsAPressed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool SprintPressed;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float LastDashTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsDashing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float DashCooldownRemaining;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bLastDirectionRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float DamageImmunityDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float DamageStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsFloating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float FloatingCooldownRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float FloatingStartTime;

	FCharacterState()
		: Lives(3)
		, CurrentState(EStateOfCharacter::Idle)
		, bIsDPressed(false)
		, bIsAPressed(false)
		, SprintPressed(false)
		, LastDashTime(0.0f)
		, bIsDashing(false)
		, DashCooldownRemaining(0.0f)
		, bLastDirectionRight(true)
		, DamageImmunityDuration(1.0f)
		, DamageStartTime(0.0f)
		, bIsFloating(false)
		, FloatingCooldownRemaining(0.0f)
		, FloatingStartTime(0.0f)
	{
	}
};

USTRUCT(BlueprintType)
struct FMovementParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 700.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DamageVerticalVelocity = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DamageHorizontalVelocity = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpZVelocity = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DoubleJumpZVelocity = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FloatingDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FloatingCooldown = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FloatingSpeed = 500.0f;

};

USTRUCT(BlueprintType)
struct FAnimationVariables
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsInAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDashing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDamaged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsFloating = false;

	FAnimationVariables()
		: Speed(0.0f)
		, Direction(0.0f)
		, bIsInAir(false)
		, bIsFalling(false)
		, bIsDashing(false)
		, bIsDamaged(false)
		, bIsDead(false)
		, bIsFloating(false)
	{
	}
};


UCLASS()
class MYMARIO_API AMario : public ACharacter
{
	GENERATED_BODY()

public:
	AMario();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

public:
	
	void Move(const float Value);
	void StopMove(const float Value);
	void Jump() override;
	void StartSprint();
	void StopSprint();
	void StartDash();
	void StartFloating();
	void HandleMouseMovement(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Character")
	void TriggerDamage();
	
	void TriggerDeath();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FCharacterState CharacterState;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FMovementParameters MovementParams;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FAnimationVariables AnimationVars;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystemComponent* FloatingParticle;

	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE float GetSpeed() const { return AnimationVars.Speed; }
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE float GetDirection() const { return AnimationVars.Direction; }
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsInAir() const { return AnimationVars.bIsInAir; }
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsFalling() const { return AnimationVars.bIsFalling; }
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsDashing() const { return AnimationVars.bIsDashing; }
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsDamaged() const { return AnimationVars.bIsDamaged; }
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsDead() const { return AnimationVars.bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsFloating() const { return AnimationVars.bIsFloating; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE bool GetIsDoubleJumping() const { return CharacterState.CurrentState == EStateOfCharacter::DoubleJumping; }

	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE float GetWalkSpeed() const { return MovementParams.WalkSpeed; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE float GetSprintSpeed() const { return MovementParams.SprintSpeed; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE float GetDashSpeed() const { return MovementParams.DashSpeed; }

private:
    FTimerHandle _dashTimerHandle;
	FTimerHandle _damageTimerHandle;
	FTimerHandle _floatingTimerHandle;


	FVector2D MouseMovement;
	
	void EndDash();
	
	void EndDamage();

	void EndFloating();

	void UpdateFloatingCooldown();
	
	void UpdateAnimationVariables();
};

