#include "Mario.h"

#include <functional>

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"


AMario::AMario()
{
 	PrimaryActorTick.bCanEverTick = true;

	CharacterState = FCharacterState();

	MouseMovement = FVector2D::ZeroVector;

}

void AMario::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = MovementParams.WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = MovementParams.JumpZVelocity;

	if (!FloatingParticle)
	{
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			TArray<USceneComponent*> ChildComponents;
			Capsule->GetChildrenComponents(true, ChildComponents);
			for (USceneComponent* Child : ChildComponents)
			{
				if (UParticleSystemComponent* PSC = Cast<UParticleSystemComponent>(Child))
				{
					FloatingParticle = PSC;
					FloatingParticle->Deactivate();

					break;
				}
			}
		}
	}
}

void AMario::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateAnimationVariables();
	
	FVector InputVector = FVector::ZeroVector;
	switch (CharacterState.CurrentState)
	{
		case EStateOfCharacter::Walking:
		case EStateOfCharacter::Running:
		case EStateOfCharacter::Falling:
		case EStateOfCharacter::FallAfterDoubleJump:
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
		case EStateOfCharacter::Jumping:
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

			// Проверяем вертикальную скорость персонажа.
			if (GetVelocity().Z < 0.0f)
			{
				CharacterState.CurrentState = EStateOfCharacter::Falling;
			}
			
			break;
		case EStateOfCharacter::DoubleJumping:
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

			// Проверяем вертикальную скорость персонажа.
			if (GetVelocity().Z < 0.0f)
			{
				CharacterState.CurrentState = EStateOfCharacter::FallAfterDoubleJump;
			}
			
			break;
		case EStateOfCharacter::Dashing:
			break;
		case EStateOfCharacter::Floating:
			if (MouseMovement.SizeSquared() > 0.0f)
			{
				FVector MovementDirection = FVector(MouseMovement.X, 0.0f, 0.0f);
				MovementDirection.Normalize();

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
			break;
		default:
			break;
	}
}

void AMario::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

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

	if(CharacterState.bIsDPressed && CharacterState.bIsAPressed && Value > 0.0f)
	{
		CharacterState.bLastDirectionRight = true;
	}
	else if(CharacterState.bIsAPressed && CharacterState.bIsDPressed && Value < 0.0f)
	{
		CharacterState.bLastDirectionRight = false;
	}

    auto moveFunc = std::function<void(float)>([this](float MovementValue)
    {
		if (Controller && MovementValue != 0.0f)
		{
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
		case EStateOfCharacter::Falling:
			moveFunc(Value);
			break;
		case EStateOfCharacter::FallAfterDoubleJump:
			moveFunc(Value);
			break;
		case EStateOfCharacter::Damage:
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
		case EStateOfCharacter::Falling:
			stopMoveFunc(Value);
			break;
		case EStateOfCharacter::FallAfterDoubleJump:
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
		case EStateOfCharacter::Falling:
			jumpfunc();
			CharacterState.CurrentState = EStateOfCharacter::DoubleJumping;
			break;
		case EStateOfCharacter::FallAfterDoubleJump:
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

	if (CharacterState.CurrentState == EStateOfCharacter::Jumping ||
	    CharacterState.CurrentState == EStateOfCharacter::DoubleJumping ||
	    CharacterState.CurrentState == EStateOfCharacter::Falling ||
	    CharacterState.CurrentState == EStateOfCharacter::FallAfterDoubleJump)
	{
		if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
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
		case EStateOfCharacter::FallAfterDoubleJump:
			GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
			break;
		case EStateOfCharacter::Damage:
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
		case EStateOfCharacter::FallAfterDoubleJump:
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
		case EStateOfCharacter::Falling:
		case EStateOfCharacter::FallAfterDoubleJump:
			CharacterState.CurrentState = EStateOfCharacter::Dashing;

			GetCharacterMovement()->GroundFriction = 0.f;

			if (CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
			{
				GetMesh()->SetRelativeRotation(FRotator(0.0f, 00.0f, 0.0f));
				LaunchCharacter(FVector(0.0f, -MovementParams.DashSpeed, 0.0f) , true, true);
			}
			else if (CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
			{
				GetMesh()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
				LaunchCharacter(FVector(0.0f, MovementParams.DashSpeed, 0.0f), true, true);
			}
			else if (CharacterState.bLastDirectionRight)
			{
				GetMesh()->SetRelativeRotation(FRotator(0.0f, 00.0f, 0.0f));
				LaunchCharacter(FVector(0.0f, -MovementParams.DashSpeed, 0.0f) , true, true);
			}
			else
			{
				GetMesh()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
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
	else
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
		case EStateOfCharacter::FallAfterDoubleJump:
		case EStateOfCharacter::Falling:
			if (CharacterState.CurrentState == EStateOfCharacter::Damage || 
				CharacterState.CurrentState == EStateOfCharacter::Dead)
			{
				return;
			}

			CharacterState.CurrentState = EStateOfCharacter::Damage;

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

		case EStateOfCharacter::Damage:
			break;

		default:
			break;
	}
}

void AMario::EndDamage()
{
	if(GetMovementComponent()->IsFalling() && GetVelocity().Z <= 0.0f)
	{
		CharacterState.CurrentState = EStateOfCharacter::Falling;
	}
	else if(GetMovementComponent()->IsFalling() && GetVelocity().Z > 0.0f)
	{
		CharacterState.CurrentState = EStateOfCharacter::Jumping;
	}
	else if (!CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
	{
		CharacterState.CurrentState = EStateOfCharacter::Idle;
	}
	else if (CharacterState.SprintPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = MovementParams.SprintSpeed;
		CharacterState.CurrentState = EStateOfCharacter::Running;
	}
	else
	{
		CharacterState.CurrentState = EStateOfCharacter::Walking;
	}



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
		case EStateOfCharacter::FallAfterDoubleJump:
			CharacterState.CurrentState = EStateOfCharacter::Floating;
			CharacterState.bIsFloating = true;
			CharacterState.FloatingStartTime = GetWorld()->GetTimeSeconds();

			GetCharacterMovement()->SetMovementMode(MOVE_Flying);

            MouseMovement = FVector2D::ZeroVector;

			
			if (FloatingParticle)
			{
				FloatingParticle->Activate(true);
			}
			if (GetMesh())
			{
				GetMesh()->SetVisibility(false);
			}

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

	if (FloatingParticle)
	{
		FloatingParticle->Deactivate();
	}
	if (GetMesh())
	{
		GetMesh()->SetVisibility(true);
	}

	CharacterState.FloatingCooldownRemaining = MovementParams.FloatingCooldown;
	
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

	MouseMovement = FVector2D::ZeroVector;
}

void AMario::HandleMouseMovement(const FInputActionValue& Value)
{
	MouseMovement += Value.Get<FVector2D>();
}

void AMario::UpdateAnimationVariables()
{
	if (!GetCharacterMovement())
	{
		return;
	}

	FVector Velocity = GetVelocity();
	
	FVector HorizontalVelocity2D = FVector(Velocity.X, Velocity.Y, 0.0f);
	AnimationVars.Speed = HorizontalVelocity2D.Size();
	
	if (CharacterState.bIsDPressed && !CharacterState.bIsAPressed)
	{
		AnimationVars.Direction = 1.0f;
	}
	else if (CharacterState.bIsAPressed && !CharacterState.bIsDPressed)
	{
		AnimationVars.Direction = -1.0f;
	}
	else
	{
		AnimationVars.Direction = 0.0f;
	}
	
	AnimationVars.bIsInAir = GetCharacterMovement()->IsFalling();
	
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

		case EStateOfCharacter::FallAfterDoubleJump:
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

