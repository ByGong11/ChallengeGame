#include "ChallengeGameMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"

AChallengeGameMode::AChallengeGameMode()
	: RespawnState(FTransform::Identity)
{
}

void AChallengeGameMode::InitializeDefaultSpawnTransform(const FTransform& SpawnTransform)
{
	if (!bDefaultSpawnInitialised && RespawnState.SetDefaultSpawnTransform(SpawnTransform))
	{
		bDefaultSpawnInitialised = true;
	}
}

bool AChallengeGameMode::RegisterCheckpoint(const FTransform& CheckpointTransform)
{
	return RespawnState.SetCheckpoint(CheckpointTransform);
}

bool AChallengeGameMode::HasActiveCheckpoint() const
{
	return RespawnState.HasCheckpoint();
}

FTransform AChallengeGameMode::GetActiveRespawnTransform() const
{
	return RespawnState.GetRespawnTransform();
}

bool AChallengeGameMode::IsRespawnInProgress() const
{
	return RespawnState.GetState() != ERespawnState::Alive;
}

bool AChallengeGameMode::RequestRespawn(AController* Controller, APawn* PawnToDestroy)
{
	if (!Controller || !PawnToDestroy || !RespawnState.TryBeginDeath())
	{
		return false;
	}

	if (ACharacter* Character = Cast<ACharacter>(PawnToDestroy))
	{
		Character->GetCharacterMovement()->DisableMovement();
	}
	PawnToDestroy->SetActorEnableCollision(false);
	PawnToDestroy->DisableInput(Cast<APlayerController>(Controller));

	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		FTimerDelegate::CreateUObject(this, &AChallengeGameMode::CompleteRespawn, Controller),
		RespawnDelay,
		false);

	PawnToDestroy->Destroy();
	return true;
}

void AChallengeGameMode::CompleteRespawn(AController* Controller)
{
	if (!Controller || !RespawnState.BeginRespawn())
	{
		RespawnState.CancelRespawn();
		return;
	}

	UClass* PawnClass = RespawnPawnClass.Get();
	if (!PawnClass)
	{
		PawnClass = GetDefaultPawnClassForController(Controller);
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Controller;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APawn* NewPawn = PawnClass ? GetWorld()->SpawnActor<APawn>(PawnClass, RespawnState.GetRespawnTransform(), SpawnParameters) : nullptr;
	if (NewPawn)
	{
		Controller->Possess(NewPawn);
		RespawnState.CompleteRespawn();
	}
	else
	{
		RespawnState.CancelRespawn();
	}
}
