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
	MovementParams.WalkSpeed = 500.0f;
	MovementParams.SprintSpeed = 1500.0f;
	MovementParams.DashSpeed = 3000.0f;
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
	// updateCharacterState(DeltaTime);
	
	FVector InputVector = FVector::ZeroVector;
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Walking:
		case EStateOfCharacter::Running:
		case EStateOfCharacter::Jumping:
		case EStateOfCharacter::DoubleJumping:
			// Перед движением обязательно обновляем WalkSpeed (например, если изменился режим бега/ходьбы)
			if (CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
			{
				InputVector = FVector::LeftVector;
			}
			else if (CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
			{
				InputVector = FVector::RightVector;
			}
			else
			{
				InputVector = FVector::ZeroVector;
			}

			if(CharacterState.SprintPressed)
			{
				GetCharacterMovement()->AddInputVector(InputVector * MovementParams.SprintSpeed);				}
			else
			{
				GetCharacterMovement()->AddInputVector(InputVector * MovementParams.WalkSpeed);
			}
			break;
		case EStateOfCharacter::Dashing:
			if (CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
			{
				GetCharacterMovement()->AddInputVector(FVector::LeftVector * MovementParams.DashSpeed);
			}
			else if (CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
			{
				GetCharacterMovement()->AddInputVector(FVector::RightVector * MovementParams.DashSpeed);
			}
			break;
		default:
			break;
	}
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
	if (Value > 0.0f)
	{
		CharacterState.bIsDPressed = true;
	}
	else if (Value< 0.0f)
	{
		CharacterState.bIsAPressed = true;
	}

    auto moveFunc = std::function<void(float)>([this](float MovementValue)
    {
		if (Controller && MovementValue != 0.0f)
		{
			// Поворот персонажа в направлении движения через скелетную компоненту
			if (MovementValue > 0.0f)
			{
				CharacterState.bIsDPressed = true;
				if (GetMesh() && !CharacterState.bIsAPressed)
				{
					GetMesh()->SetRelativeRotation(FRotator(0.0f, 00.0f, 0.0f));
				}
			}
			else if (MovementValue < 0.0f)
			{
				CharacterState.bIsAPressed = true;
				if (GetMesh() && !CharacterState.bIsDPressed)
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
			CharacterState.CurrentState = EStateOfCharacter::Walking;
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

void AMario::StopMove(const float Value)
{
	if (Value > 0.0f)
	{
		CharacterState.bIsDPressed = false;
	}
	else if (Value < 0.0f)
	{
		CharacterState.bIsAPressed = false;
	}

	auto stopMoveFunc = std::function<void(float)>([this](float MovementValue)
    {
		GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
	
		if (CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
		{
			GetMesh()->SetRelativeRotation(FRotator(0.0f, 00.0f, 0.0f));
		}
		if (CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
		{
			GetMesh()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
    });

	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Walking:
			stopMoveFunc(Value);

			if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
			{
				CharacterState.CurrentState = EStateOfCharacter::Idle;
			}
			break;
		case EStateOfCharacter::Running:
			stopMoveFunc(Value);

			if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
			{
				CharacterState.CurrentState = EStateOfCharacter::Idle;
			}
			break;
		case EStateOfCharacter::Jumping:
			stopMoveFunc(Value);
			break;
		case EStateOfCharacter::DoubleJumping:
			stopMoveFunc(Value);
			break;
		case EStateOfCharacter::Dashing:
			stopMoveFunc(Value);
			break;
		default:
			break;
	}

}

void AMario::Jump()
{
 	auto jumpfunc = std::function<void()>([this]()
	{
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
		case EStateOfCharacter::Idle:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
		break;
		case EStateOfCharacter::Walking:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
			CharacterState.CurrentState = EStateOfCharacter::Running;
			break;
		default:
			break;
	}
	CharacterState.SprintPressed = true;
}

void AMario::StopSprint()
{
	CharacterState.SprintPressed = false;

	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
            break;
		case EStateOfCharacter::Walking:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
			break;
		case EStateOfCharacter::Running:
			if(CharacterState.bIsDPressed || CharacterState.bIsAPressed)
			{
				CharacterState.CurrentState = EStateOfCharacter::Walking;
			}
			else
			{
				CharacterState.CurrentState = EStateOfCharacter::Idle;
			}
			break;
		case EStateOfCharacter::Jumping:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
		break;
		case EStateOfCharacter::DoubleJumping:
	     	GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
		    break;
		case EStateOfCharacter::Dashing:
			break;
		default:
			break;
	}
}

void AMario::PerformDash()
{
}

void AMario::EndDash()
{
}

void AMario::TriggerDeath()
{
}

