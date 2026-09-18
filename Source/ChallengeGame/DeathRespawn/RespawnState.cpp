#include "RespawnState.h"

FRespawnState::FRespawnState(const FTransform& InDefaultSpawnTransform)
	: DefaultSpawnTransform(IsUsableTransform(InDefaultSpawnTransform) ? InDefaultSpawnTransform : FTransform::Identity)
{
}

bool FRespawnState::TryBeginDeath()
{
	if (State != ERespawnState::Alive)
	{
		return false;
	}

	State = ERespawnState::Dying;
	return true;
}

bool FRespawnState::BeginRespawn()
{
	if (State != ERespawnState::Dying)
	{
		return false;
	}

	State = ERespawnState::Respawning;
	return true;
}

bool FRespawnState::CompleteRespawn()
{
	if (State != ERespawnState::Respawning)
	{
		return false;
	}

	State = ERespawnState::Alive;
	return true;
}

bool FRespawnState::CancelRespawn()
{
	if (State == ERespawnState::Alive)
	{
		return false;
	}

	State = ERespawnState::Alive;
	return true;
}

bool FRespawnState::SetDefaultSpawnTransform(const FTransform& InTransform)
{
	if (!IsUsableTransform(InTransform))
	{
		return false;
	}

	DefaultSpawnTransform = InTransform;
	return true;
}

bool FRespawnState::SetCheckpoint(const FTransform& InTransform)
{
	if (!IsUsableTransform(InTransform))
	{
		return false;
	}

	// 检查点只描述重生的位置和朝向；角色必须以自身的默认单位缩放生成。
	// 否则关卡物体的非单位缩放会被继承到 Pawn，造成角色变形、相机距离异常。
	CheckpointTransform = InTransform;
	CheckpointTransform.SetScale3D(FVector::OneVector);
	bHasCheckpoint = true;
	return true;
}

void FRespawnState::ClearCheckpoint()
{
	bHasCheckpoint = false;
	CheckpointTransform = FTransform::Identity;
}

bool FRespawnState::HasCheckpoint() const
{
	return bHasCheckpoint;
}

ERespawnState FRespawnState::GetState() const
{
	return State;
}

const FTransform& FRespawnState::GetRespawnTransform() const
{
	return bHasCheckpoint ? CheckpointTransform : DefaultSpawnTransform;
}

const FTransform& FRespawnState::GetDefaultSpawnTransform() const
{
	return DefaultSpawnTransform;
}

bool FRespawnState::IsUsableTransform(const FTransform& Transform)
{
	return !Transform.ContainsNaN();
}
