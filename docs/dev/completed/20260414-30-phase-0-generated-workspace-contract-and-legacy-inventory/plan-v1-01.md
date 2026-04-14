# Phase 0 Plan v1.01

Date: 2026-04-14 16:34:34 +08:00
Status: in-progress

## 1. 目标

冻结 `Generated Workspace Model v2` 的 Phase 0 硬边界，作为后续 Phase 1-5 的唯一 contract 基线：

- 固定 `workspace.manifest.json` v2 的字段集合与职责边界
- 固定 generated managed host / native test host 的输入输出 contract
- 给出 legacy cleanup inventory，明确哪些旧入口、旧字段、旧测试壳必须在后续阶段被替换并最终删除
- 给出迁移顺序，避免后续 child task 边做边改 contract

## 2. 输入文档

- parent design:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent roadmap:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`

## 3. 执行 checklist

### 3.1 Contract freeze

- 审计当前 `project_workspace.py`、`commands/test.py`、`testing/compiled_catalog.py`、`testing/registry.py`、`testing/subjects.py`
- 冻结 workspace manifest v2：
  - `managedProjects`
  - `managedTestProjects`
  - `nativeProjects`
  - `nativeTestProjects`
  - `matrices`
  - 需要保留的最小兼容读取策略
- 冻结 generated host contract：
  - declared catalog 必备字段
  - `entryIndex` 数值调度约束
  - managed host 与 native host 的职责切分

### 3.2 Legacy inventory

- 列出手写 proof / benchmark 注册路径
- 列出旧 native proof / generated native target 字段与路径
- 列出 string source-entry / selector / compat surface
- 列出 tests 中直接耦合旧 subject/phase 布局的用例与 support 逻辑
- 列出 subject 内仍保留手写 host / `Program.cs` / `Console.WriteLine` 断言壳的路径

### 3.3 输出文档

- `contract-freeze-v1-01.md`
- `legacy-cleanup-inventory-v1-01.md`
- `migration-order-v1-01.md`

## 4. 验收标准

- 输出文档能直接作为 Phase 1-5 的引用基线
- inventory 至少覆盖 parent roadmap Phase 0 定义的五类 legacy surface
- 文档中每个 cleanup item 都给出文件级锚点与建议删除阶段
- 不引入新的实现代码，只冻结 contract 和清理边界

## 5. 非目标

- 本阶段不修改 runtime / pipeline 实现
- 本阶段不开始 workspace v2 代码生成
- 本阶段不启动 benchmark runner/dashboard 修复
