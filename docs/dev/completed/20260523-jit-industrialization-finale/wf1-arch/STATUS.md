---
parent_task_id: 20260523-jit-industrialization-finale
source_task_id: 20260523-jit-industrialization-finale
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
task_type: plan
phase: planning
created: 2026-05-23
---

# wf1-arch — IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构

## Scope

将 jit_engine.cpp 中直接使用的 x64 编码器内联函数和 SEH 平台相关代码抽象为接口，使得：
1. 新增 ARM64 编码器只需实现 IEncoder 接口（无需改 jit_engine.cpp）
2. 新增 Linux SEH 实现只需实现 ISehHandler 接口
3. 现有 x64/Windows 路径零功能变更

## Inputs

- `src/native/jit/x64_encoder.h` — 当前 x64 编码器内联函数（928 行）
- `src/native/jit/jit_seh.h/cpp` — 当前 SEH 实现（Windows VEH + Linux stub）
- `src/native/jit/jit_engine.cpp` — 调用编码器的主引擎（3,715 行）
- `src/native/jit/jit_engine.h` — CompileConfig / Compile 声明

## Architecture (from parent roadmap)

- IEncoder: 纯虚接口，包含 EmitMovRM, EmitCallReg, EmitJccRel32 等所有当前 x64_encoder.h 中的公共函数
- ISehHandler: 纯虚接口，包含 Register, Unregister, RaiseException, FindCodeByAddress
- X64Encoder: 实现 IEncoder，包装当前内联函数调用（或直接保留 inline + 加适配层）
- jit_engine.cpp: 通过 IEncoder* 和 ISehHandler* 调用，不再直接依赖平台细节

## Expected Outputs

1. `src/native/jit/IEncoder.h` — 编码器接口定义（新建）
2. `src/native/jit/X64Encoder.h` — x64 编码器实现（从 x64_encoder.h 适配）
3. `src/native/jit/ISehHandler.h` — SEH handler 接口定义（新建）
4. `src/native/jit/WinSehHandler.h/cpp` — Windows VEH 实现（从 jit_seh.cpp 提取）
5. `src/native/jit/jit_engine.cpp` 重构 — 通过接口调用编码器
6. `src/native/jit/CMakeLists.txt` 更新

## Exit Criteria

- 全回归通过（无功能退化）
- 所有现有 test_codegen_native 测试通过
- 编码器接口可独立实现（x64 现有功能不变）

## Terminal Notes

（由执行终端填写）
