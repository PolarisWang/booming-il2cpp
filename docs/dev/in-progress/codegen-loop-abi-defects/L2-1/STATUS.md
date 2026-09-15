# L2-1 — 取证明确定位 latch 外置

```yaml
task_id: L2-1
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

定位 `i++`（latch）被发在循环外的切分点。

## 结论：**L2 不复存在 —— 已被 L1 的修复一并解决**

### 取证（fresh 生成物，commit `b3ebf5139` 之后）

循环区间 `while@2912 — closes@3328`，区间内实测：

| 计数 | 值 |
|---|---|
| `chaos_locals[4] = `（`i++`） | **1** |
| `<` 比较（条件重算） | **1** |

循环尾部实样：

```cpp
    chaos_locals[4] = _s2;          // ← i++
    }
    _s6 = chaos_locals[4];          // ┐
    _s7 = chaos_args[0];            │ 条件重算
    _s7 = [... length ...];         │
    _s6 = (_s6 < _s7) ? 1 : 0;      // ┘
}
```

与 IL 形态逐条对应：
```
IL_0072: <body> ; IL_00d5: i++ ; IL_00d9: i < len ; brtrue IL_0072
```

### 为什么 L2 会消失

当初判定 L2 存在，是在**只有 L1 的部分修复**时观察到的：
条件指令发在 `while` 之前、`i++` 也在 `while` 之前。

L1 的方案 B 修复把**条件指令**移入循环体尾部；而 `i++` 本就在 `IRWhileLoop.Body`
的范围里（`bodyEnd = headerIndex - 1` 含 latch 块），此前只是因为**条件块被当作
循环体的一部分**而一起排在了 `while` 之前。

⇒ 条件归位后，`i++` 自然落回体内。**两者是同一个切分问题的两个面，不是两个缺陷。**

### 对 roadmap 的修正

原计划 P2 的 exit_criteria 是「latch 落在循环体内；si=54/56 转绿」。
**前半达成**（已核实）；**后半未达成** —— si=54/56 仍是 `caught=true`，
但原因已查明**不是** latch 外置（见下）。

## si=54/56 现状（转 P5 / 另行定位）

`[ABORT-FAULT]`（SIGABRT），非断言失败，stderr 无 `[ASSERT FAIL]`。
已排除：
- 循环结构（现与 IL 一致）
- `stloc` 源（A1 已修）
- `Assert.AreEqual(byte[])` 的条件与槽位（L1 + 槽位修复已核实）

候选方向（未验证）：
- 这 6 个 API 走 **catch-all 零参数兜底**（`this` 未传），可能与 abort 相关
- `ChaosArrayEmpty_Inline()` 的 GC 分配路径

## Terminal Notes

- **L2-2（修复）无对象** —— 同 L2-1 一并关闭
- L2 的教训：**两个"缺陷"其实是同一根因的两个观察面**。
  当时的判断（分列 L1/L2）是在只修了一半时的中间态观察，非最终形态。
