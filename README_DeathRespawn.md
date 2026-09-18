# Death & Respawn C++ module

This module moves the game rules of the original Blueprint death/checkpoint/respawn flow into C++. Blueprint assets remain responsible for level placement, collision geometry, character visuals, and effects.

## Added source files

| File | Responsibility |
| --- | --- |
| `Source/ChallengeGame/DeathRespawn/RespawnState.*` | World-independent state machine and checkpoint selection rules. |
| `Source/ChallengeGame/DeathRespawn/DeathRespawnComponent.*` | Blueprint-callable bridge attached to the player character. |
| `Source/ChallengeGame/DeathRespawn/ChallengeGameMode.*` | Delayed respawn, pawn recreation, and controller possession. |
| `Source/ChallengeGame/Tests/RespawnStateTests.cpp` | 30 automated test cases for the rule layer. |

The existing `RespawnComponent` actor is not used by this implementation. It is deliberately left unchanged so existing content is not broken; remove it only after the new flow has been verified in Unreal Editor.

## Required Blueprint changes

Perform the following after compiling the project once. The existing Blueprint assets do not need to be recreated.

### 1. `BP_ChallengeMode`

1. Open **Class Settings** and change its parent class from `GameModeBase` to `ChallengeGameMode`.
2. In **Class Defaults**, set both **Default Pawn Class** and **Respawn Pawn Class** to `BP_ChallengeCharacter`.
3. Delete the old `Rebirth` custom event and the `RebirthLocation` variable. They are replaced by the C++ game mode.

### 2. `BP_ChallengeCharacter`

1. Add the component **Death Respawn Component** in the Components panel.
2. Delete the old `Dead` custom event graph (physics, delay, Destroy Actor, and `Reborn` call).
3. Replace it with the component node **Request Death**.
4. Keep the falling check, but change its condition to `Actor Location Z < 470` and call **Request Death**. The component rejects repeated requests while a respawn is pending.

### 3. `BP_CheckPoint`

1. Keep the existing Box overlap and the cast to `BP_ChallengeCharacter`.
2. Delete `Get Game Mode`, the cast to `BP_ChallengeMode`, and `Set RebirthLocation`.
3. From the cast character, get **Death Respawn Component** and call **Register Checkpoint**.
4. Connect this checkpoint actor's `GetActorTransform` to the `Checkpoint Transform` input. The C++ state layer retains the location and rotation, then normalizes `Scale3D` to `(1, 1, 1)` before respawn. This prevents the checkpoint actor's scale from deforming the newly spawned player pawn.

### 4. `BP_003`

1. Keep the existing overlap, character cast, and `Death` Boolean branch.
2. In the `Death = true` branch, replace the direct `Dead` call with the cast character's **Death Respawn Component -> Request Death** node.
3. Keep the `Death = false` launch branch unchanged.
4. In the rotation graph, multiply the rotation rate by `Delta Seconds` (for example, use 180 degrees/second rather than a fixed 3 degrees per frame).

## Automated test execution

1. Build the project in the **Development Editor** configuration.
2. Start Unreal Editor and open **Tools -> Test Automation** (or Session Frontend -> Automation).
3. Search for `ChallengeGame.DeathRespawn.RespawnState`.
4. Run all 30 child cases and export the results for the test report.

The tests exercise the pure `FRespawnState` rules, so they do not need a map, a player, or Blueprint assets. This is intentional: unit-test failures can be located in the rule layer without level or rendering side effects.

## Current verified baseline

After the DEF-001 scale-inheritance fix, the current automated result is:

```text
DR-01 to DR-30: passed
Succeeded: 30
Failed: 0
Not Run: 0
```

`DR-09` is retained as the regression test that verifies a checkpoint with non-unit scale still respawns the player with unit scale.

## Test design coverage

- **Equivalence classes:** valid/invalid checkpoint transforms; alive/dying/respawning states; checkpoint present/absent.
- **Boundary and invalid values:** NaN transform values; initial/default transform; repeated state requests.
- **Scenario tests:** first death, repeated death, complete respawn, cancelled respawn, checkpoint update during the death delay.

## Behavioural note

`AChallengeGameMode` keeps checkpoint data alive while the old player pawn is destroyed. The component's `BeginPlay` records the first player pawn transform as the default spawn point. The most recently registered checkpoint then takes precedence until the level changes.
