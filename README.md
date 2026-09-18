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
