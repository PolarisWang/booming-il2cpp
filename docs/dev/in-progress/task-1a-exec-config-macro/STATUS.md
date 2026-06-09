# Task 1a: execution_config.h 重写为纯宏

- **parent_task_id**: 20260604-exec-arch-reform
- **source_relation**: roadmap-child
- **phase**: 1
- **clearance_source**: parent-roadmap
- **blocking_questions**: []
- **question_clearance**: cleared
- **clearance_confirmed_by_user**: true
- **entry_skill**: dev-executing-plans

## Scope

将 `execution_config.h` 中的 `ExecutionConfig` struct + `if constexpr` 替换为纯 `#if CHAOS_IL2CPP_ENABLE_JIT` / `#if CHAOS_IL2CPP_ENABLE_AOT` 宏。移除 `ExecutionProfile` enum。保留配置文件的 CMake 分发生成（desktop.h / mobile-release.h / test-only.h）。

## Inputs

- `src/native/runtime-core/chaos/runtime/execution_config.h`

## Expected Outputs

- `src/native/runtime-core/chaos/runtime/execution_config.h` — 重写为纯宏守卫
- `cmake/runtime-config/` — 三个 profile 文件更新为宏定义

## Exit Criteria

- 文件重写完成，`ExecutionConfig` struct 移除
- `ExecutionProfile` enum 移除
- 编译通过（desktop preset）
