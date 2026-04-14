# Phase 3 Plan v1.01

Date: 2026-04-14 17:32:13 +08:00
Status: completed

## 1. 目标

把 `run test` / registry 的 subject-family 主链从“对象分发 + 历史入口混合执行”升级为“registry 找对象 + workspace manifest v2 找宿主 + generated host 执行 entry”。

## 2. 输入文档

- parent design:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent roadmap:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- phase-0 contract freeze:
  - `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- phase-2 completion:
  - `docs/dev/completed/20260414-32-phase-2-workspace-v2-and-native-test-project-generation/STATUS.md`

## 3. 执行 checklist

### 3.1 RED

- 为 `run test subject` 增加 failing tests，锁定它优先从 workspace manifest v2 解析 managed/native test host。
- 为 `run test declared-unit-test` / `run test declared-benchmark` 增加 failing tests，锁定 registry object 会解析到 workspace host，而不是继续绕回旧的 subject 内部分发。
- 为 engineering validation / workload 增加保护测试，确保它们继续保持 pipeline 语义，不被误伪装成 declared proof / benchmark。

### 3.2 GREEN

- 引入 workspace host resolution helper，把 registry object 与 workspace manifest v2 接起来。
- 更新 `build/toolchains/run/commands/test.py`，让 subject/declared family 优先消费 workspace manifest v2。
- 保持 engineering validation / workload 仍走现有 pipeline/object 执行路径，但与统一入口共享 report / artifact spine。

### 3.3 REFACTOR

- 收口 `test.py` 中 subject-family 分支的重复解析逻辑。
- 删除 registry-object compatibility alias、legacy run shim 与 dead legacy dispatch code。
- 为 benchmark command / dashboard 复用同一套 workspace declared catalog / host lookup surface。

## 4. 验收标准

- `run test subject --id subject/<id>` 可以基于 workspace manifest v2 找到并执行宿主。
- `run test declared-unit-test --id declared-unit-test/<stableId>` 与 `run test declared-benchmark --id declared-benchmark/<stableId>` 通过 registry object -> workspace host -> generated host 执行。
- engineering validation / workload 继续保持 pipeline 语义，且不会因 cutover 丢失统一入口或 report/artifact 记录。

## 5. 完成记录

- `build/toolchains/run/commands/test.py` 已完成 workspace manifest v2 驱动的 `subject` / `declared-unit-test` / `declared-benchmark` 执行收口，并保留 engineering validation / workload 的 pipeline 语义边界。
- `build/toolchains/run/commands/benchmark.py`、`build/toolchains/run/testing/benchmark_dashboard_generator.py`、`build/toolchains/run/testing/workspace_declared_catalog.py` 已完成 benchmark catalog 与 dashboard 的 workspace declared catalog 收口。
- registry-object compatibility alias、`tests/unit/run` legacy shim、部分 `tests/integration/run` legacy shim 与 dead legacy dispatch code 已删除。
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs` 已补齐 `conv.i` / `conv.u` 的 loader 支持，恢复 native-aot workload bundle。
- 2026-04-14 18:37:13 +08:00：`python -m pytest` 全量通过。
