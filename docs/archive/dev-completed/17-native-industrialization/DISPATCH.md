# DISPATCH.md — Native C++ 工业化品质提升 调度总纲

## 调度模型

**`dispatch_model: hybrid`**

- **batch-1**: 逻辑并行（T-A + T-B 两轨互不依赖）；受"单终端"限制，物理上以单终端交替切换串行推进（非真多线程并行）
- **batch-2 起**: 串行（P1 → P2 → P3 → P4，强依赖链，每阶段完成必须测试+review 后才能进入下一阶段）

## 并行能力

- 当前可用终端数：1（单人执行）
- batch-1 mode=parallel 的语义：T-A / T-B 两轨**逻辑互不依赖、可任意排序/交错**，仅受单终端物理限制而**顺序交替执行**（上午 T-A / 下午 T-B）；不代表多终端真并行。排期估算须按总工时=Σ各任务顺序和计，勿按墙钟并行预计。

- batch-1 输出注入 batch-2 P1 的**隐性依赖**（需 batch-1 对应产物产出并 review 后才能启动 P1，不可仅等整组 batch-1 all-completed 记录）：
  - **T-B4 → P1**：T-B4 `alloc-audit.md`（裸 new/reinterpret_cast 清单，含 count 与逐处结论）是 P1 "176 处裸 new 改宏 + reinterpret_cast 审查"的**输入**。无该清单即无从逐处审查落地。
  - **T-B3 → P1**：T-B3 `error-model-design.md`（错误类型统一方案）是 P1 "错误类型落地"的**依据**。方案未定型前 P1 无落地目标。

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
| P1 | sequential | 裸 new 改宏(基线:176处, 来源:alloc-audit.md L19 2026-05-21统计 覆盖 src/native/runtime-core/ + src/native/common/ + src/native/chaos-metadata/ 三域; 审计后已修正4处泄漏, 合约头新增 kNew/malloc 宏; **注意**: 176是静态快照, 非增量基准(4处泄漏修正已含在176内, 176与172的分歧仅在审计后若按修正量扣减会得到172, 但不需要追踪——验收以"全部替换完毕"而非"对齐176"为准, 最终计数应当为0) + reinterpret_cast 审查 + 错误类型落地 | batch-1 且 T-B3/T-B4 产物已产出+review ✅ |

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