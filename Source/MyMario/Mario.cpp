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

	// Инициализация структуры состояния персонажа
	CharacterState = FCharacterState();

	// Инициализация вектора движения мыши
	MouseMovement = FVector2D::ZeroVector;

}

// Called when the game starts or when spawned
void AMario::BeginPlay()
{
	Super::BeginPlay();
	
	// Установка начальной скорости ходьбы
	GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = MovementParams.JumpZVelocity;
}

// Called every frame
void AMario::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// updateCharacterState(DeltaTime);
	
	// Обновляем переменные для анимации
	UpdateAnimationVariables();
	
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
			break;
		case EStateOfCharacter::Floating:
			// Применяем движение на основе ввода мыши
			if (MouseMovement.SizeSquared() > 0.0f)
			{
				FVector MovementDirection = FVector(MouseMovement.X, 0.0f, 0.0f);
				MovementDirection.Normalize();

				// перемещение по X и Y (например, по плоскости X-Y)
				AddMovementInput(GetActorRightVector(), MouseMovement.X  * MovementParams.FloatingSpeed * DeltaTime);
				AddMovementInput(GetActorUpVector(), MouseMovement.Y * MovementParams.FloatingSpeed * DeltaTime);

				if (MouseMovement.X > 0.0f)
				{
					CharacterState.bLastDirectionRight = true;
					GetMesh()->SetRelativeRotation(FRotator(0.0f, 00.0f, 0.0f));
				}
				else if (MouseMovement.X < 0.0f)
				{
					CharacterState.bLastDirectionRight = false;
					GetMesh()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
				}
				
				MouseMovement = FVector2D::ZeroVector;
			}
			break;
		case EStateOfCharacter::Damage:
			// Во время урона нельзя применять никакую силу
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
		case EStateOfCharacter::Damage:
			// Во время урона движение заблокировано
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

	if(CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
	{
		CharacterState.bLastDirectionRight = true;
	}
	else if(CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
	{
		CharacterState.bLastDirectionRight = false;
	}
	else if( Value == 1.0f)
	{
		CharacterState.bLastDirectionRight = true;
	}
	else if ( Value == -1.0f)
	{
		CharacterState.bLastDirectionRight = false;
	}

	auto stopMoveFunc = std::function<void(float)>([this](float MovementValue)
    {
		if (!CharacterState.SprintPressed)
		{
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
		}
	
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
		case EStateOfCharacter::Damage:
			// Во время урона прыжок заблокирован
			break;
		default:
			break;
	}
}

void AMario::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

	// Обрабатываем приземление для всех состояний в воздухе
	if (CharacterState.CurrentState == EStateOfCharacter::Jumping ||
	    CharacterState.CurrentState == EStateOfCharacter::DoubleJumping ||
	    CharacterState.CurrentState == EStateOfCharacter::Falling)
	{
		if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
		{
			CharacterState.CurrentState = EStateOfCharacter::Idle;
		}
		else if (CharacterState.SprintPressed)
		{
			CharacterState.CurrentState = EStateOfCharacter::Running;
		}
		else // if CharacterState.IsDPressed || CharacterState.IsAPressed
		{
			CharacterState.CurrentState = EStateOfCharacter::Walking;
		}
	}
}

void AMario::StartSprint()
{
	CharacterState.SprintPressed = true;
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
		break;
		case EStateOfCharacter::Walking:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
			CharacterState.CurrentState = EStateOfCharacter::Running;
			break;
		case EStateOfCharacter::Jumping:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
			break;
		case EStateOfCharacter::DoubleJumping:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
			break;
		case EStateOfCharacter::Damage:
			// Во время урона бег заблокирован
			break;
		default:
			break;
	}
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
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
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

void AMario::StartDash()
{
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
		case EStateOfCharacter::Walking:
		case EStateOfCharacter::Running:
		case EStateOfCharacter::Jumping:
		case EStateOfCharacter::DoubleJumping:
			CharacterState.CurrentState = EStateOfCharacter::Dashing;

			GetCharacterMovement()->GroundFriction = 0.f;

			if (CharacterState.bLastDirectionRight)
			{
				LaunchCharacter(FVector(0.0f, -MovementParams.DashSpeed, 0.0f) , true, true);
			}
			else
			{
				LaunchCharacter(FVector(0.0f, MovementParams.DashSpeed, 0.0f), true, true);
			}

			GetWorldTimerManager().SetTimer(
				_dashTimerHandle,
				this,
				&AMario::EndDash,
				MovementParams.DashDuration,
				false
			);
			break;
		case EStateOfCharacter::Damage:
			// Во время урона дэш заблокирован
			break;
		default:
			break;
	}	
}

void AMario::EndDash()
{
	GetCharacterMovement()->GroundFriction = 1.f;
	if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
	{
		CharacterState.CurrentState = EStateOfCharacter::Idle;
	}
	else if (CharacterState.SprintPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
		CharacterState.CurrentState = EStateOfCharacter::Running;
	}
	else // if CharacterState.IsDPressed || CharacterState.IsAPressed
	{
		CharacterState.CurrentState = EStateOfCharacter::Walking;
	}
}

void AMario::TriggerDamage()
{
	UMaterialInstanceDynamic* DynMat = nullptr;

	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
		case EStateOfCharacter::Walking:
		case EStateOfCharacter::Running:
		case EStateOfCharacter::Jumping:
		case EStateOfCharacter::DoubleJumping:
					// Проверяем, не находится ли уже персонаж в состоянии урона или неуязвимости
			if (CharacterState.CurrentState == EStateOfCharacter::Damage || 
				CharacterState.CurrentState == EStateOfCharacter::Dead)
			{
				return;
			}

			// Переходим в состояние урона
			CharacterState.CurrentState = EStateOfCharacter::Damage;

			// Останавливаем персонажа - убираем все силы
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->Velocity = FVector::ZeroVector;

			if (CharacterState.bLastDirectionRight)
			{
				LaunchCharacter(FVector(0.0f, MovementParams.DamageHorizontalVelocity, MovementParams.DamageVerticalVelocity) , true, true);
			}
			else
			{
				LaunchCharacter(FVector(0.0f, -MovementParams.DamageHorizontalVelocity, MovementParams.DamageVerticalVelocity), true, true);
			}
			
			
			// Сделать персонажа красным
			if (GetMesh())
			{
				int32 NumMaterials = GetMesh()->GetNumMaterials();
				for (int32 i = 0; i < NumMaterials; ++i)
				{
					DynMat = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
					if (DynMat)
					{
						DynMat->SetVectorParameterValue(TEXT("BodyColor"), FLinearColor::Red);
					}
				}
			}


			GetWorldTimerManager().SetTimer(
				_damageTimerHandle,
				this,
				&AMario::EndDamage,
				CharacterState.DamageImmunityDuration,
				false
			);


		default:
			break;
	}
}

void AMario::EndDamage()
{
	CharacterState.CurrentState = EStateOfCharacter::Idle;

	UMaterialInstanceDynamic* DynMat = nullptr;
	if (GetMesh())
	{
		int32 NumMaterials = GetMesh()->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			DynMat = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
			if (DynMat)
			{
				DynMat->SetVectorParameterValue(TEXT("BodyColor"), FLinearColor::White);
			}
		}
	}
}

void AMario::TriggerDeath()
{
}

void AMario::StartFloating()
{
	// Проверяем, доступен ли floating (не на кулдауне и не в состоянии урона/смерти)
	if (GetWorld()->GetTimeSeconds() - CharacterState.FloatingStartTime  < CharacterState.FloatingCooldownRemaining)
	{
		return;
	}

	FVector CurrentVelocity = GetVelocity();

	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
		case EStateOfCharacter::Walking:
		case EStateOfCharacter::Running:
		case EStateOfCharacter::Jumping:
		case EStateOfCharacter::DoubleJumping:
		case EStateOfCharacter::Falling:
			CharacterState.CurrentState = EStateOfCharacter::Floating;
			CharacterState.bIsFloating = true;
			CharacterState.FloatingStartTime = GetWorld()->GetTimeSeconds();

			GetCharacterMovement()->SetMovementMode(MOVE_Flying);

            MouseMovement = FVector2D::ZeroVector;

			// Запускаем таймер на завершение floating
			GetWorldTimerManager().SetTimer(
				_floatingTimerHandle,
				this,
				&AMario::EndFloating,
				MovementParams.FloatingDuration,
				false
			);
			break;
		case EStateOfCharacter::Damage:
		case EStateOfCharacter::Dead:
			break;
		default:
			break;
	}
}

void AMario::EndFloating()
{
	CharacterState.bIsFloating = false;
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// Запускаем кулдаун
	CharacterState.FloatingCooldownRemaining = MovementParams.FloatingCooldown;
	
	// Переходим в состояние падения, если персонаж в воздухе
	if (GetCharacterMovement()->IsFalling())
	{
		CharacterState.CurrentState = EStateOfCharacter::Falling;
	}
	else if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
	{
		CharacterState.CurrentState = EStateOfCharacter::Idle;
	}
	else if (CharacterState.SprintPressed)
	{
		CharacterState.CurrentState = EStateOfCharacter::Running;
	}
	else
	{
		CharacterState.CurrentState = EStateOfCharacter::Walking;
	}

	// Сбрасываем движение мыши
	MouseMovement = FVector2D::ZeroVector;
}

void AMario::HandleMouseMovement(const FInputActionValue& Value)
{
	// Получаем значение оси мыши (2D)
	MouseMovement += Value.Get<FVector2D>();
}

void AMario::UpdateAnimationVariables()
{
	if (!GetCharacterMovement())
	{
		return;
	}

	// Получаем текущую скорость персонажа
	FVector Velocity = GetVelocity();
	
	// Вычисляем горизонтальную скорость (2D)
	FVector HorizontalVelocity2D = FVector(Velocity.X, Velocity.Y, 0.0f);
	AnimationVars.Speed = HorizontalVelocity2D.Size();
	
	// Определяем направление движения
	if (CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
	{
		AnimationVars.Direction = 1.0f; // Вправо
	}
	else if (CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
	{
		AnimationVars.Direction = -1.0f; // Влево
	}
	else
	{
		AnimationVars.Direction = 0.0f; // Не движется
	}
	
	// Проверка, находится ли персонаж в воздухе
	AnimationVars.bIsInAir = GetCharacterMovement()->IsFalling();
	
	// Обновление состояний анимации на основе текущего состояния персонажа
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Idle:
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::Walking:
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::Running:
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::Jumping:
			AnimationVars.bIsFalling = Velocity.Z < 0.0f;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::DoubleJumping:
			AnimationVars.bIsFalling = Velocity.Z < 0.0f;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::Falling:
			AnimationVars.bIsFalling = true;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::Dashing:
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = true;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;

		case EStateOfCharacter::Floating:
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = true;
			break;
			
		case EStateOfCharacter::Damage:
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = true;
			AnimationVars.bIsFloating = false;
			break;
			
		case EStateOfCharacter::Dead:
			AnimationVars.bIsDead = true;
			AnimationVars.bIsFalling = false;
			AnimationVars.bIsDashing = false;
			AnimationVars.bIsDamaged = false;
			AnimationVars.bIsFloating = false;
			break;
			
		default:
			break;
	}
}

