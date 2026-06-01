---
parent_task_id: cleanup-test-framework-old-attributes
source_task_id: T1
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
lifecycle_status: in-progress
---

# T1: SDK 修复 + NuGet 包重建

## Scope
修复唯一被编译且会 break 的文件（ArithmeticPatch.cs），重建 SDK NuGet 包。

## Inputs
- `testing/project-test/runtime-self-test/patch/ArithmeticPatch.cs` — 使用 `[HotUpdateSubjectId]` 旧属性
- `src/reference/Chaos.TestFramework.Sdk/` — SDK 源码（已清理，需 build）
- `src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj` — SDK 项目文件

## Expected Outputs
- `ArithmeticPatch.cs` 更新：`[HotUpdateSubjectId]` → `[HotUpdate]`
- `testing/_packages/Chaos.TestFramework.Sdk.0.1.0.nupkg` 重新生成
- NuGet 全局缓存更新

## Exit Criteria
- `dotnet build patch.csproj` 通过（使用本地 SDK 源码）
- NuGet 包 build 成功
- 全局缓存中 SDK DLL 的旧类型被清除
