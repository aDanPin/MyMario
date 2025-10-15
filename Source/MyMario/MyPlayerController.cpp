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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveRightAction)
		{
			EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleMoveRightStarted);
			EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &AMyPlayerController::HandleMoveRightCompleted);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: MoveRightAction привязан"));
		}

		if (MoveLeftAction)
		{
			EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleMoveLeftStarted);
			EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Completed, this, &AMyPlayerController::HandleMoveLeftCompleted);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: MoveLeftAction привязан"));
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleJumpStarted);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: JumpAction привязан"));
		}

		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleSprintStarted);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyPlayerController::HandleSprintCompleted);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: SprintAction привязан с Started и Completed"));
		}

		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleDash);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: DashAction привязан"));
		}

		if (DeathAction)
		{
			EnhancedInputComponent->BindAction(DeathAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleDeath);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: DeathAction привязан"));
		}

		if (FloatingAction)
		{
			EnhancedInputComponent->BindAction(FloatingAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleFloating);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: FloatingAction привязан"));
		}

		if (MouseMovementAction)
		{
			EnhancedInputComponent->BindAction(MouseMovementAction, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleMouseMovement);
			UE_LOG(LogTemp, Log, TEXT("MyPlayerController: MouseMovementAction привязан"));
		}
	}
}

void AMyPlayerController::HandleMoveRightStarted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Move Right (D) Started"));

	if (ControlledMario)
	{
		ControlledMario->Move(1.0f);
	}
}

void AMyPlayerController::HandleMoveRightCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Move Right (D) Completed"));

	if (ControlledMario)
	{
		ControlledMario->StopMove(1.0f);
	}
}

void AMyPlayerController::HandleMoveLeftStarted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Move Left (A) Started"));

	if (ControlledMario)
	{
		ControlledMario->Move(-1.0f);
	}
}

void AMyPlayerController::HandleMoveLeftCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Move Left (A) Completed"));

	if (ControlledMario)
	{
		ControlledMario->StopMove(-1.0f);
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
		ControlledMario->StartDash();
	}
}

void AMyPlayerController::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("MyPlayerController: Death Triggered"));

	if (ControlledMario)
	{
		ControlledMario->TriggerDamage();
	}
}

void AMyPlayerController::HandleFloating()
{
	UE_LOG(LogTemp, Log, TEXT("MyPlayerController: Floating (F) Triggered"));

	if (ControlledMario)
	{
		ControlledMario->StartFloating();
	}
}

void AMyPlayerController::HandleMouseMovement(const FInputActionValue& Value)
{
	if (ControlledMario)
	{
		ControlledMario->HandleMouseMovement(Value);
	}
}

