# Layout Boundary Freeze 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 冻结 Option C 强规范化重组的终态目录结构、ownership 分类、命名规则、文件拆分策略与 manifest 分片原则，为后续所有 child task 提供唯一参考基线。

**架构：** 本阶段不涉及代码变更，只产出三份规范性文档作为后续 Phase 1-6 的约束基线。所有文档写入当前任务目录。

**技术栈：** 无（纯文档）

**设计文档：** `docs/dev/in-progress/20260409-11-contract-root-without-subject-content/design-v1-02.md`

**预期知识沉淀：** `wiki/` 下按需决定

---

## 交付物

1. `layout-target-v1.md` — 终态目录树、ownership 分类、命名规则
2. `path-mapping-v1.md` — 当前路径 → 终态路径的完整映射表
3. `file-split-policy-v1.md` — 文件拆分策略与软性行数预算

## 步骤

### Batch 1: 终态目录结构文档

- [ ] **1.1** 编写 `layout-target-v1.md`，包含以下内容：
  - 终态 `contracts/` 目录树（只保留 formal definition：schemas、docs、native headers）
  - 终态 `tests/contracts/` 目录树（承载 analysis contract fixtures、native compile smoke、trace 验证）
  - 终态 `build/toolchains/run/` 目录树（按 design-v1-02 Option C 的 domain-based layout）
  - 终态 `src/managed/Chaos.IL2CPP.CodeGen/` 目录树
  - 终态 `src/managed/Chaos.IL2CPP.Loader/` 目录树
  - 终态 `tests/tooling/run/` 目录树
  - 终态 `tests/unit/run/` 目录树
  - 三类 ownership 定义：
    - **formal definition**：contract schemas、native headers、contract docs
    - **shared validation fixture**：analysis samples、snapshots、native compile smoke、trace snapshots
    - **subject-owned content**：subject source、validation、expected、baselines
  - 命名规则冻结：
    - 目录命名使用 `kebab-case`
    - Python 文件命名使用 `snake_case`
    - C# 文件命名使用 `PascalCase`
    - JSON manifest 命名使用 `kebab-case`
    - 测试文件命名统一前缀 `test_`

### Batch 2: 路径映射表

- [ ] **2.1** 编写 `path-mapping-v1.md`，包含以下映射：
  - `contracts/artifacts/v0/samples/**` → `tests/contracts/analysis/v0/samples/**`
  - `contracts/artifacts/v0/snapshots/**` → `tests/contracts/analysis/v0/snapshots/**`
  - `contracts/native/v0/samples/**` → `tests/contracts/native/v0/samples/**`
  - `contracts/examples/v0/**` → 删除（已被 samples 取代）
  - `contracts/native/examples/v0/**` → 删除（已被 native samples 取代）
  - `tests/contract/**` → 删除（legacy 别名，已被 `tests/contracts/**` 取代）
  - `build/toolchains/run/` 旧模块 → 新 domain-based 模块映射
  - `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs` → 拆分到 `Lowering/` 子目录
  - `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs` → 拆分到 `Metadata/`、`Decode/`、`Materialization/` 子目录
  - `tests/tooling/run/` 平铺测试 → 按子系统分目录
  - `tests/unit/run/` 平铺测试 → 按子系统分目录
  - 对每条映射标注影响的 consumer（path_resolver、run_manifest、test discovery、docs 等）

### Batch 3: 文件拆分策略

- [ ] **3.1** 编写 `file-split-policy-v1.md`，包含以下内容：
  - 软性行数预算：
    - Python 脚本：≤ 500 行（超过需要拆分审查）
    - C# 源码：≤ 600 行（超过需要拆分审查）
    - JSON manifest：≤ 300 行（超过需要目录化分片）
  - 拆分原则：
    - facade 保留在原文件，具体实现下沉到子模块/子目录
    - 按职责边界拆分，不按行数机械切割
    - 拆分后每个文件应有单一清晰职责
  - manifest 分片策略：
    - `run_manifest.json` 按 command group 分片为多个 JSON 文件
    - 分片目录 `build/toolchains/run/manifests/run/`
    - 保留组合加载入口，运行时合并所有分片

### Batch 4: 验证与交付

- [ ] **4.1** 检查三份文档内部一致性（path mapping 的源和目标与 layout-target 对应，file split policy 的阈值与实际热点文件吻合）
- [ ] **4.2** 更新当前任务 STATUS.md 标记完成
- [ ] **4.3** 更新父任务 roadmap STATUS.md，标记 `layout-boundary-freeze` 为 `done`
