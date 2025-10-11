// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Mario.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMyPlayerController::AMyPlayerController()
{
	ControlledMario = nullptr;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Добавление Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Input Mapping Context добавлен"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: DefaultMappingContext не установлен!"));
		}
	}

	// Получение ссылки на управляемого персонажа Mario
	ControlledMario = Cast<AMario>(GetPawn());
	if (ControlledMario)
	{
		UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Контроль над персонажем Mario установлен"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Не удалось получить ссылку на Mario!"));
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Привязка Enhanced Input Actions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Движение (A/D)
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleMove);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: MoveAction привязан"));
		}

		// Прыжок (Space)
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleJumpStarted);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: JumpAction привязан"));
		}

		// Спринт (Shift)
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleSprintStarted);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyPlayerController::HandleSprintCompleted);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: SprintAction привязан"));
		}

		// Дэш (Ctrl)
		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleDash);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: DashAction привязан"));
		}

		// Смерть (M)
		if (DeathAction)
		{
			EnhancedInputComponent->BindAction(DeathAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleDeath);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: DeathAction привязан"));
		}
	}
}

// ===== Функции обработки ввода с логированием =====

void AMyPlayerController::HandleMove(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();
	
	// Логируем только ненулевые значения, чтобы не засорять лог
	if (FMath::Abs(MovementValue) > 0.01f)
	{
		UE_LOG(LogTemp, Verbose, TEXT("MyPlayerController: Move Input = %.2f"), MovementValue);
	}

	if (ControlledMario)
	{
		ControlledMario->Move(MovementValue);
	}
}

void AMyPlayerController::HandleJumpStarted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Jump Started"));

	if (ControlledMario)
	{
		ControlledMario->Jump();
	}
}

void AMyPlayerController::HandleJumpCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Jump Completed"));

	if (ControlledMario)
	{
		ControlledMario->StopJumping();
	}
}

void AMyPlayerController::HandleSprintStarted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Sprint Started"));

	if (ControlledMario)
	{
		ControlledMario->StartSprint();
	}
}

void AMyPlayerController::HandleSprintCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Sprint Completed"));

	if (ControlledMario)
	{
		ControlledMario->StopSprint();
	}
}

void AMyPlayerController::HandleDash()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Dash Performed"));

	if (ControlledMario)
	{
		ControlledMario->PerformDash();
	}
}

void AMyPlayerController::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Death Triggered"));

	if (ControlledMario)
	{
		ControlledMario->TriggerDeath();
	}
}

