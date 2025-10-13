// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Mario.generated.h"

// Enum для состояний персонажа
UENUM(BlueprintType)
enum class EStateOfCharacter : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Jumping UMETA(DisplayName = "Jumping"),
	DoubleJumping UMETA(DisplayName = "DoubleJumping"),
	Falling UMETA(DisplayName = "Falling"),
	Dashing UMETA(DisplayName = "Dashing"),
	Dead UMETA(DisplayName = "Dead")
};

// Структура состояния персонажа
USTRUCT(BlueprintType)
struct FCharacterState
{
	GENERATED_BODY()

	// Количество жизней
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	int32 Lives;

	// Текущее состояние персонажа
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	EStateOfCharacter CurrentState;

	// Флаг для проверки, нажата ли клавиша движения D
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsDPressed;

	// Флаг для проверки, нажата ли клавиша движения A
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsAPressed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool SprintPressed;

	
	// Время последнего дэша
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float LastDashTime;

	// Флаг дэша
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsDashing;

	// Оставшееся время кулдауна дэша
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float DashCooldownRemaining;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bLastDirectionRight;
	// Конструктор по умолчанию
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
	{
	}
};

// Структура для параметров движения
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
	float JumpZVelocity = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DoubleJumpZVelocity = 500.0f;
};


UCLASS()
class MYMARIO_API AMario : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMario();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Вызывается при приземлении
	virtual void Landed(const FHitResult& Hit) override;

public:
	// ===== Функции обработки ввода (вызываются из PlayerController) =====
	
	void Move(const float Value);
	void StopMove(const float Value);
	void Jump() override;
	void StartSprint();
	void StopSprint();
	void StartDash();
	void TriggerDeath();

	// ===== Состояние персонажа =====
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FCharacterState CharacterState;

	// ===== Параметры движения =====
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FMovementParameters MovementParams;

private:
    FTimerHandle _timerHandle;
	// Функция для завершения дэша
	void EndDash();
};

