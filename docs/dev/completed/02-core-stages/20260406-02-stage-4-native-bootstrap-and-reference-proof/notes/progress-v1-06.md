# 进度记录 v1-06

## 时间

- 2026-04-06 10:37:52 +08:00

## 已完成

- 将当前 Stage 4 相关 native contract / codegen / runtime-core / tests / 文档中的旧 BOOM/Boom 系前缀统一切换为 `CHAOS_*`、`chaos_*`、`chaos::`、`Chaos.`。
- 更新 `contracts/native/v0/runtime_abi.h` 与 `contracts/native/v0/codegen_bridge.h` 的公共宏、状态码与导出函数命名，使其与仓库主命名空间保持一致。
- 同步更新 `src/managed/Chaos.IL2CPP.CodeGen` 的 Scriban 模板和 emitter，让 Stage 4 生成的 C++ 改为输出 `CHAOS_RUNTIME_ABI_CALL` 与 `CHAOS_BRIDGE_STATUS_OK`。
- 同步更新 `src/native/runtime-core/`、contracts examples、compile-only smoke、Stage 4 相关测试和文档中的旧前缀引用。
- 新增 `tests/unit/run/test_native_prefix_naming.py`，用于扫描当前 native / Stage 4 相关目录，防止旧的 `BOOM*` 前缀再次混入。

## 验证

- `python -m unittest tests.unit.run.test_native_prefix_naming`
- `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`
- `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- `python -m unittest tests.unit.run.test_stage1b_layout`
- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`

## 当前状态

- 任务状态：`in_progress`
- 当前阶段：`implementation`
- 说明：命名统一已经完成并被回归测试锁住；真实 native 编译验证仍受 `cmake` / C++ 编译器缺失阻塞。

## 下一步

- 若继续推进功能主线，下一块仍是恢复工具链后验证 `runtime-core` 的真实编译；或者在当前环境下继续盲写 Chunk 4 的 `bootstrap/support` 骨架。
