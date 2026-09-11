#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathRespawnComponent.generated.h"

UCLASS(ClassGroup = (DeathRespawn), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CHALLENGEGAME_API UDeathRespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeathRespawnComponent();

	virtual void BeginPlay() override;

	/** Safe to call from hazards, falling checks, and Blueprint overlap events. */
	UFUNCTION(BlueprintCallable, Category = "Death Respawn")
	bool RequestDeath();

	/** Records the latest checkpoint transform in the active game mode. */
	UFUNCTION(BlueprintCallable, Category = "Death Respawn")
	bool RegisterCheckpoint(const FTransform& CheckpointTransform);

	UFUNCTION(BlueprintPure, Category = "Death Respawn")
	bool IsRespawnInProgress() const;
};
