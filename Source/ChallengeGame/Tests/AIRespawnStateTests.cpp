#if WITH_DEV_AUTOMATION_TESTS

#include "../DeathRespawn/RespawnState.h"
#include "Misc/AutomationTest.h"

#include <limits>

IMPLEMENT_COMPLEX_AUTOMATION_TEST(
	FAIRespawnStateAutomationTest,
	"ChallengeGame.AI.DeathRespawn.RespawnState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FAIRespawnStateAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	static const TCHAR* Cases[] =
	{
		TEXT("AI-UT-001 Default spawn preserves location and rotation"),
		TEXT("AI-UT-002 Reject non-normalized rotation quaternion"),
		TEXT("AI-UT-003 NaN default spawn falls back to identity"),
		TEXT("AI-UT-004 Invalid default does not overwrite valid default"),
		TEXT("AI-UT-005 Zero-scale checkpoint uses unit scale"),
		TEXT("AI-UT-006 Latest checkpoint replaces location and rotation"),
		TEXT("AI-UT-007 NaN checkpoint does not overwrite valid checkpoint"),
		TEXT("AI-UT-008 Large finite coordinates are accepted"),
		TEXT("AI-UT-009 First death enters Dying"),
		TEXT("AI-UT-010 Duplicate death request is rejected"),
		TEXT("AI-UT-011 Alive cannot begin respawn"),
		TEXT("AI-UT-012 Complete respawn returns to Alive"),
		TEXT("AI-UT-013 Cancel while dying returns to Alive"),
		TEXT("AI-UT-014 Checkpoint may update while dying"),
		TEXT("AI-UT-015 Clear checkpoint while dying restores default spawn")
	};

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Cases); ++Index)
	{
		OutBeautifiedNames.Add(Cases[Index]);
		OutTestCommands.Add(FString::FromInt(Index + 1));
	}
}

bool FAIRespawnStateAutomationTest::RunTest(const FString& Parameters)
{
	const int32 CaseNumber = FCString::Atoi(*Parameters);
	const FTransform DefaultTransform(FRotator(0.0, 45.0, 0.0), FVector(100.0, 200.0, 300.0), FVector::OneVector);
	const FTransform FirstCheckpoint(FRotator(0.0, 90.0, 0.0), FVector(1000.0, 2000.0, 3000.0), FVector::OneVector);
	const FTransform LatestCheckpoint(FRotator(10.0, 180.0, 20.0), FVector(-300.0, 400.0, 500.0), FVector(1.5, 2.0, 0.5));
	const FTransform ExpectedLatestCheckpoint(LatestCheckpoint.GetRotation(), LatestCheckpoint.GetLocation(), FVector::OneVector);
	FRespawnState State(DefaultTransform);

	switch (CaseNumber)
	{
	case 1:
		return TestTrue(TEXT("Default transform is retained"), State.GetDefaultSpawnTransform().Equals(DefaultTransform));
	case 2:
	{
		const FTransform InvalidRotation(FQuat(0.0, 0.0, 0.0, 0.0), FVector(100.0, 200.0, 300.0), FVector::OneVector);
		return TestFalse(TEXT("Reject non-normalized quaternion"), State.SetCheckpoint(InvalidRotation));
	}
	case 3:
	{
		FTransform Invalid = FTransform::Identity;
		Invalid.SetLocation(FVector(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0));
		FRespawnState InvalidDefault(Invalid);
		return TestTrue(TEXT("Identity fallback"), InvalidDefault.GetDefaultSpawnTransform().Equals(FTransform::Identity));
	}
	case 4:
	{
		FTransform Invalid = FTransform::Identity;
		Invalid.SetLocation(FVector(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0));
		return TestTrue(TEXT("Set valid default"), State.SetDefaultSpawnTransform(FirstCheckpoint)) &&
			TestFalse(TEXT("Reject invalid default"), State.SetDefaultSpawnTransform(Invalid)) &&
			TestTrue(TEXT("Valid default retained"), State.GetDefaultSpawnTransform().Equals(FirstCheckpoint));
	}
	case 5:
	{
		const FTransform ZeroScaleCheckpoint(FRotator(0.0, 120.0, 0.0), FVector(800.0, 900.0, 1000.0), FVector::ZeroVector);
		return TestTrue(TEXT("Set zero-scale checkpoint"), State.SetCheckpoint(ZeroScaleCheckpoint)) &&
			TestTrue(TEXT("Unit scale is used"), State.GetRespawnTransform().GetScale3D().Equals(FVector::OneVector));
	}
	case 6:
		return TestTrue(TEXT("Set first checkpoint"), State.SetCheckpoint(FirstCheckpoint)) &&
			TestTrue(TEXT("Set latest checkpoint"), State.SetCheckpoint(LatestCheckpoint)) &&
			TestTrue(TEXT("Latest location and rotation are selected"), State.GetRespawnTransform().Equals(ExpectedLatestCheckpoint));
	case 7:
	{
		FTransform Invalid = FTransform::Identity;
		Invalid.SetLocation(FVector(0.0, 0.0, std::numeric_limits<double>::quiet_NaN()));
		return TestTrue(TEXT("Set valid checkpoint"), State.SetCheckpoint(FirstCheckpoint)) &&
			TestFalse(TEXT("Reject NaN checkpoint"), State.SetCheckpoint(Invalid)) &&
			TestTrue(TEXT("Original checkpoint retained"), State.GetRespawnTransform().Equals(FirstCheckpoint));
	}
	case 8:
	{
		const FTransform LargeCoordinates(FRotator(0.0, 30.0, 0.0), FVector(1000000.0, -1000000.0, 500000.0), FVector::OneVector);
		return TestTrue(TEXT("Accept large finite checkpoint"), State.SetCheckpoint(LargeCoordinates)) &&
			TestTrue(TEXT("Large finite location retained"), State.GetRespawnTransform().GetLocation().Equals(LargeCoordinates.GetLocation()));
	}
	case 9:
		return TestTrue(TEXT("First death accepted"), State.TryBeginDeath()) &&
			TestTrue(TEXT("State is Dying"), State.GetState() == ERespawnState::Dying);
	case 10:
		return TestTrue(TEXT("First death accepted"), State.TryBeginDeath()) &&
			TestFalse(TEXT("Duplicate death rejected"), State.TryBeginDeath());
	case 11:
		return TestFalse(TEXT("Cannot begin respawn from Alive"), State.BeginRespawn()) &&
			TestTrue(TEXT("State remains Alive"), State.GetState() == ERespawnState::Alive);
	case 12:
		return TestTrue(TEXT("Death begins"), State.TryBeginDeath()) &&
			TestTrue(TEXT("Respawn begins"), State.BeginRespawn()) &&
			TestTrue(TEXT("Respawn completes"), State.CompleteRespawn()) &&
			TestTrue(TEXT("State returns to Alive"), State.GetState() == ERespawnState::Alive);
	case 13:
		return TestTrue(TEXT("Death begins"), State.TryBeginDeath()) &&
			TestTrue(TEXT("Cancel succeeds"), State.CancelRespawn()) &&
			TestTrue(TEXT("State returns to Alive"), State.GetState() == ERespawnState::Alive);
	case 14:
		return TestTrue(TEXT("Initial checkpoint is set"), State.SetCheckpoint(FirstCheckpoint)) &&
			TestTrue(TEXT("Death begins"), State.TryBeginDeath()) &&
			TestTrue(TEXT("Checkpoint updates while dying"), State.SetCheckpoint(LatestCheckpoint)) &&
			TestTrue(TEXT("Latest checkpoint is selected"), State.GetRespawnTransform().Equals(ExpectedLatestCheckpoint));
	case 15:
	{
		const bool bCheckpointSet = TestTrue(TEXT("Checkpoint is set"), State.SetCheckpoint(FirstCheckpoint));
		const bool bDeathStarted = TestTrue(TEXT("Death begins"), State.TryBeginDeath());
		const bool bCheckpointExists = TestTrue(TEXT("Checkpoint exists before clear"), State.HasCheckpoint());
		State.ClearCheckpoint();
		return bCheckpointSet && bDeathStarted && bCheckpointExists &&
			TestFalse(TEXT("Checkpoint is cleared"), State.HasCheckpoint()) &&
			TestTrue(TEXT("Default spawn is restored"), State.GetRespawnTransform().Equals(DefaultTransform));
	}
	default:
		AddError(TEXT("Unknown AI test case."));
		return false;
	}
}

#endif
