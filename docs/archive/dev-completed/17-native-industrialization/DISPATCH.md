# DISPATCH.md — Native C++ 工业化品质提升 调度总纲

## 调度模型

**`dispatch_model: hybrid`**

- **batch-1**: 并行（T-A + T-B，两轨互不依赖）
- **batch-2 起**: 串行（P1 → P2 → P3 → P4，强依赖链，每阶段完成必须测试+review 后才能进入下一阶段）

## 并行能力

- 当前可用终端数：1（单人执行）
- batch-1 并行策略：单终端交替切换（上午 T-A / 下午 T-B）

## 执行纪律

```
各阶段必须：
  1. 实现 → 2. 测试（python tests/runner/test_driver.py --layer unit）→ 3. review 风险 → 4. 用户确认
  5. 只有全部通过才进入下一阶段
```

## Batches

### batch-1: T-A 工具链 + T-B 架构设计（并行组）

| task_id | mode | description | conflict_check | wait_for |
|---------|------|-------------|:--------------:|:--------:|
| T-A1 | parallel | clang-tidy 集成 + CI 强制 | custom | - |
| T-A2 | parallel | clang-format CI 强制 | custom | - |
| T-A3 | parallel | 覆盖率硬门禁 | custom | - |
| T-A4 | parallel | UBSan CI 集成 | custom | - |
| T-A5 | parallel | 模糊测试扩展 | custom | - |
| T-A6 | parallel | 性能回归硬门禁 | custom | - |
| T-B1 | parallel | 大文件拆分方案 | docs/ | - |
| T-B2 | parallel | 循环依赖消除方案 | docs/ | - |
| T-B3 | parallel | 错误类型统一方案 | docs/ | - |
| T-B4 | parallel | 裸 new / reinterpret_cast 审计清单 | docs/ | - |
| T-B5 | parallel | Doxygen 方案 | docs/ | - |

**merge_after**: all-completed
**merge_gate**: 用户 review 确认

### batch-2: P1 编码规范审计（串行组）

| task_id | mode | description | wait_for |
|---------|------|-------------|:--------:|
| P1 | sequential | 176 处裸 new 改宏 + reinterpret_cast 审查 + 错误类型落地 | batch-1 |

### batch-3: P2 模块化重构（串行组）

| task_id | mode | description | wait_for |
|---------|------|-------------|:--------:|
| P2 | sequential | 超大文件拆分 + 循环依赖消除 + runtime-core 拆子系统 | P1 |

### batch-4: P3 API 文档体系（串行组）

| task_id | mode | description | wait_for |
|---------|------|-------------|:--------:|
| P3 | sequential | Doxygen + 核心头注释 + CI 发布 | P2 |

### batch-5: P4 单测覆盖补齐（串行组）

| task_id | mode | description | wait_for |
|---------|------|-------------|:--------:|
| P4 | sequential | 核心覆盖率 ≥70% + known-fail ≤30 | P3 |