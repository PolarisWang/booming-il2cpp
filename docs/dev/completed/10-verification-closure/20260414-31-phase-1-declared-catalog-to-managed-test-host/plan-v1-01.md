# Phase 1 Plan v1.01

Date: 2026-04-14 16:49:05 +08:00
Status: in-progress

## 1. 目标

把 declared metadata 从“可发现”推进到“可生成 managed proof host / benchmark host”。

## 2. 输入文档

- parent design:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent roadmap:
  - `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- phase-0 contract freeze:
  - `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- phase-0 legacy inventory:
  - `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/legacy-cleanup-inventory-v1-01.md`

## 3. TDD checklist

### 3.1 RED

- 为 declared catalog `entryIndex` 增加失败测试
- 为 generated managed proof host 增加失败测试
- 为 generated managed benchmark host 增加失败测试
- 为 generated host 在无 attribute / framework 缺失 / catalog 不完整时的错误契约增加失败测试

### 3.2 GREEN

- 扩展 catalog 输出模型
- 生成 managed host `.g.cs` 与 `.csproj`
- 以 `entryIndex` 生成 `stableId -> entryIndex` / `entryIndex -> invoke`
- 接上最小 host generation API

### 3.3 REFACTOR

- 收口 catalog / host generator 公共模型
- 避免复制 benchmark/proof 生成模板逻辑
- 不引入新的 string dispatch surface

## 4. 验收标准

- 任一 retained subject 的 declared unit test / benchmark 都可以生成 managed host
- host 调度主键是 `entryIndex`
- 新增实现不依赖手写 `Dictionary<string, Func<int>>` 或 `sourceEntry` 主分发

## 5. 非目标

- 本阶段不生成 native test project
- 本阶段不切换 `run test` 公共入口
- 本阶段不删除 retained subject 旧 host
