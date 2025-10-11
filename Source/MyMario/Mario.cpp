// Fill out your copyright notice in the Description page of Project Settings.

#include "Mario.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"

// Sets default values
AMario::AMario()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Настройка компонента движения
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Инициализация параметров движения
	MovementParams.WalkSpeed = 400.0f;
	MovementParams.SprintSpeed = 700.0f;
	MovementParams.DashSpeed = 1500.0f;
	MovementParams.DashDuration = 0.2f;
	MovementParams.DashCooldown = 1.0f;
	MovementParams.JumpZVelocity = 600.0f;
	MovementParams.DoubleJumpZVelocity = 500.0f;

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

	// Восстановление доступных прыжков при приземлении
	CharacterState.AvailableJumps = CharacterState.MaxJumps;
}

// ===== Функции обработки ввода =====

void AMario::Move(const FInputActionValue& Value)
{
	// Если персонаж мёртв или в дэше, не позволяем движение
	if (CharacterState.CurrentState == EStateOfCharacter::Dead || CharacterState.bIsDashing)
	{
		return;
	}

	// Получение значения ввода (для 2D движения)
	float MovementValue = Value.Get<float>();

	if (Controller && MovementValue != 0.0f)
	{
		// Определение направления движения (вправо/влево)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Получение вектора направления (вправо)
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Добавление движения
		AddMovementInput(Direction, MovementValue);

		// Поворот персонажа в направлении движения
		if (MovementValue > 0.0f)
		{
			SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
		}
		else if (MovementValue < 0.0f)
		{
			SetActorRotation(FRotator(0.0f, -90.0f, 0.0f));
		}
	}
}

void AMario::Jump()
{
	// Проверка на смерть
	if (CharacterState.CurrentState == EStateOfCharacter::Dead)
	{
		return;
	}

	// Проверка доступности прыжков
	if (CharacterState.AvailableJumps > 0)
	{
		// Первый прыжок (с земли)
		if (CharacterState.AvailableJumps == CharacterState.MaxJumps)
		{
			Super::Jump();
			GetCharacterMovement()->JumpZVelocity = MovementParams.JumpZVelocity;
			CharacterState.CurrentState = EStateOfCharacter::Jumping;
		}
		// Второй прыжок (двойной прыжок)
		else if (CharacterState.AvailableJumps == 1)
		{
			LaunchCharacter(FVector(0, 0, MovementParams.DoubleJumpZVelocity), false, true);
			CharacterState.CurrentState = EStateOfCharacter::DoubleJumping;
		}

		CharacterState.AvailableJumps--;
		CharacterState.LastJumpTime = GetWorld()->GetTimeSeconds();
	}
}

void AMario::StopJumping()
{
	Super::StopJumping();
}

void AMario::StartSprint()
{
	if (CharacterState.CurrentState != EStateOfCharacter::Dead && !CharacterState.bIsDashing)
	{
		CharacterState.CurrentState = EStateOfCharacter::Running;
		GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
	}
}

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
			CharacterState.AvailableJumps = CharacterState.MaxJumps;
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
		if (CharacterState.AvailableJumps == CharacterState.MaxJumps - 1)
		{
			CharacterState.CurrentState = EStateOfCharacter::Jumping;
		}
		else if (CharacterState.AvailableJumps < CharacterState.MaxJumps - 1)
		{
			CharacterState.CurrentState = EStateOfCharacter::DoubleJumping;
		}
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

