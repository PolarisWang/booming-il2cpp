# KNOWN_ISSUES.md — 已知固有限制

> 记录在 Phase 0 Pipeline 预检期间识别的已知限制，这些限制不阻塞当前验证流程但需在后续阶段解决。

## 1. Codegen 构建失败 — `chaos_*` 宏未定义

**影响范围**：全量 family
**阶段**：Stage 1 — Codegen (AOT)

**现象**：`codegen/*Subjects/generated/native-aot.generated.cpp` 使用以下标识符，但编译时未定义：
- `chaos_array`
- `chaos_value`
- `chaos_managed_pointer_local_slot_tag`
- `chaos_is_type_compatible`

**根因**：codegen 层新发射的宏名称与 runtime 头文件提供的定义不匹配。这是一个 codegen ↔ runtime 头文件的接口脱节。

**影响评估**：
- Fact AOT/JIT 验证不受影响（使用 pre-built entry-aot.exe ✅）
- Audit 不受影响 ✅
- Microbench 不受影响 ✅
- HotUpdate 各阶段不受影响 ✅
- AsmCompare 和 Benchmark 的部分流程受影响（需要 codegen 产物）

**建议修复阶段**：Phase 2（Family Storm）或 Phase 1.5（Risk Ranking）

## 2. Benchmark stub 检测 — 所有方法被标记为 stub

**影响范围**：部分 family
**阶段**：Stage 8 — Benchmark

**现象**：benchmark 报告 "All methods are stubs" 或 "All non-stub methods are invalid (no managed harness)"

**根因**：部分 family 没有 managed benchmark harness，benchmark 使用 fallback 路径

**影响评估**：功能性验证正常，benchmark 数据不可用于性能回归分析

**建议修复阶段**：Phase 2（Family Storm）— 按风险优先级逐个 family 补充 benchmark

## 3. p1_benchmark 固有 VIOLATION

**影响范围**：涉及 GC 分配的 stub 路径
**阶段**：Stage 5 — Audit (principle audit)

**说明**：部分 p1_benchmark 方法因 stub 路径包含 GC 分配而产生固有 VIOLATION（managed JIT 使用 stack allocation，il2cpp stub 路径必须通过 GC heap）。这是 stub-based 架构的固有限制。

**记录**：参见 `wiki/06-测试验证/ENUM_GUID_BENCHMARK_LIMIT.md`
