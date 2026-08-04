# Subject Generated Test Workspace And Unified Test Entry Roadmap v1.01

Date: 2026-04-14 16:17:43 +08:00
Status: completed

## 0. 完成结论

- Phase 0-2 已冻结 workspace manifest v2、declared catalog v2、四类工程生成与 generated managed/native test host contract。
- Phase 3 已把 `run test` / registry / selector 主链切到 workspace manifest v2，并统一 `subject`、`declared-unit-test`、`declared-benchmark` 的 host 解析。
- Phase 4 已把 benchmark command 与 dashboard 收口到 workspace declared catalog，补齐 `entryIndex` 透传，并按 managed baseline 重建对比布局。
- Phase 5 已删除 registry-object compatibility alias、legacy run shim 与 dead legacy dispatch code，完成本路线定义的历史入口清理。
- 2026-04-14 18:37:13 +08:00：全量验证 `python -m pytest` 通过，结果为 `612 passed, 40 skipped`。

## 1. 目标

在当前 `subject + compiled catalog + registry + unified run command` 基础上，完成一次结构性升级：

- 为每个 `subject` 生成四类工程：
  - managed projects
  - managed test projects
  - native projects
  - native test projects
- 让 `ChaosUnitTest` / `ChaosBenchmark` 成为 proof / benchmark 的唯一声明源。
- 让 `run generate/build/test` 全部围绕 workspace manifest v2 运行。
- 系统性删除历史手工注册、旧 proof host、旧兼容字段和旧测试壳，保持仓库整洁。

## 2. 范围边界

本 roadmap 负责：

- 冻结 workspace v2 contract、generated host contract 和 legacy cleanup inventory
- 把 declared metadata 从“可发现”推进到“可生成 managed/native test host”
- 升级 subject workspace 为四类工程一等模型
- 统一 CLI / registry / executor 对 workspace manifest 的消费方式
- 跑通 benchmark/proof 的 managed/native 主链，并同步 dashboard
- 删除被新模型替代的旧逻辑

本 roadmap 不负责：

- 把 engineering validation / workload 全部改写成 attribute
- 为 declared test v1 支持所有复杂方法签名
- 扩大新的 string 协议面
- 新增新的顶层 subject 分类

## 3. 非目标

- 不做“所有测试都物理变成 native test project”这类误导性收口
- 不把 `solutions/subjects/` 变成手工维护真源
- 不保留长期 compatibility alias
- 不为了短期平滑迁移而接受长期双轨

## 4. 阶段列表

### Phase 0: Contract Freeze And Legacy Inventory

- 冻结 workspace manifest v2
- 冻结 generated host contract
- 列出全部旧逻辑删除清单和迁移顺序

### Phase 1: Declared Catalog To Managed Test Host

- 升级 declared catalog 产物
- 生成 managed proof host / benchmark host
- 把手工 managed 注册逻辑替换为 generated host

### Phase 2: Workspace v2 And Native Test Project Generation

- 让 `project_workspace.py` 生成四类工程
- 引入 `managedTestProjects` / `nativeTestProjects`
- native test host 改为基于 generated managed host 翻译结果生成

### Phase 3: Unified Test Entry And Registry Cutover

- `run test ...` 改为优先消费 workspace manifest v2
- declared unit / benchmark 直接定位 generated host
- engineering validation / workload 保持 pipeline 语义，但统一到同一入口和 artifact spine

### Phase 4: Benchmark Native Runner And Dashboard Closure

- benchmark managed/native runner 跑通
- 以 managed baseline 为基线重做对比布局
- 修复历史 benchmark 缺失数据和 identity 漂移

### Phase 5: Subject Migration And Legacy Deletion

- 迁移 retained subjects 到新模型
- 删除旧 proof host / 手工注册 / manifest 旧字段 / 旧兼容入口
- 清理 tests 中残留的 subject / phase 历史耦合

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 冻结后续所有 child task 都要遵守的 contract 和清理边界。
- `exit_criteria`
  - workspace manifest v2 字段集合固定。
  - generated managed/native host contract 固定。
  - legacy inventory 至少覆盖：
    - 手工注册
    - 旧 proof host
    - 旧 manifest 字段
    - 旧 registry / selector compat path
    - tests 中历史耦合逻辑
- `deliverables`
  - contract freeze note
  - legacy cleanup inventory
  - migration order note
- `dependencies`
  - 无
- `open_questions`
  - 无阻塞问题；本阶段以 design 文档中的 v1 method contract 为默认假设。

### Phase 1

- `goal`
  - 把 declared metadata 真正变成 managed test host 生成输入。
- `exit_criteria`
  - 任一 subject 的 declared unit test / benchmark 都能生成 managed host。
  - managed host 不再依赖手工注册。
  - generated file 中内部调度使用 `entryIndex` 或等价数值索引，而不是继续扩大 string 协议。
- `deliverables`
  - declared catalog v2
  - generated managed proof host
  - generated managed benchmark host
  - 红线测试：无 attribute 时、框架引用缺失时、catalog 不完整时的契约测试
- `dependencies`
  - Phase 0
- `open_questions`
  - 如后续需要异步或实例方法支持，作为 follow-up，不阻塞 v1。

### Phase 2

- `goal`
  - 把 workspace 生成升级到四类工程的一等模型。
- `exit_criteria`
  - `workspace.manifest.json` 写入 v2 字段。
  - `run generate project subject` 结果中可见 managed/native test project。
  - native test host 使用 generated managed host 的翻译结果，不再手工维护第二套测试列表。
- `deliverables`
  - subject workspace v2
  - build selector 升级
  - per-matrix native/native-test project generation
- `dependencies`
  - Phase 0
  - Phase 1
- `open_questions`
  - device/package matrix 的 native test project 以 package-root 表达，不阻塞 contract 冻结。

### Phase 3

- `goal`
  - 完成 unified test entry 的主链切换。
- `exit_criteria`
  - `run test subject`
  - `run test declared-unit-test`
  - `run test declared-benchmark`
  - 三条路径都能通过 workspace manifest v2 找到执行宿主。
  - engineering validation / workload 仍能工作，且 artifact/report spine 与新模型对齐。
- `deliverables`
  - test command cutover
  - registry object to workspace-host resolution
  - selector / report / artifact contract update
- `dependencies`
  - Phase 1
  - Phase 2
- `open_questions`
  - 若 registry 中仍有纯历史 alias，对应删除工作放到 Phase 5，但 Phase 3 不再新增。

### Phase 4

- `goal`
  - 让 benchmark 从“能跑”升级为“结果正确且能展示”。
- `exit_criteria`
  - managed baseline 成为 dashboard 对比基线。
  - native benchmark 数据不再大面积缺失；缺失项必须可解释。
  - dashboard 能下钻到 solution 中更细的 declared benchmark 条目。
- `deliverables`
  - benchmark runner closure
  - dashboard layout redesign
  - missing-data diagnosis and fixes
- `dependencies`
  - Phase 2
  - Phase 3
- `open_questions`
  - 若少量 matrix 仍然不支持 benchmark，必须输出明确 unsupported reason code。

### Phase 5

- `goal`
  - 把旧逻辑彻底删掉，并清理迁移后的仓库结构。
- `exit_criteria`
  - 旧手工 proof / benchmark 注册入口删除。
  - 旧 manifest 字段和 compat path 删除。
  - 不再保留只服务历史入口的 subject 项目或测试壳。
  - `tests/` 中直接耦合历史 subject / phase 命名的逻辑被替换。
- `deliverables`
  - legacy deletion patch set
  - migrated subject inventory
  - cleanup report
- `dependencies`
  - Phase 3
  - Phase 4
- `open_questions`
  - 无；本阶段默认以“清理优先于保留兼容壳”为原则。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory` | `Phase 0` | `completed` | `codex` | 冻结 workspace v2 / generated host contract，并列出旧逻辑删除清单 | `-` |
| `20260414-31-phase-1-declared-catalog-to-managed-test-host` | `Phase 1` | `completed` | `codex` | 把 declared catalog 接到 generated managed proof/benchmark host | `20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory` |
| `20260414-32-phase-2-workspace-v2-and-native-test-project-generation` | `Phase 2` | `completed` | `codex` | 生成四类工程并完成 native test host 工程化 | `20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory, 20260414-31-phase-1-declared-catalog-to-managed-test-host` |
| `20260414-33-phase-3-unified-test-entry-and-registry-cutover` | `Phase 3` | `completed` | `codex` | 切换 `run test` / registry / selector 到 workspace manifest v2 主链 | `20260414-31-phase-1-declared-catalog-to-managed-test-host, 20260414-32-phase-2-workspace-v2-and-native-test-project-generation` |
| `20260414-34-phase-4-benchmark-native-runner-and-dashboard-closure` | `Phase 4` | `completed-inline` | `codex` | 在 Phase 3 收口后的同一主线上完成 managed/native benchmark closure、缺失数据修复与 dashboard 调整 | `20260414-32-phase-2-workspace-v2-and-native-test-project-generation, 20260414-33-phase-3-unified-test-entry-and-registry-cutover` |
| `20260414-35-phase-5-subject-migration-and-legacy-deletion` | `Phase 5` | `completed-inline` | `codex` | 删除 registry alias、legacy run shim、旧 dispatch 代码并清理迁移后的历史入口 | `20260414-33-phase-3-unified-test-entry-and-registry-cutover, 20260414-34-phase-4-benchmark-native-runner-and-dashboard-closure` |

## 7. 依赖

- Phase 0 是整个 roadmap 的冻结入口，没有它，后续 child task 会边做边改 contract。
- Phase 1 是 Phase 2 和 Phase 3 的前置，因为 generated host 是 workspace v2 和 unified entry 的核心桥梁。
- Phase 2 与 Phase 3 紧耦合，但可以拆成两个 child task：先生成四类工程，再切 test entry。
- Phase 4 必须在 Phase 3 之后，因为 benchmark identity 和 runner 依赖新的 host resolution。
- Phase 5 最后执行，负责把所有临时兼容层和历史壳清掉。

## 8. 风险

### 风险 1：四类工程模型做出来，但执行主链仍绕回旧入口

- 缓解：
  - Phase 3 以“workspace manifest v2 主链切换”为明确出口。

### 风险 2：managed host 和 native host 出现两套 identity

- 缓解：
  - 从 Phase 1 开始，强制 stableId / entryIndex 双轨字段同时存在且来源一致。

### 风险 3：为了平滑迁移保留过多 shim，导致 cleanup 失败

- 缓解：
  - Phase 5 作为独立 child task，不与功能开发混写。

### 风险 4：benchmark 直到后期才发现缺失数据

- 缓解：
  - 单独设 Phase 4，不把 benchmark closure 附着在 Phase 3 后面顺手处理。

## 9. 备选路径

### 备选路径 A：只重做 workspace，不改 test entry

- 优点：
  - 早期改动更小。
- 缺点：
  - 旧入口仍会绕过新 contract，收益有限。

### 备选路径 B：先切 test entry，再补 workspace v2

- 优点：
  - CLI 变化更快可见。
- 缺点：
  - 缺少稳定 contract，切换风险高。

### 当前判断

- 推荐保持当前主路线：
  - 先冻结 contract
  - 再生成 managed host
  - 再升级 workspace
  - 再切入口
  - 再收 benchmark
  - 最后删旧逻辑

## 10. 当前建议推进顺序

1. 先执行 `20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory`，冻结 contract 和清理 inventory。
2. 再执行 `20260414-31-phase-1-declared-catalog-to-managed-test-host`，把 attribute 发现在 managed host 层真正落地。
3. 之后执行 `20260414-32-phase-2-workspace-v2-and-native-test-project-generation`，把四类工程模型做实。
4. 再执行 `20260414-33-phase-3-unified-test-entry-and-registry-cutover`，切换统一测试入口。
5. 再执行 `20260414-34-phase-4-benchmark-native-runner-and-dashboard-closure`，把 benchmark 跑通并让 dashboard 正确展示。
6. 最后执行 `20260414-35-phase-5-subject-migration-and-legacy-deletion`，彻底清掉旧逻辑和历史壳。
