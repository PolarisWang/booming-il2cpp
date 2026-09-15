# debug-20-real-defects — STATUS

> **task_id**: debug-20-real-defects
> **task_type**: plan
> **phase**: ready-to-start
> **创建日期**: 2026-09-15
> **entry_skill**: dev-il2cpp → dev-il2cpp-debug-expert
> **关键文档**: `handoff.md`

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

---

## 一句话

`System.Private.CoreLib/system` chunk 的能力清单判定出 **20 个 `realDefect`** —— 有 codegen shape（AOT 有实现）但执行时抛托管异常，断言从未执行。需要调试器级定位。

## 边界拍板

### 做什么
1. 定位这 20 个方法抛的具体异常类型和位置
2. 区分「AOT 实现缺陷」与「ATG 输入/断言问题」
3. 修复真正的 AOT 实现缺陷

### 不做什么
- 不改能力清单机制（已验证正确）
- 不改结构化匹配逻辑（已验证精确）
- 不碰其他 chunk

## 已知线索（详见 handoff.md）

| 线索 | 值 |
|------|-----|
| `caught` | **全部 20 个 = true** |
| `assertFailed` | **全部 20 个 = false** |
| AOT vs JIT | **完全一致**（非平台差异） |
| Phase 3 catch-all 触发 | **0 次**（异常不来自 ExternalRuntimeFallback） |
| 模式 | 多为 `_1`/`_2`/`_3` 变体（第 2/3/4 个值集） |

## 分类推测

| 簇 | 数量 | 推测方向 |
|----|------|---------|
| Math (Floor/Pow/Sin/Sqrt) | 4 | `BitConverter.DoubleToInt64Bits` 对 NaN 的未定义行为 |
| DateTime (AddHours/AddMinutes/Parse) | 6 | DateTime 结构体 ABI 封送 |
| Convert (ToBoolean/ToInt32/ToDecimal) | 3 | 边界字符串输入 |
| Decimal::Ceiling | 2 | `DecimalCarrier*` 空指针 |
| 其他 (BitConverter/Char/Enum/GC) | 5 | 各异 |

## 推荐调试入口

**方法 B（最直接）**：在 `NativeAotLoweringPlanner.MethodEmission.cs` 的 try/catch 中临时输出异常类型到 stderr，重跑 `--fact-json`。

## 前置条件

- entry.exe 已含 Phase B（NotImplementedException），时间戳 ≥ 2026-09-15 10:07
- 复现命令：
  ```bash
  cd /d/agent/chaos-il2cpp/tests/e2e
  CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/tests/e2e/translation \
    python -m verification.chunk_pipeline --chunk system --stages build,fact --smoke
  ```

## 验收标准

- 20 个 `realDefect` 全部归因（AOT 缺陷 / ATG 问题）
- 真正的 AOT 缺陷已修复或明确记录为 not-supported
