# ChallengeGame 死亡与重生模块自动化测试说明

## 1. 被测对象

本项目的被测模块为第三人称跑酷游戏的“角色死亡与检查点重生模块”。C++ 代码将死亡状态、检查点记录和重生点选择逻辑从蓝图中抽离，以便进行自动化单元测试。

核心源码位置：

```text
Source/ChallengeGame/DeathRespawn/RespawnState.h/.cpp
Source/ChallengeGame/DeathRespawn/DeathRespawnComponent.h/.cpp
Source/ChallengeGame/DeathRespawn/ChallengeGameMode.h/.cpp
```

## 2. 自动化测试脚本

测试脚本位于：

```text
Source/ChallengeGame/Tests/RespawnStateTests.cpp
```

脚本使用 Unreal Engine Automation Test 框架，测试名称为：

```text
ChallengeGame.DeathRespawn.RespawnState
```

其中包含 30 条测试用例：`DR-01` 至 `DR-30`。用例覆盖等价类法、边界值法和场景法。

## 3. 首次运行前准备

1. 使用 Unreal Engine 5.7.4 打开 `ChallengeGame.uproject`。
2. 使用 Visual Studio 将解决方案编译为 `Development Editor | Win64`。
3. 在 `BP_ChallengeMode` 的“类默认值”中，将 `Default Pawn Class` 和 `Respawn Pawn Class` 都设置为 `BP_ChallengeCharacter`。
4. 确认 `BP_ChallengeCharacter` 已添加 `Death Respawn Component`。

## 4. 一键运行全部测试

双击项目根目录下的：

```text
RunAllDeathRespawnTests.bat
```

脚本会执行全部 30 条测试，并将 HTML/JSON 测试报告导出到：

```text
Saved/AutomationReports/DeathRespawn/
```

脚本默认检查以下两个程序：

```text
D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe
```

如果 Unreal Engine 的安装目录不同，使用文本编辑器打开 `RunAllDeathRespawnTests.bat`，仅修改 `ENGINE_BIN` 的路径即可。

## 5. 通过编辑器界面运行

1. 打开 UE 项目。
2. 选择“工具”→“测试自动化”。
3. 搜索 `ChallengeGame.DeathRespawn.RespawnState`。
4. 勾选该测试组并点击“开始测试”。

## 6. 当前测试基线与缺陷修复结果

DEF-001（检查点缩放继承）已修复。现在保存检查点时，C++ 保留检查点的位置和旋转，但将重生 Transform 的 `Scale3D` 统一为 `(1, 1, 1)`。因此，关卡中检查点 Actor 的非单位缩放不会再传递给新生成的角色，角色比例与相机距离可保持正常。

最新自动化测试结果：

```text
DR-01 至 DR-30：全部通过
Succeeded：30
Failed：0
Not Run：0
```

DEF-001 应在缺陷报告中更新为“已修复/已关闭”，并保留 DR-09 作为该缺陷的回归测试用例。



# ChallengeGame AI 辅助测试说明
## 1. 被测对象与 AI 融合方式

本次测试对象是 ChallengeGame 的死亡、检查点与重生状态规则，核心类为 `FRespawnState`。AI 辅助完成以下工作：

- 根据已有模块和测试清单生成 15 条测试用例；
- 覆盖等价类、边界值、状态迁移、场景组合和异常输入；
- 从 `IsUsableTransform` 的实现中定位 Transform 有效性校验不足；
- 生成 Unreal Engine Automation Test 脚本和一键运行脚本。

## 2. 文件说明

```text
Source/ChallengeGame/Tests/AIRespawnStateTests.cpp  AI 生成的 15 条自动化单元测试
RunAllAITests.bat                                   一键执行脚本
README_AI测试.md                                    本说明文件
```

测试组名称：

```text
ChallengeGame.AI.DeathRespawn.RespawnState
```

测试编号为 `AI-UT-001` 至 `AI-UT-015`，与 AI 测试用例清单逐一对应。

## 3. 新发现缺陷：AI-DEF-001

当前 `FRespawnState::IsUsableTransform` 仅检查：

```cpp
return !Transform.ContainsNaN();
```

该判断不能拒绝“没有 NaN、但旋转四元数未归一化”的 Transform。例如 `FQuat(0, 0, 0, 0)` 数值有限，却不能表示有效旋转。当前实现会接受它作为检查点；`AI-UT-002` 因此失败。

修复建议：

```cpp
return Transform.IsValid();
```

`FTransform::IsValid()` 同时校验有限数值和旋转四元数是否已归一化。

## 4. 首次运行前准备

1. 将 `AIRespawnStateTests.cpp` 放入项目的 `Source/ChallengeGame/Tests/` 目录。
2. 将 `RunAllAITests.bat` 和本 README 放入项目根目录。
3. 使用 Unreal Engine 5.7.4 打开 `ChallengeGame.uproject`。
4. 在 Visual Studio 中选择 `Development Editor | Win64` 并重新生成 `ChallengeGameEditor`。

## 5. 一键执行全部 AI 测试

双击项目根目录的：

```text
RunAllAITests.bat
```

HTML 和 JSON 报告会导出到：

```text
Saved/AutomationReports/AITesting/
```

脚本默认使用：

```text
D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
```

若引擎安装位置不同，只需修改脚本顶部的 `ENGINE_BIN`。

## 6. 当前预期结果与修复验证

未修复 AI-DEF-001 时，预期结果为：

```text
AI-UT-001、AI-UT-003 至 AI-UT-015：通过
AI-UT-002：失败
```

将 `IsUsableTransform` 修复为 `return Transform.IsValid();` 后，重新编译并再次双击脚本，预期结果为：

```text
AI-UT-001 至 AI-UT-015：全部通过
```

## 7. 编辑器内执行方式

1. 打开 UE 项目。
2. 选择“工具”→“测试自动化”。
3. 搜索 `ChallengeGame.AI.DeathRespawn.RespawnState`。
4. 勾选测试组后点击“开始测试”。
