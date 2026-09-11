#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "RespawnState.h"
#include "ChallengeGameMode.generated.h"

class APawn;
class AController;

UCLASS(Blueprintable)
class CHALLENGEGAME_API AChallengeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChallengeGameMode();

	/** Stores the first player spawn transform. It is ignored after initialisation. */
	void InitializeDefaultSpawnTransform(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "Death Respawn")
	bool RegisterCheckpoint(const FTransform& CheckpointTransform);

	UFUNCTION(BlueprintPure, Category = "Death Respawn")
	bool HasActiveCheckpoint() const;

	UFUNCTION(BlueprintPure, Category = "Death Respawn")
	FTransform GetActiveRespawnTransform() const;

	UFUNCTION(BlueprintPure, Category = "Death Respawn")
	bool IsRespawnInProgress() const;

	/** Starts one delayed respawn. Returns false for duplicate death requests. */
	bool RequestRespawn(AController* Controller, APawn* PawnToDestroy);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death Respawn", meta = (ClampMin = "0.0"))
	float RespawnDelay = 3.0f;

	/** Pawn class created at the selected respawn transform. Set this to BP_ChallengeCharacter in BP_ChallengeMode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death Respawn")
	TSubclassOf<APawn> RespawnPawnClass;

private:
	void CompleteRespawn(AController* Controller);

	FRespawnState RespawnState;
	FTimerHandle RespawnTimerHandle;
	bool bDefaultSpawnInitialised = false;
};
