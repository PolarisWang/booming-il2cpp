# Crypto Chunk Fact 失败 — Subject Slot 映射不完整

## 问题展开

### 数据流

```
ATG probe → subjects.metadata.json (1307 methods)
                │
CodeGen → AotCoreIr lowering (1557 methods, 含 non-subject 调用)
                │
Planner → BuildSubjectSlotMap → kSubjectSlotMap[1222]
                │            85 methods MISSING
TPG → verification_dispatch.generated.cpp (只 dispatch kSubjectSlotMap 中的 1222)
                │
entry.exe --fact-json → factResults: 0 passed, 0 total ("skipped")
                │
fact_chunk.py → 1307 (ATG) vs 0 (dispatched) → 100% dropped → FAILED
```

### 根因分析

1. **Codegen 的 `BuildSubjectSlotMap()`** 只包含在 AotCoreIr lowering 中有对应 method body 的 subject
2. Crypto 方法调用 BCrypt/NCrypt P/Invoke → 部分方法无法生成完整的 AOT method body
3. 没有 AOT body 的 subject → 不在 slot map 中 → `kSubjectSlotMap[i] = -1`
4. verification dispatch 遍历 slot map，遇到 -1 跳过 → 对应 subject 未执行

### 影响面

- 当前 `kSubjectEntryCount = 1222`（codegen），`meta_total = 1307`（ATG）
- 缺口：85 methods（6.5%）
- 类似问题影响所有含 P/Invoke 的 assembly（System.Security.Cryptography, System.Net.Http 等）

## 方案

### 方案 A：Codegen 补齐 Subject stub body

在 AotCoreIr lowering 阶段，对于无法生成完整 AOT body 的 subject，生成一个最小 stub body（只调用 `ChaosExternalRuntimeFallback(subject_id)` 返回 sentinel）。

**优点**：所有 subject 都有 slot entry，fact 能跑满 100%
**缺点**：stub body 返回默认值（0/nullptr），assert 可能误报失败；需要确定哪些 subject 无 body

### 方案 B：Interpreter fallback for missing subjects

修改 verification dispatch：当 `kSubjectSlotMap[i] == -1` 时，不走 AOT dispatch table，改为通过 InterpreterEntryDirect 直接解释执行。

**优点**：不需要修改 codegen；利用已有 interpreter 路径
**缺点**：解释执行比 AOT 慢 10-100x；需要为 crypto subject 提供 IL body（当前 AOT codegen 不保留 IL）

### 方案 C：Subject skip-list + 降低阈值

接受 crypto subject 无法全量覆盖的事实，为 crypto chunk 建立已知 missing list，将 fact 通过阈值从 100% 降到合理值（当前 STATUS.md 显示 88%）。

**优点**：0 代码改动
**缺点**：不是修复，是接受限制；fact 覆盖率无法提升

### 方案 D：提前过滤 — ATG 只生成可 AOT 的方法

在 ATG probe 阶段，检查方法是否有 AOT lowering 路径。只对可 AOT 的方法生成 subject，跳过 P/Invoke-only 方法。

**优点**：slot map 与 metadata 天然对齐
**缺点**：减少 subject 覆盖范围；需要 ATG 感知 AOT lowering 能力

## 推荐

**Phase 1：方案 A（最小 stub）** — 快速提升覆盖率到接近 100%
- 改动：`NativeAotLoweringPlanner.BuildSubjectSlotMap()` 生成默认 body 替代 -1
- 风险：低（只影响 fact 验证，不影响生产 AOT）
- 工作量：~1 天

**Phase 2：方案 B（interpreter fallback）** — 长期方案
- 改动：verification dispatch + codegen 保留 IL body
- 优势：完整覆盖，解释执行保证正确性
- 工作量：~3-5 天

建议先做 Phase 1，把 crypto fact 覆盖率从 0% 提到接近 100%，然后再评估是否需要 Phase 2。
