# Phase 1 Chaos.TestFramework Contract Expansion 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 和 `dev:test-driven-development` 执行本计划。

**目标：** 把 Phase 0 冻结的 archetype / capability vocabulary 落到 `Chaos.TestFramework`、declaration discovery 和 compiled catalog 的稳定 metadata contract 中。

**架构：**
- C# 声明侧：`src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
- C# 发现侧：`src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
- Python 消费侧：
  - `build/toolchains/run/testing/declarations.py`
  - `build/toolchains/run/testing/compiled_catalog.py`

**设计文档：**
- `docs/dev/in-progress/20260414-01-il2cpp-hotupdate-capability-closure/design-v1-01.md`
- `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/capability-taxonomy-freeze-v1-01.md`
- `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/solution-archetype-freeze-v1-01.md`

## 计划任务

- [ ] Task 1: 建立 TDD 基线并补齐失败测试
  - 目标文件：
    - `tests/unit/compatibility/test_chaos_test_framework_contracts.py`
    - `tests/integration/registry/test_declared_metadata_discovery.py`
  - 验收：
    - 新增测试先失败，明确缺少 archetype/capability metadata 字段

- [ ] Task 2: 扩展 `Chaos.TestFramework` 公开契约
  - 目标文件：
    - `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
  - 内容要求：
    - 新增 `ChaosSolutionArchetype : byte`
    - 新增 `ChaosHotUpdateCapability : ushort`
    - 扩展 `ChaosRuntimeFeature`
    - `ChaosUnitTestAttribute` / `ChaosBenchmarkAttribute` 新增 `Archetype` 与 `HotUpdateCapability`
    - 所有公开 enum 补齐 XML 注释
  - 验收：
    - contract 测试通过

- [ ] Task 3: 扩展 declaration discovery metadata 输出
  - 目标文件：
    - `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
  - 内容要求：
    - 正确识别新增 enum 的底层类型
    - unit / benchmark entry 输出 `archetype` 与 `hotUpdateCapability`
  - 验收：
    - fixture metadata discovery 测试通过

- [ ] Task 4: 扩展 Python catalog 消费链路
  - 目标文件：
    - `build/toolchains/run/testing/declarations.py`
    - `build/toolchains/run/testing/compiled_catalog.py`
  - 内容要求：
    - catalog payload 保留新增 metadata 字段
    - 兼容 unit / benchmark 两类 entry
  - 验收：
    - unit / integration 测试通过

- [ ] Task 5: 回写 parent roadmap / STATUS，并为 Phase 2 做切换准备
  - 目标文件：
    - `docs/dev/in-progress/20260414-01-il2cpp-hotupdate-capability-closure/STATUS.md`
    - `docs/dev/in-progress/20260414-03-phase-1-chaos-test-framework-contract-expansion/STATUS.md`
    - `docs/dev/in-progress/20260414-03-phase-1-chaos-test-framework-contract-expansion/notes/progress-v1-01.md`
  - 验收：
    - parent/child 状态与验证结果一致
    - 明确下一步切入 `SolutionCorePack` Phase 2

## 验证

- `python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py`
- 如有必要，补跑：
  - `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py`

## 执行备注

- 属性只承载内在 metadata：`Category`、`Requires`、`Archetype`、`HotUpdateCapability`。
- support-state 继续保留在外层 matrix / manifest / reporting，不进入 attribute。
- 保持紧凑类型和枚举，不引入新的 string 协议面。
