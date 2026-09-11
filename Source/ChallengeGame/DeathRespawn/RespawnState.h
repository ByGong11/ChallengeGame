// Deterministic, world-independent rules for the death and respawn system.
#pragma once

#include "CoreMinimal.h"

enum class ERespawnState : uint8
{
	Alive,
	Dying,
	Respawning
};

class FRespawnState
{
public:
	explicit FRespawnState(const FTransform& InDefaultSpawnTransform = FTransform::Identity);

	bool TryBeginDeath();
	bool BeginRespawn();
	bool CompleteRespawn();
	bool CancelRespawn();

	bool SetDefaultSpawnTransform(const FTransform& InTransform);
	bool SetCheckpoint(const FTransform& InTransform);
	void ClearCheckpoint();

	bool HasCheckpoint() const;
	ERespawnState GetState() const;
	const FTransform& GetRespawnTransform() const;
	const FTransform& GetDefaultSpawnTransform() const;

	static bool IsUsableTransform(const FTransform& Transform);

private:
	ERespawnState State = ERespawnState::Alive;
	FTransform DefaultSpawnTransform;
	FTransform CheckpointTransform;
	bool bHasCheckpoint = false;
};
