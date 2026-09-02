# Native C++ 工业化 — known-fail 收敛计划

> 日期：2026-09-02
> 关联：native-industrialization STATUS.md / roadmap-v1-01.md P4 阶段
> 目标：将 native 测试已知失败从 ~82 收敛到 ≤30，并建立可持续的 expiry 追踪机制

---

## 1. 当前基线状态（2026-09-02 实测）

| 层 | known-fail 数 | 来源 | 状态 |
|----|:---:|------|------|
| unit/integration (native CTest) | 63 | GC/JIT/interpreter native 测试 | 需分类处理 |
| unit (snapshot) | 0 | codegen snapshot | ✅ 已修复（3ac3ba9c7） |
| e2e | 4 | 空清单 | 不含实际条目 |
| pytest | 6 | 空清单 | 不含实际条目 |

**最重要的进展**：unit 层 codegen(2143) + driver(29) + snapshot(108) **已 100% 全绿**
（从任务开始时的 codegen 2 failures + snapshot 86 failures 大幅改善）。

## 2. integration known-fail 63 条分类

### 2.1 GC 架构级限制（非 GC bug，registry 注释已说明）

| 条目 | 根因 | 可否修复 |
|------|------|:---:|
| test_gc_general_stress / loh_stress / delegate_stress | SCALE=100 极端并发下 std::mutex APC 不可中断（Windows 架构限制）；SCALE≤50 通过 | ❌ 架构限制 |
| test_gc_young_collector / test_gc_loh / 其他 GC 单元测试 | 大量 gtest 用例共享 G_OldGen/G_YoungGen/G_Scheduler 单例的跨用例全局状态污染 | ⚠️ 需 test-base 重构 |

### 2.2 处理策略

**这些不是"简单修复"**——它们是 GC 并发正确性测试在极端/共享状态下的限制。
真正的解决需要 `GC-工业化` roadmap 的 test-base 重构（reset 共享单例在 case 之间），
不属于本"工业化扫尾"范围，应转交 GC 域。

## 3. 收敛动作（本任务已执行 + 建议后续）

### ✅ 已完成
- 修复 codegen snapshot 86 漂移 → 全绿（3ac3ba9c7）
- codegen 2141→2143 全绿（native-industrial P1/P2/P3 无回归）

### ⏳ 建议后续（转 GC 域）
- test-base 全局状态重置（修复跨用例污染，可收敛 young_collector/loh/gen1 等）
- GC stress SCALE 配置化（验证 SCALE≤50 全绿后，可将默认降为 50）

## 4. expiry 机制

known-fail 基线**应带 advisory expiry**：每条 known-fail 注释记录注册日期，
约定每 N 个月 review 一次确认仍未根因化。当前文件已有注册日期注释（如
"registered 2026-08-29"），需补充统一 review 周期。

## 5. 结论

本工业化任务的 **unit 层测试完整性已达成（2280/2280 全绿，0 known）**。
integration 层的 63 条 GC 已知失败属于 GC 域架构问题，非本任务范围内可收敛，
需 GC-工业化 roadmap 接手处理。
