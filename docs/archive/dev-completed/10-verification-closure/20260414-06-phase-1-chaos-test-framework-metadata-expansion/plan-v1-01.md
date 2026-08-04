# Phase 1 Chaos TestFramework Metadata Expansion 实现计划

> 面向执行 Agent：必须继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。
> 目标：把 Phase 0 冻结的 capability taxonomy 引入 `Chaos.TestFramework`、declaration discovery 与 compiled catalog，并用测试锁定 contract。
> 架构：先用 fixture 和 contract tests 定义新字段，再修改 C# framework、metadata discovery、Python normalization/catalog。当前阶段只打通 capability family / capability item，不把 support-state 矩阵塞进 attribute。
> 技术栈：C#、.NET metadata reader、Python catalog tooling、`pytest`

## 目标文件

- `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
- `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
- `build/toolchains/run/testing/declarations.py`
- `build/toolchains/run/testing/compiled_catalog.py`
- `build/toolchains/run/testing/declared_metadata_labels.py`
- `tests/unit/compatibility/test_chaos_test_framework_contracts.py`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/fixtures/contracts/declared-metadata/DeclaredEntries/DeclaredProofs.cs`

## 执行步骤

- [x] 为 framework contract 增加 capability family / capability item 断言
- [x] 为 fixture 声明增加 capability family / capability item 字段
- [x] 为 compiled catalog / discovery 输出增加断言
- [x] 实现 `ChaosCapabilityFamily : byte`
- [x] 实现 `ChaosCapabilityItem : ushort`
- [x] 扩展 `ChaosUnitTestAttribute` / `ChaosBenchmarkAttribute`
- [x] 扩展 declaration discovery 读取新字段
- [x] 扩展 declarations / compiled catalog normalization
- [x] 补充 label helpers
- [x] 运行相关测试并收口状态文档

## 验证

- `python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py -q` `3 passed`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` `8 passed`

## 状态维护

- 先做 RED，再做实现，再跑 GREEN
- 每一轮测试结果写回 `STATUS.md` 与 `notes/progress-*.md`
