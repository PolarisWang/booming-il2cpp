---
task_id: linker-driver-b1-build-command
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: 20260523-linker-driver-industrialization
source_task_id: b1-build-command
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
auto_execution_decision: continue
---

# b1-build-command: CLI build 子命令实现

## 设计摘要

实现 `chaos-il2cpp build` 子命令，将 `cmake --build` 包装为统一调用接口。

- `BuildService.cs` — 封装 cmake --build 进程调用，处理 cmake 未安装异常
- `RunBuild` 从 stub 升级：参数解析 → BuildService.RunBuild() → 结果输出
- 保持原有 CLI 签名：`chaos-il2cpp build <dir> [--target <id>]`

## 完成证据

- **文件创建**：
  - `src/managed/Chaos.IL2CPP.Driver/Build/BuildService.cs` — BuildService 实现
- **文件修改**：
  - `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs` — RunBuild 替换为真实调用
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/DriverEntryTests.cs` — 新增 3 个 build 测试
- **测试结果**：25 测试全部通过（新增 3：Help_ReturnsZero, UnknownFlag_ReturnsOne, NonExistentDir_ReturnsOne）
- **验证命令**：
  ```bash
  dotnet build tests/managed/Chaos.IL2CPP.Driver.Tests
  dotnet test tests/managed/Chaos.IL2CPP.Driver.Tests --no-build
  ```

## 架构审视

- **结构告警**：BuildService 独立文件，职责单一。DriverEntry.cs 中 RunBuild 从 20 行 stub 缩减为更简洁的实现。warn 不适用。
- **测试粒度**：通过 DriverEntry.Main() 间接测试 BuildService，无需 mock cmake。
- **权责图审核 (critical)**：
  - `authority`：BuildService 仅在本模块使用，无外部 authority
  - `upstream`：无
  - `downstream`：被 DriverEntry.RunBuild 调用，无其他消费者
  - `allowed_deps`：仅依赖 System.Diagnostics.Process
  - `shim_debt`：无
  - `proof`：3 个 build 测试 + 原有 22 个回归测试
