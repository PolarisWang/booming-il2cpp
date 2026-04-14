# Phase 2 Plan v1.01

Date: 2026-04-14 17:04:51 +08:00
Status: completed

## 1. 目标

把 Phase 1 的 declared catalog + generated managed host 接到 `project_workspace.py` 的工程生成主链里，形成 workspace manifest v2 与四类工程的一等模型。

## 2. 输入文档

- parent design:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent roadmap:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- phase-0 contract freeze:
  - `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- phase-1 completion:
  - `docs/dev/completed/20260414-31-phase-1-declared-catalog-to-managed-test-host/STATUS.md`

## 3. TDD checklist

### 3.1 RED

- 为 workspace manifest v2 的 `managedProjects` / `managedTestProjects` / `nativeProjects` / `nativeTestProjects` 增加失败测试
- 为 `project_workspace.py` 生成 managed proof host / benchmark host 工程增加失败测试
- 为 native test project 必须消费 generated managed host 而不是第二套手写注册表增加失败测试
- 为旧 proof-native target 不再作为新主输出增加失败测试

### 3.2 GREEN

- 扩展 workspace 生成模型，写出 manifest v2 一等字段
- 生成 managed test projects，并把 generated host/catalog 一起落到 workspace
- 生成 per-matrix native projects / native test projects
- 让 native test host 以 translated managed host 为输入，而不是重建声明发现逻辑

### 3.3 REFACTOR

- 收口 project descriptor / manifest writer 的重复结构
- 清理 `project_workspace.py` 中只服务旧 proof target 的分支
- 保持旧字段只读兼容，不再继续扩面

## 4. 验收标准

- `run generate project subject` 产物中可见四类工程模型
- `workspace.manifest.json` v2 写出 `managedProjects` / `managedTestProjects` / `nativeProjects` / `nativeTestProjects` / `matrices`
- native test project 不再维护第二套 proof/benchmark 清单
- 新实现不再扩写 legacy manifest 字段或新的字符串调度面

## 5. 非目标

- 本阶段不切换 `run test` 公共入口
- 本阶段不做 benchmark dashboard 改版
- 本阶段不删除 retained subject legacy host

## 6. 完成摘要

- workspace manifest v2 已成为 subject workspace 的主写契约。
- managed proof/benchmark host 已生成并纳入 workspace / solution。
- native test project 已与 generated managed host 对齐。
- Phase 3 将继续处理 `run test` / registry / selector 的主链切换。
