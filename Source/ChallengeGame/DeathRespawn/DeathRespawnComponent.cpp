#include "DeathRespawnComponent.h"

#include "ChallengeGameMode.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UDeathRespawnComponent::UDeathRespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDeathRespawnComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AChallengeGameMode* GameMode = GetWorld()->GetAuthGameMode<AChallengeGameMode>())
	{
		GameMode->InitializeDefaultSpawnTransform(GetOwner()->GetActorTransform());
	}
}

bool UDeathRespawnComponent::RequestDeath()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AChallengeGameMode* GameMode = GetWorld()->GetAuthGameMode<AChallengeGameMode>();
	return OwnerPawn && GameMode && GameMode->RequestRespawn(OwnerPawn->GetController(), OwnerPawn);
}

bool UDeathRespawnComponent::RegisterCheckpoint(const FTransform& CheckpointTransform)
{
	if (AChallengeGameMode* GameMode = GetWorld()->GetAuthGameMode<AChallengeGameMode>())
	{
		return GameMode->RegisterCheckpoint(CheckpointTransform);
	}

	return false;
}

bool UDeathRespawnComponent::IsRespawnInProgress() const
{
	if (const AChallengeGameMode* GameMode = GetWorld()->GetAuthGameMode<AChallengeGameMode>())
	{
		return GameMode->IsRespawnInProgress();
	}

	return false;
}
