#if WITH_DEV_AUTOMATION_TESTS

#include "../DeathRespawn/RespawnState.h"
#include "Misc/AutomationTest.h"

#include <limits>

IMPLEMENT_COMPLEX_AUTOMATION_TEST(
	FRespawnStateAutomationTest,
	"ChallengeGame.DeathRespawn.RespawnState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FRespawnStateAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	static const TCHAR* Cases[] =
	{
		TEXT("DR-01 Initial state is Alive"), TEXT("DR-02 Uses supplied default spawn"),
		TEXT("DR-03 Invalid default falls back to identity"), TEXT("DR-04 No checkpoint at startup"),
		TEXT("DR-05 Valid checkpoint is accepted"), TEXT("DR-06 Checkpoint is selected for respawn"),
		TEXT("DR-07 Latest checkpoint replaces earlier one"), TEXT("DR-08 Checkpoint rotation is retained"),
		TEXT("DR-09 Checkpoint scale is reset to one"), TEXT("DR-10 NaN checkpoint is rejected"),
		TEXT("DR-11 Invalid checkpoint does not overwrite valid checkpoint"), TEXT("DR-12 Clear checkpoint restores default spawn"),
		TEXT("DR-13 First death request is accepted"), TEXT("DR-14 Duplicate death request is rejected"),
		TEXT("DR-15 Accepted death enters Dying state"), TEXT("DR-16 Dying can begin respawn"),
		TEXT("DR-17 Begin respawn enters Respawning state"), TEXT("DR-18 Duplicate begin respawn is rejected"),
		TEXT("DR-19 Respawning can complete"), TEXT("DR-20 Completion returns to Alive"),
		TEXT("DR-21 Completion from Alive is rejected"), TEXT("DR-22 Begin respawn from Alive is rejected"),
		TEXT("DR-23 Cancel while dying returns to Alive"), TEXT("DR-24 Cancel while respawning returns to Alive"),
		TEXT("DR-25 Cancel while alive is rejected"), TEXT("DR-26 Death is allowed after cancellation"),
		TEXT("DR-27 Death is allowed after completion"), TEXT("DR-28 Checkpoint may update during death delay"),
		TEXT("DR-29 Default transform may be replaced"), TEXT("DR-30 Finite negative coordinates are valid")
	};

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Cases); ++Index)
	{
		OutBeautifiedNames.Add(Cases[Index]);
		OutTestCommands.Add(FString::FromInt(Index + 1));
	}
}

bool FRespawnStateAutomationTest::RunTest(const FString& Parameters)
{
	const int32 CaseNumber = FCString::Atoi(*Parameters);
	const FTransform DefaultTransform(FRotator(0.0, 45.0, 0.0), FVector(100.0, 200.0, 300.0), FVector(1.0, 1.0, 1.0));
	const FTransform FirstCheckpoint(FRotator(0.0, 90.0, 0.0), FVector(1000.0, 2000.0, 3000.0), FVector(1.0, 1.0, 1.0));
	const FTransform LatestCheckpoint(FRotator(10.0, 180.0, 20.0), FVector(-300.0, 400.0, 500.0), FVector(1.5, 2.0, 0.5));
	const FTransform ExpectedLatestCheckpoint(LatestCheckpoint.GetRotation(), LatestCheckpoint.GetLocation(), FVector::OneVector);
	FRespawnState State(DefaultTransform);

	switch (CaseNumber)
	{
	case 1: return TestTrue(TEXT("State is Alive"), State.GetState() == ERespawnState::Alive);
	case 2: return TestTrue(TEXT("Default transform"), State.GetRespawnTransform().Equals(DefaultTransform));
	case 3:
	{
		FTransform Invalid = FTransform::Identity;
		Invalid.SetLocation(FVector(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0));
		FRespawnState InvalidDefault(Invalid);
		return TestTrue(TEXT("Identity fallback"), InvalidDefault.GetDefaultSpawnTransform().Equals(FTransform::Identity));
	}
	case 4: return TestFalse(TEXT("Has checkpoint"), State.HasCheckpoint());
	case 5: return TestTrue(TEXT("Set checkpoint"), State.SetCheckpoint(FirstCheckpoint));
	case 6: State.SetCheckpoint(FirstCheckpoint); return TestTrue(TEXT("Checkpoint selected"), State.GetRespawnTransform().Equals(FirstCheckpoint));
	case 7: State.SetCheckpoint(FirstCheckpoint); State.SetCheckpoint(LatestCheckpoint); return TestTrue(TEXT("Latest selected with unit scale"), State.GetRespawnTransform().Equals(ExpectedLatestCheckpoint));
	case 8: State.SetCheckpoint(LatestCheckpoint); return TestTrue(TEXT("Rotation"), State.GetRespawnTransform().GetRotation().Equals(LatestCheckpoint.GetRotation()));
	case 9: State.SetCheckpoint(LatestCheckpoint); return TestTrue(TEXT("Unit scale"), State.GetRespawnTransform().GetScale3D().Equals(FVector::OneVector));
	case 10:
	{
		FTransform Invalid = FTransform::Identity;
		Invalid.SetLocation(FVector(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0));
		return TestFalse(TEXT("Reject NaN"), State.SetCheckpoint(Invalid));
	}
	case 11:
	{
		State.SetCheckpoint(FirstCheckpoint);
		FTransform Invalid = FTransform::Identity;
		Invalid.SetScale3D(FVector(std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
		State.SetCheckpoint(Invalid);
		return TestTrue(TEXT("Original retained"), State.GetRespawnTransform().Equals(FirstCheckpoint));
	}
	case 12: State.SetCheckpoint(FirstCheckpoint); State.ClearCheckpoint(); return TestTrue(TEXT("Default restored"), State.GetRespawnTransform().Equals(DefaultTransform));
	case 13: return TestTrue(TEXT("First death"), State.TryBeginDeath());
	case 14: State.TryBeginDeath(); return TestFalse(TEXT("Duplicate death"), State.TryBeginDeath());
	case 15: State.TryBeginDeath(); return TestTrue(TEXT("State is Dying"), State.GetState() == ERespawnState::Dying);
	case 16: State.TryBeginDeath(); return TestTrue(TEXT("Begin respawn"), State.BeginRespawn());
	case 17: State.TryBeginDeath(); State.BeginRespawn(); return TestTrue(TEXT("State is Respawning"), State.GetState() == ERespawnState::Respawning);
	case 18: State.TryBeginDeath(); State.BeginRespawn(); return TestFalse(TEXT("Duplicate begin"), State.BeginRespawn());
	case 19: State.TryBeginDeath(); State.BeginRespawn(); return TestTrue(TEXT("Complete respawn"), State.CompleteRespawn());
	case 20: State.TryBeginDeath(); State.BeginRespawn(); State.CompleteRespawn(); return TestTrue(TEXT("State is Alive"), State.GetState() == ERespawnState::Alive);
	case 21: return TestFalse(TEXT("Complete from alive"), State.CompleteRespawn());
	case 22: return TestFalse(TEXT("Begin from alive"), State.BeginRespawn());
	case 23: State.TryBeginDeath(); State.CancelRespawn(); return TestTrue(TEXT("State is Alive after cancel"), State.GetState() == ERespawnState::Alive);
	case 24: State.TryBeginDeath(); State.BeginRespawn(); State.CancelRespawn(); return TestTrue(TEXT("State is Alive after respawn cancel"), State.GetState() == ERespawnState::Alive);
	case 25: return TestFalse(TEXT("Cancel from alive"), State.CancelRespawn());
	case 26: State.TryBeginDeath(); State.CancelRespawn(); return TestTrue(TEXT("Death after cancel"), State.TryBeginDeath());
	case 27: State.TryBeginDeath(); State.BeginRespawn(); State.CompleteRespawn(); return TestTrue(TEXT("Death after completion"), State.TryBeginDeath());
	case 28: State.TryBeginDeath(); State.SetCheckpoint(LatestCheckpoint); return TestTrue(TEXT("Updated checkpoint with unit scale"), State.GetRespawnTransform().Equals(ExpectedLatestCheckpoint));
	case 29: return TestTrue(TEXT("Replace default"), State.SetDefaultSpawnTransform(LatestCheckpoint)) && TestTrue(TEXT("New default selected"), State.GetRespawnTransform().Equals(LatestCheckpoint));
	case 30: return TestTrue(TEXT("Negative finite transform"), FRespawnState::IsUsableTransform(LatestCheckpoint));
	default:
		AddError(TEXT("Unknown test case."));
		return false;
	}
}

#endif
