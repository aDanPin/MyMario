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
	FallAfterDoubleJump UMETA(DisplayName = "FallAfterDoubleJump"),
	Dashing UMETA(DisplayName = "Dashing"),
	Floating UMETA(DisplayName = "Floating"),
	Damage UMETA(DisplayName = "Damage"),
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

	// Длительность неуязвимости при получении урона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float DamageImmunityDuration;

	// Время начала состояния урона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float DamageStartTime;

	// Флаг состояния floating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsFloating;

	// Оставшееся время кулдауна floating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float FloatingCooldownRemaining;

	// Время начала floating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	float FloatingStartTime;

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
		, DamageImmunityDuration(1.0f)
		, DamageStartTime(0.0f)
		, bIsFloating(false)
		, FloatingCooldownRemaining(0.0f)
		, FloatingStartTime(0.0f)
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

// Структура для переменных анимации
USTRUCT(BlueprintType)
struct FAnimationVariables
{
	GENERATED_BODY()

	// Скорость персонажа (для blend spaces)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed = 0.0f;

	// Направление движения (-1 влево, 0 стоп, 1 вправо)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction = 0.0f;

	// Находится ли персонаж в воздухе
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsInAir = false;

	// Находится ли персонаж в состоянии падения
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsFalling = false;

	// Выполняет ли персонаж дэш
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDashing = false;

	// Получает ли персонаж урон
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDamaged = false;

	// Мертв ли персонаж
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDead = false;

	// Находится ли персонаж в состоянии floating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsFloating = false;

	// Конструктор по умолчанию
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
	void StartFloating();
	void HandleMouseMovement(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Character")
	void TriggerDamage();
	
	void TriggerDeath();

	// ===== Состояние персонажа =====
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FCharacterState CharacterState;

	// ===== Параметры движения =====
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FMovementParameters MovementParams;

	// ===== Переменные анимации =====
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FAnimationVariables AnimationVars;

	// ===== Компонент партикла для Floating =====
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystemComponent* FloatingParticle;

	// ===== Геттеры для переменных анимации =====
	
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

	// ===== Геттеры для параметров движения =====
	
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


	// Вектор движения мыши для floating
	FVector2D MouseMovement;
	
	// Функция для завершения дэша
	void EndDash();
	
	// Функция для завершения состояния урона
	void EndDamage();

	// Функция для завершения floating
	void EndFloating();

	// Функция для уменьшения кулдауна floating
	void UpdateFloatingCooldown();
	
	// Функция обновления переменных для анимации
	void UpdateAnimationVariables();
};

