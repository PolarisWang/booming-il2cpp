# C3 Verification Topology And Unified Entry Design v1.01

Date: 2026-04-24 00:00:00 +08:00
Status: draft-approved-for-roadmap

## 1. 目标

在保留 `verification-v1` 的核心验证模型前提下，对验证目录、生成型 workspace、owner 证据落点和人类入口做一次性 `hard cut` 重构，形成一个更纯净的统一验证根，并清理所有旧测试框架的公共路径与文档入口。

本设计要同时满足以下目标：

- 验证 authority 有唯一正式根，不再分散在 `docs/`、`subjects/`、`solutions/`
- `run.py` 继续是唯一真实执行入口
- 人类可通过一个稳定生成的总 `.sln` 查看所有项目并直接调试
- owner 对照证据仍然保留，但必须收口到新根内，不再散落到 `subjects/*/verification/*`
- 旧测试框架的公共入口、路径、wiki、skill 约束在 cutover 后全部切干净
- 后续 roadmap 可以按 checklist 逐项推进并验收

## 2. 非目标

- 本轮不重写 `verification-v1` 的 Claim Graph / Projection 基本语义
- 本轮不新增新的测试能力域或新的 route lane
- 本轮不保留 dual-write、shadow path、兼容 alias
- 本轮不把 `.sln` 升格为真实执行入口

## 3. 设计结论

### 3.1 总体方案

采用：

- 总体方案：`Full Authority Migration`
- cutover 方式：`one-shot hard cut`
- 新根内部分层：`职责分层型（方案 A）`

这三个拍板共同形成最终结论：

- 新建顶层 `verification/` 作为唯一正式验证根
- `docs/testing-inventory/verification/*` 与 `subjects/*/verification/*` 全部退出 authority
- 所有验证相关的生成型 workspace / total solution 统一落在 `verification/workspaces/*`
- 旧测试框架公共入口在 cutover 后必须全部删除，而不是保留兼容层

### 3.2 新根结构

最终目标结构固定为：

```text
verification/
  INDEX.md
  verification.manifest.json
  catalog/
    owners/
      <owner-id>/
        owner.manifest.json
        owner.features.json
        proofs/
        benchmarks/
    scenarios/
    routes/
    labs/
  archive/
    latest/
    master/
    reports/<closure-kind>/<scope-code>/
  evidence/
    owners/
      <owner-id>/
        codegen-stubs/
  workspaces/
    verification-all.sln
    verification-all.manifest.json
    subjects/<subject-id>/
    routes/<route-id>/
    core/<host-platform>/
  projections/
    testing-inventory/
    benchmark/
    dashboards/
```

说明如下：

- `catalog/`
  - 新测试体系的结构 authority 根
  - 承接原 `subjects/*` 里属于测试体系的 owner / feature / proof / benchmark / scenario / route / lab 定义
- `archive/`
  - 正式 verification authority
  - 承接原 `docs/testing-inventory/verification/*`
- `evidence/`
  - owner-local verification evidence authority
  - 承接原 `subjects/*/verification/codegen-stubs/*`
- `workspaces/`
  - 人类入口与 IDE facade
  - 所有验证相关的生成型 `.sln` 和 workspace manifest 必须落在这里
- `projections/`
  - 面向人类的 inventory / dashboard / HTML / CSV / JSON 投影视图
  - 不承载 formal authority
- `verification.manifest.json`
  - 新根的总索引
  - 连接 archive / owners / workspaces / projections / run command
- `INDEX.md`
  - 面向人的统一导航首页

## 4. 职责边界

### 4.1 `run.py`

`build/toolchains/run/run.py` 继续是唯一真实执行入口。

它负责：

- 解析命令
- 调度 verifier / generator / reporter
- 驱动 `verification/` 根内的生成与刷新

它不负责：

- 成为 formal authority
- 成为人类浏览载体
- 手工维护总 `.sln`

### 4.2 `verification/archive`

`verification/archive/*` 是新的正式验证 authority。

它负责：

- `latest`
- `master`
- `reports`
- formal evidence snapshot / merged master / report summary

它不负责：

- IDE workspace
- owner compare 布局
- 手工浏览优化

### 4.2 `verification/catalog`

`verification/catalog/*` 是新测试体系的结构 authority 根。

它负责：

- owner catalog
- migrated `subject.manifest` / `subject.features`
- proof declarations
- benchmark declarations
- scenario catalog
- route catalog
- lab catalog

它不负责：

- formal merged archive
- owner evidence payload
- IDE workspace

### 4.3 `verification/evidence`

`verification/evidence/owners/<owner-id>/*` 是 owner 侧证据 authority。

它负责：

- codegen stubs
- 与 owner subject 直接相关的验证存根

它不负责：

- formal merged archive
- 主导航首页
- CLI 主入口

### 4.4 `verification/workspaces`

`verification/workspaces/*` 是统一的人类入口。

它负责：

- `verification-all.sln`
- subject / route / core 的生成型 workspace
- workspace manifest

它不负责：

- formal authority
- owner evidence authority
- 真实执行

### 4.5 `verification/projections`

`verification/projections/*` 是只读投影视图。

它负责：

- inventory html/json/csv
- benchmark dashboard
- 统一展示页

它不负责：

- 反向定义 authority
- 自行决定 support state / verification state

## 5. Workspace 设计

### 5.1 总 `.sln`

总 `.sln` 固定为：

- `verification/workspaces/verification-all.sln`

约束如下：

- 路径稳定
- 内容生成
- 不允许手工维护
- 不允许回落到旧 `solutions/`

### 5.2 生成型 workspace 落点

验证相关 workspace 统一迁移到：

- `verification/workspaces/subjects/<subject-id>/`
- `verification/workspaces/routes/<route-id>/`
- `verification/workspaces/core/<host-platform>/`

这意味着以下旧路径退出主线：

- `solutions/subjects/*`
- `solutions/core/*`
- 所有以旧测试框架为中心的验证 workspace 主入口

### 5.3 人类体验

人类入口固定为：

1. `verification/INDEX.md`
2. `verification/workspaces/verification-all.sln`
3. `run verify *`
4. `run test *`

解释：

- `INDEX.md` 负责导航
- `.sln` 负责 IDE / 查看 / 本地调试
- `run.py` 负责真实执行

## 6. Data Flow

新的主数据流固定为：

```text
run.py
  -> verification planners / generators
  -> verification/archive/*
  -> verification/owners/*
  -> verification/workspaces/*
  -> verification/projections/*
  -> verification/verification.manifest.json
```

其中：

- formal objects 先写入 `archive`
- owner compare artifacts 写入 `owners`
- workspace 生成器写入 `workspaces`
- projections 从 `archive` 派生，不反向改写 authority

## 7. Hard Cut 规则

### 7.1 必须一次性切换的对象

以下对象必须在同一批次切换到新根：

- generator
- inventory refresh
- verification bundle writer
- codegen stub writer
- workspace generator
- CLI public outputs
- TUI public outputs
- wiki 入口
- skill / governance 文档中的正式路径
- 自动化测试断言

### 7.2 禁止事项

hard cut 后禁止：

- dual-write
- shadow path
- public alias
- 保留旧 formal authority 目录
- 保留旧公共测试框架入口文字

### 7.3 失败策略

cutover 失败时不允许“回写旧路径补救”，而是直接视为失败并阻断完成。

阻断条件包括：

- 新根未生成完整 formal archive
- 总 `.sln` 仍写到旧 `solutions/`
- owner codegen stubs 仍写到 `subjects/*/verification/*`
- CLI / TUI / wiki 仍引用旧测试框架主入口

## 8. 旧测试框架清理范围

### 8.1 路径清理

必须清理：

- `docs/testing-inventory/verification/*`
- `subjects/*/verification/*`
- 旧测试框架相关的 `solutions/*` 公共验证入口

### 8.1a `subjects/` 清理边界

本任务修正后的目标是：

- `subjects/` 不再作为新测试体系的长期主载体
- 当前 `subjects/*` 中属于测试体系的承载内容，都要迁到新根

需要从 `subjects/*` 迁出的内容包括：

- `subject.manifest.json`
- `subject.features.json`
- `source/Proofs/**`
- `source/Benchmarks/**`
- `source/EngineeringScenarios/**`
- 任何 testing-only host / workspace source
- `subjects/*/verification/*`

迁出后的新落点为：

- owner authority -> `verification/catalog/owners/*`
- scenario authority -> `verification/catalog/scenarios/*`
- route authority -> `verification/catalog/routes/*`
- lab authority -> `verification/catalog/labs/*`
- owner evidence -> `verification/evidence/owners/*`
- generated workspace -> `verification/workspaces/*`

因此，`subjects/` 的清理时机不是只删一个子目录，而是：

- Phase 1 先把 `subjects/*` 中的测试 authority 全量复制/重建到 `verification/catalog/*`
- Phase 2/3 完成 registry / planner / CLI / TUI / tests 对新 catalog 的切换
- Phase 4 做一次性 `hard cut`，删除旧 `subjects/*` 中属于测试体系的全部承载内容

执行期允许把当前 `subjects/*` 作为迁移输入源，但最终 cutover commit 中，`subjects/` 必须退出测试主线。

### 8.2 公共入口清理

必须清理：

- 旧 CLI public command 文案
- 旧 TUI public menu 文案
- 旧 manifest public output path
- 旧 wiki 的正式入口描述
- 旧 skill/guidance 中的测试框架路径与流程表述

### 8.3 代码级清理

必须清理：

- 所有旧 formal path 常量
- 所有旧 verification sink helper
- 所有旧 workspace output helper
- 所有旧 public path scan / index generation 逻辑

### 8.4 `tests/` 下 Python 脚本调整范围

本次 hard cut 会直接影响 `tests/` 下大量 Python 脚本，调整原则如下：

- 不保留继续断言旧路径的测试
- 不允许为了兼容旧路径而在测试里保留双断言
- 所有测试最终只承认新根 `verification/*`

### 8.4a `tests/` 的最终物理落点

`tests/` 不再继续把 verification framework 的自动化验证分散在旧的混合目录中。hard cut 完成后，verification-framework 专属测试统一收口到：

```text
tests/
  verification/
    unit/
      archive/
      catalog/
      evidence/
      projections/
        testing-inventory/
        benchmark/
      workspaces/
    integration/
      pipeline/
      catalog/
      workspaces/
    tooling/
      run/
    contracts/
```

目录职责固定为：

- `tests/verification/unit/archive/`
  - formal archive / latest / master / reports writer 的单元测试
- `tests/verification/unit/catalog/`
  - owner / feature / proof / benchmark / scenario / route / lab catalog 的解析、schema 与稳定键测试
- `tests/verification/unit/evidence/`
  - owner evidence / codegen stubs 写入、覆盖与 stable path 测试
- `tests/verification/unit/projections/testing-inventory/`
  - testing inventory 生成、列定义、tooltip、中文 hint 测试
- `tests/verification/unit/projections/benchmark/`
  - benchmark inventory / dashboard / tooltip / 中文说明测试
- `tests/verification/unit/workspaces/`
  - verification workspace manifest、path key 与生成参数测试
- `tests/verification/integration/pipeline/`
  - `run verify` / stage execution / summary / report / merged outputs 的端到端测试
- `tests/verification/integration/catalog/`
  - catalog scan / registry expansion / discovery / projection feed 的端到端测试
- `tests/verification/integration/workspaces/`
  - total `.sln`、subject workspace、route workspace、core workspace 生成测试
- `tests/verification/tooling/run/`
  - CLI / TUI / manifest / JSON output / importantOutputs / help text 等 public entry 测试
- `tests/verification/contracts/`
  - verification schema / manifest / report / projection contract 测试

旧目录到新目录的物理迁移规则固定为：

- `tests/unit/verification/*`
  - 按职责拆到 `tests/verification/unit/archive/*`
  - 或 `tests/verification/unit/evidence/*`
  - 或 `tests/verification/unit/projections/testing-inventory/*`
- `tests/unit/performance/` 中与 testing inventory / benchmark projection 直接相关的脚本
  - 移到 `tests/verification/unit/projections/testing-inventory/*`
  - 或 `tests/verification/unit/projections/benchmark/*`
- `tests/tooling/run/` 中 verification 专属脚本
  - 移到 `tests/verification/tooling/run/*`
- `tests/integration/run/` 中 verification pipeline 专属脚本
  - 移到 `tests/verification/integration/pipeline/*`
- `tests/integration/registry/*`
  - 移到 `tests/verification/integration/catalog/*`
- `tests/unit/compatibility/*subject*` 与其他 subject discovery / projection / catalog 脚本
  - 移到 `tests/verification/unit/catalog/*`
  - 或 `tests/verification/integration/catalog/*`
- `tests/contracts/*` 中 verification 专属 contract 脚本
  - 移到 `tests/verification/contracts/*`
- `tests/tooling/run/test_tui_legacy_*` 与所有只验证旧 public path / old menu / old alias 仍存在的脚本
  - 直接删除，不迁移

以下目录默认原地保留，不属于这次 verification hard cut 的物理迁移对象：

- `tests/tooling/run/*` 中与 bootstrap / doctor / generic build / generic wrapper 相关的通用命令测试
- `tests/contracts/managed/*`
- `tests/contracts/native/*`
- `tests/contracts/shared/*`
- `tests/contracts/trace/*`
- `tests/perf/*` 与其他不依赖 verification public path 的性能基础设施测试

受影响脚本分为四类：

1. `verification` / `inventory` / `bundle` 断言类
   - 典型路径：
     - `tests/unit/verification/*`
     - `tests/unit/performance/test_testing_inventory_generator.py`
     - `tests/tooling/run/test_subject_command_inventory.py`
   - 需要改动：
     - `docs/testing-inventory/verification/* -> verification/archive/*`
     - `subjects/*/verification/codegen-stubs/* -> verification/owners/*/codegen-stubs/*`
     - `docs/testing-inventory/* -> verification/projections/testing-inventory/*`

2. `run` / `CLI` / `manifest` / `TUI` 输出断言类
   - 典型路径：
     - `tests/tooling/run/*`
     - `tests/integration/run/*`
   - 需要改动：
     - 命令默认输出根
     - `importantOutputs`
     - `summaryPath`
     - `workspace` / `.sln` 路径
     - public menu / help / manifest 文案

3. 旧框架遗留入口断言类
   - 典型路径：
     - `tests/tooling/run/test_tui_legacy_*`
     - 任何专门断言旧 public path / old menu / old alias 的脚本
   - 需要改动：
     - 如果测试目标只是旧入口存在性，直接删除
     - 如果测试目标是长期行为，需要改写成新入口断言

4. 低耦合 contract / compatibility / foundation 类
   - 典型路径：
     - `tests/contracts/*`
     - `tests/unit/compatibility/*`
   - 需要改动：
     - 仅当它们引用 old verification path / old workspace sink 时才改
     - 不做无关的机械性重写

5. subject-based discovery / projection / catalog 类
   - 典型路径：
     - `tests/integration/registry/*`
     - `tests/unit/compatibility/*subject*`
     - 所有默认从 `subjects/*` 扫描 owner / proof / benchmark / scenario 的脚本
   - 需要改动：
     - 从 `subjects/*` discovery 改为 `verification/catalog/*`
     - 主线完成后不再允许测试断言“subject 目录下存在测试 authority”

## 9. Wiki 与 Skill 切换要求

### 9.1 wiki

至少需要同步更新：

- `wiki/06-测试验证/*`
- `wiki/02-Skill体系/04-质量保障/project-test-governance.md`
- 与统一测试入口、验证路径、formal archive 相关的索引文档

更新要求：

- 新正式路径只写 `verification/*`
- 不保留旧 `docs/testing-inventory/verification/*` 为正式路径
- 不保留旧 `subjects/*/verification/*` 为正式路径

### 9.2 skill / governance

至少需要同步更新：

- `project-test-governance`
- 统一 run/test/verify 相关说明
- skill registry 中涉及验证入口的描述

更新要求：

- 只允许新 authority 根
- 明确总 `.sln` 是 IDE facade，不是真实执行入口
- 明确 `run.py` 是唯一真实执行入口

## 10. Roadmap Checklist

下游必须进入 roadmap，而不是单次 plan。

### Phase 0: Contract Freeze

- [ ] 冻结 `verification/` 根结构
- [ ] 冻结 `archive / owners / workspaces / projections` 边界
- [ ] 冻结总 `.sln` 与 workspace manifest 固定路径
- [ ] 冻结旧路径清理清单
- [ ] 冻结 wiki / skill 切换清单

### Phase 1: Writer Cutover

- [ ] formal archive writer 切到 `verification/archive/*`
- [ ] owner stub writer 切到 `verification/evidence/owners/*`
- [ ] inventory / projection writer 切到 `verification/projections/*`
- [ ] verification manifest/index 生成到新根
- [ ] owner / feature / proof / benchmark / scenario catalog 写到 `verification/catalog/*`

### Phase 2: Workspace Cutover

- [ ] `verification-all.sln` 生成到 `verification/workspaces/`
- [ ] subject workspace 生成到 `verification/workspaces/subjects/*`
- [ ] route workspace 生成到 `verification/workspaces/routes/*`
- [ ] core workspace 生成到 `verification/workspaces/core/*`
- [ ] 清理旧 `solutions/*` 验证相关主入口

### Phase 3: Public Entry Cutover

- [ ] CLI 输出路径与文案切到新根
- [ ] TUI 输出路径与文案切到新根
- [ ] manifest / registry public path 切到新根
- [ ] 统一首页 `verification/INDEX.md` 接通

### Phase 4: Legacy Framework Cleanup

- [ ] 删除旧 `docs/testing-inventory/verification/*`
- [ ] 删除旧 `subjects/*` 中全部测试体系承载内容
- [ ] 删除旧 public alias / old path constants
- [ ] 删除旧测试框架公共说明与入口残留
- [ ] 删除仅用于断言旧入口存在性的 Python 测试脚本
- [ ] 保留并改写长期有效的 run / verification / inventory 测试脚本

### Phase 5: Wiki And Skill Cleanup

- [ ] 更新 wiki 正式入口
- [ ] 更新 skill governance 文案
- [ ] 更新索引文档
- [ ] 扫描并清理旧路径引用
- [ ] wiki 中明确说明 `subjects/` 本轮只删除旧 `verification/*`，不等于清空 subject authority

### Phase 5a: Python Test Cutover

- [ ] 创建 `tests/verification/unit/archive/*`
- [ ] 创建 `tests/verification/unit/catalog/*`
- [ ] 创建 `tests/verification/unit/evidence/*`
- [ ] 创建 `tests/verification/unit/projections/testing-inventory/*`
- [ ] 创建 `tests/verification/unit/projections/benchmark/*`
- [ ] 创建 `tests/verification/unit/workspaces/*`
- [ ] 创建 `tests/verification/integration/pipeline/*`
- [ ] 创建 `tests/verification/integration/catalog/*`
- [ ] 创建 `tests/verification/integration/workspaces/*`
- [ ] 创建 `tests/verification/tooling/run/*`
- [ ] 创建 `tests/verification/contracts/*`
- [ ] 将 `tests/unit/verification/*` 按职责拆分迁移到新树
- [ ] 将 verification 专属 `tests/tooling/run/*` 迁到 `tests/verification/tooling/run/*`
- [ ] 将 verification 专属 `tests/integration/run/*` 迁到 `tests/verification/integration/pipeline/*`
- [ ] 将 `tests/integration/registry/*` 迁到 `tests/verification/integration/catalog/*`
- [ ] 将 subject discovery / projection / catalog 相关脚本迁到 `tests/verification/unit/catalog/*` 或 `tests/verification/integration/catalog/*`
- [ ] 将 inventory / benchmark projection 相关脚本迁到 `tests/verification/unit/projections/*`
- [ ] 删除纯 legacy 入口断言脚本
- [ ] 确认保留在旧目录的通用测试不再引用 old verification public path
- [ ] 增加“仓库中不存在旧 verification public path”的负向断言

### Phase 6: Acceptance

- [ ] formal archive 在新根下完整生成
- [ ] owner stubs 在新根下完整生成
- [ ] 总 `.sln` 和 workspace manifest 在新根下完整生成
- [ ] CLI / TUI / wiki 不再引用旧路径
- [ ] 路径扫描确认旧测试框架公共路径已清空
- [ ] 自动化测试通过

## 11. 验收标准

### 11.1 结构验收

必须满足：

- `verification/` 成为唯一正式验证根
- 所有 formal authority、owner evidence、workspace、projection 都可从新根恢复
- 旧路径不再出现在 public/mainline
- `subjects/` 不再承载测试体系 authority；若目录保留，也只能承载非测试主线内容

### 11.2 行为验收

必须满足：

- `run.py` 仍可完整驱动验证流程
- 总 `.sln` 可用于查看与调试
- projections 能从新 archive 正常生成

### 11.3 清理验收

必须满足：

- 仓库中不再存在旧测试框架公共路径引用
- wiki / skill / manifest / CLI / TUI 不再宣称旧路径
- 旧路径不是“废弃但保留”，而是“切掉并清理”
- `tests/` 下 Python 脚本不再断言旧路径，也不再依赖 legacy alias

## 12. 风险

- `hard cut` 需要一次性切换 generator、projection、workspace、wiki、tests，批次协调复杂度高
- 若 roadmap 中途把 `.sln` 重新做成静态手工文件，会重新引入漂移源
- 若为了方便保留任何旧路径 alias，会直接破坏本设计的“单一 authority 根”目标

## 13. 下游建议

本设计的下游必须是 `roadmap`，原因如下：

- 需要多阶段切换
- 需要跨代码、文档、skill、测试四条线同步推进
- 需要硬切与旧路径清理的明确收口检查

下游 roadmap 必须显式带上逐项 checklist，并以 checklist 完成度作为阶段进度主视图。
