# T-A6 — Cross-test 全域态污染检测

## Scope

调查并修复多 case binary 间共享 G_OldGen/G_YoungGen/G_Scheduler singleton 导致的跨 case 污染。

## 现状调查

- **已知污染**：`test_gc_gen1`（14 个 TEST，2 个 singleton 引用）已在 known-failures 注册
- **潜在污染候选**：`gc_heap_limit_test`（8 TEST，21 singleton 引用 ⚠️）、`gc_max_promote_test`（4 TEST，1 singleton 引用）
- `GcTestBase::TearDown` 已做 `snapshot_.ExpectNoLeaks` 资源泄漏检测，但**不强制 singleton 归零**
- **已知的 GcResourceSnapshot 不覆盖**：region-count delta 是 logged not failed，gen 状态不重置

## 决策

**建议延后至 T-A6 被实际需要时再实施**。理由：
1. `test_gc_gen1` 已在 known-failures 中注册，不会静默失败
2. 修复方案（在 `GcTestBase::TearDown` 中加 `GcResetSingletons()`）是**测试框架基础设施改动**，可能影响其他 binary 的稳定性
3. T-A6 不阻塞 P1 gate（G-P1 的 `stress 0/1000` 门禁不依赖 gen1 测试单绿）

## 延期后的 Exit Criteria

- 移除 `roadmap-v1-01.md` 中 T-A6 的依赖（不影响 G-P1 gate）
- 转至 `watch_items`：`test_gc_gen1` known-fail 持续注册，待 P2 阶段修复

## 元信息

- `status: deferred`（不阻塞 P1 门禁）
- `deferred_to: P2 (残余并发 bug 修复阶段)`