// Fill out your copyright notice in the Description page of Project Settings.

#include "Mario.h"

#include <functional>

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"


// Sets default values
AMario::AMario()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Инициализация параметров движения
	MovementParams.WalkSpeed = 2000.0f;
	MovementParams.SprintSpeed = 4000.0f;
	MovementParams.DashSpeed = 15000.0f;
	MovementParams.DashDuration = 0.2f;
	MovementParams.DashCooldown = 1.0f;
	MovementParams.JumpZVelocity = 6000.0f;
	MovementParams.DoubleJumpZVelocity = 5000.0f;



	// Инициализация структуры состояния персонажа
	CharacterState = FCharacterState();
}

// Called when the game starts or when spawned
void AMario::BeginPlay()
{
	Super::BeginPlay();
	
	// Установка начальной скорости ходьбы
	GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
}

// Called every frame
void AMario::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Обновление таймера кулдауна дэша
	if (CharacterState.DashCooldownRemaining > 0.0f)
	{
		CharacterState.DashCooldownRemaining -= DeltaTime;
	}

	// Обновление состояния персонажа
	UpdateCharacterState();
}

// Called to bind functionality to input
void AMario::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Теперь ввод обрабатывается через MyPlayerController
	// Эта функция оставлена для совместимости
}

void AMario::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

// ===== Функции обработки ввода =====

void AMario::Move(const float Value)
{
    auto moveFunc = std::function<void(float)>([this](float MovementValue)
    {
		if (Controller && MovementValue != 0.0f)
		{
			// Перед движением обязательно обновляем WalkSpeed (например, если изменился режим бега/ходьбы)
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
			GetCharacterMovement()->AddInputVector(FVector::LeftVector * MovementValue * MovementParams.WalkSpeed);
			// Поворот персонажа в направлении движения через скелетную компоненту
			if (MovementValue > 0.0f)
			{
				bIsDPressed = true;
				if (GetMesh())
				{
					GetMesh()->SetRelativeRotation(FRotator(0.0f, 00.0f, 0.0f));
				}
			}
			else if (MovementValue < 0.0f)
			{
				bIsAPressed = true;
				if (GetMesh())
				{
					GetMesh()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
				}
			}
		}
    });

	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
			moveFunc(Value);
		break;
		case EStateOfCharacter::Jumping:
			moveFunc(Value);
			break;
		case EStateOfCharacter::DoubleJumping:
			moveFunc(Value);
			break;
		default:
			break;
	}

	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
            break;
		case EStateOfCharacter::Walking:
			break;
		case EStateOfCharacter::Running:
			break;
		case EStateOfCharacter::Jumping:
			break;
		case EStateOfCharacter::DoubleJumping:
			break;
		case EStateOfCharacter::Dead:
			break;
		case EStateOfCharacter::Dashing:
			break;
		default:
			break;
	}

}

void AMario::Jump()
{
	auto jumpfunc = std::function<void()>([this]()
	{
		GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
		GetCharacterMovement()->DoJump(false);
		CharacterState.CurrentState = EStateOfCharacter::Jumping;
	});
	
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
			jumpfunc();
            break;
		case EStateOfCharacter::Walking:
			jumpfunc();
			break;
		case EStateOfCharacter::Running:
			jumpfunc();
			break;
		case EStateOfCharacter::Jumping:
			jumpfunc();
			CharacterState.CurrentState = EStateOfCharacter::DoubleJumping;
			break;
		default:
			break;
	}
}

void AMario::StopJumping()
{
	Super::StopJumping();
}

void AMario::StartSprint()
{
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Walking:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
			CharacterState.CurrentState = EStateOfCharacter::Running;
			break;
		default:
			break;
	}}

void AMario::StopSprint()
{
	CharacterState.CurrentState = EStateOfCharacter::Idle;
	GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
}

void AMario::PerformDash()
{
	// Проверка на кулдаун и смерть
	if (CharacterState.DashCooldownRemaining > 0.0f || CharacterState.CurrentState == EStateOfCharacter::Dead)
	{
		return;
	}

	// Определение направления дэша
	FVector ForwardVector = GetActorForwardVector();
	
	// Если персонаж движется, используем текущее направление
	FVector Velocity = GetVelocity();

	// Начало дэша
	CharacterState.bIsDashing = true;
	CharacterState.CurrentState = EStateOfCharacter::Dashing;
	CharacterState.DashCooldownRemaining = MovementParams.DashCooldown;

	// Отключение гравитации во время дэша
	GetCharacterMovement()->GravityScale = 0.0f;


	// Применение импульса дэша
	LaunchCharacter(ForwardVector * MovementParams.DashSpeed, true, true);

}

void AMario::EndDash()
{
	CharacterState.bIsDashing = false;
	
	// Восстановление гравитации
	GetCharacterMovement()->GravityScale = 1.0f;

	// Сброс скорости, чтобы персонаж не летел после дэша
	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.X = 0.0f;
	CurrentVelocity.Y *= 0.5f; // Оставляем немного инерции
	GetCharacterMovement()->Velocity = CurrentVelocity;
}

void AMario::TriggerDeath()
{
	CharacterState.CurrentState = EStateOfCharacter::Dead;
	CharacterState.Lives--;

	// Отключение ввода
	DisableInput(Cast<APlayerController>(Controller));

	// Отключение коллизии
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// Здесь можно добавить анимацию смерти или другие эффекты
	UE_LOG(LogTemp, Warning, TEXT("Mario died! Lives remaining: %d"), CharacterState.Lives);

	// Пример: перезапуск через 2 секунды (можно настроить)
	FTimerHandle RespawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, [this]()
	{
		if (CharacterState.Lives > 0)
		{
			// Возрождение персонажа
			CharacterState.CurrentState = EStateOfCharacter::Idle;
			EnableInput(Cast<APlayerController>(Controller));
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		else
		{
			// Game Over
			UE_LOG(LogTemp, Error, TEXT("Game Over!"));
		}
	}, 2.0f, false);
}

void AMario::UpdateCharacterState()
{
	// Не обновляем состояние, если персонаж мёртв или в дэше
	if (CharacterState.CurrentState == EStateOfCharacter::Dead || CharacterState.bIsDashing)
	{
		return;
	}

	// Определение текущего состояния на основе движения
	FVector Velocity = GetVelocity();
	float HorizontalSpeed = Velocity.Size2D();

	// Проверка, находится ли персонаж в воздухе
	if (GetCharacterMovement()->IsFalling())
	{
	}
	else
	{
		// На земле
		if (HorizontalSpeed > 1.0f)
		{
			CharacterState.CurrentState = EStateOfCharacter::Walking;
		}
		else
		{
			CharacterState.CurrentState = EStateOfCharacter::Idle;
		}
	}
}

