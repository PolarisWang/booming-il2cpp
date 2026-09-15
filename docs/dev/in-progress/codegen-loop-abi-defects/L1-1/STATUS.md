# L1-1 — EmitIRWhileLoop 条件循环内重算（方案 B）

```yaml
task_id: L1-1
task_type: plan
phase: implementation
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: completed
```

## Scope

`EmitIRWhileLoop` 两个分支按方案 B 实现条件循环内重算。

## Exit Criteria

- 生成物为 `while (cond) { …; <cond> }` —— ✅
- codegen 2252/2252 绿 —— ✅
- 反例实机执行 —— ✅ **已完成**（见下）

---

## ✅ L1-2 反例取证（已完成）

**做法**：把 `condBlock` 强制置空（禁用重算），重建 TPG 并直跑生成物核对。

```csharp
string condBlock = string.Empty;  // COUNTER-EVIDENCE
```

**实测对比**（同方法 `AreEqual(byte[])`，`while` 区间内计数）：

| 条件 | 循环体内 `i++` 写入 | 循环体内**条件重算** |
|---|---|---|
| 修后（正常） | 1 | **1** |
| 反例（禁用） | 1 | **0** |

⇒ 反例精确命中：禁用后条件重算从循环体内**消失**，与预期一致。
`i++` 保持 1 说明该计数不受此改动影响（对照组成立）。

**回退校验**：`git diff --quiet` 对 `StructuredIR.Emit.cs` → **CLEAN**
（与提交 `42915b6b4` 逐字节一致）。

---

## ✅ 已完成（commit `42915b6b4`）

**实现**：条件指令在循环体**末尾**重新发出，保留 `while (cond)` 形式（方案 B）。
两个分支（`brtrue/brfalse` 路径、比较操作码路径）都改。

**纯性门控**：`WhileConditionHasSideEffects` 对 call/stfld/stelem/stind/throw 等
保守返回 true；此类循环不做重算（宁可陈旧测试，不产错码）。
本方法的条件为 `ldloc/ldarg/ldlen/conv.i4/clt/stloc/ldloc` → 无副作用 → 重算。

**生成物核对**（`/tmp/tpgSP`，直接跑 TPG 的新鲜产物）：
函数体 2396–3705 行，`while` 在 2912–3328：
- 循环体内 `chaos_locals[4] = ` **1 次**（`i++`）
- 循环体内 `<` 比较 **1 次**（条件重算）
- 此前两者都在 `while` **之外**

## ⚠️ 未做反例

本轮**未做 in-place revert 反证**（时间/上下文限制）。
生成物形态是可直接核对的结构性证据，但按项目纪律（决策 2=A）
反例应当补做：把 `condBlock` 的 splice 注释掉 → 循环体内应重新失去条件重算。

**遗留项**：登记为 L1-2 未完成部分。

## 关键教训（本轮踩到）

**读了陈旧生成物导致误判 L1 未生效**。artifact 在 11:39、我的 TPG 直跑在 11:42，
我读的是前者 → 得出"改动没生效"的错误结论，多花数轮。
**读生成物前必须确认构建新鲜**（roadmap 陷阱 4）。
另：本方法**跨 1309 行**，只看函数头附近会漏掉循环体末尾的改动。

## Terminal Notes

- 提交：`42915b6b4`（与 A1-2 合并提交）
- 提交前工作区另有两个并发 agent 的文件（`MethodEmission.cs`、`S19.cs`），**未纳入**
