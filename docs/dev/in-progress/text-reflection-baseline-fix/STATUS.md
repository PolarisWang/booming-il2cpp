# text-reflection-baseline-fix — STATUS

> **task_id**: text-reflection-baseline-fix
> **task_type**: plan
> **phase**: brainstorming → writing-plans
> **创建日期**: 2026-09-22
> **entry_skill**: dev-brainstorm
> **parent_task_id**: —
> **roadmap_or_plan**: —
> **关键文档**: 本文件
> **上游输入**: main 分支基线实测（2026-09-22 13:27）

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

---

## 边界拍板

**覆盖**：
- **text chunk** 33 项失败（29 真断言失败 + 4 异常逃逸）
- **reflection chunk** 23 项 failed + 8 realDefect（全异常逃逸族，31 preAssertionRaise）

**不覆盖**：threading（锁句柄族，独立根因）、json/xml（未实现族）、stubGap 类（未实现，非缺陷）。

## authority 决策

- 开**独立 worktree** 隔离本轮工作（主 checkout 有并发会话跑 system chunk）
- text 的 29 项按 **native stub 实现缺陷（值错）** 处理，走 systematic-debugging
- 修复顺序：**先 text，后 reflection**

## 当前结论（实测画像）

| chunk | failed | assertFailed | 纯逃逸 | 主要方法族 |
|---|---|---|---|---|
| text | 33 | 29 | 4 | Encoding/Rune/ASCII/UTF8/UTF32/UTF7 的 GetBytes/GetChars/GetMax*Count/GetPreamble/IsValid/MovePrevious |
| reflection | 23 | 0 | 23 | Get*/GetFields/GetMethods/TypeDelegator 查表族 |

- text 29 项：`value=0` 且 `assertFailed=True` → 断言执行了且不匹配 → 疑似 native stub 值错
- text 4 项 + reflection 23 项：`caught=true`、`assertFailed=false` + `SEH-FAULT 0xe0000001` → 异常逃逸（EH 错配）

## 下一步入口

开 worktree → text 诊断（systematic-debugging）→ 修复 → 验证 → reflection

---

## 阶段 2 拍板（2026-09-22）— text 口径修正 + 实现补齐

### 根因实证（推翻 Q1 初判）

Q1 初判「text 29 项 = native stub **实现错**（值错）」**被实测推翻**：

- `Ascii::IsValid` 等 29 项运行时**命中 catch-all**（stderr 实测：
  `catch-all helper invoked: ...::IsValid — no native body; delegates to Phase 1/2, else returns 0`）
- 即**根本没有 native 实现**，返回 0 → 断言 `0 != 1` 失败
- 但 fact 记录字段是 `bodyAvailability: "NativeGenerated"`

### 口径矛盾（问题根因）

| 信号 | 值 | 含义 |
|---|---|---|
| `bodyAvailability` | `NativeGenerated` | subject-id 出现在 aot-core-ir → **只证明 lowering 发生过** |
| 运行时实际 | catch-all → 返回 0 | **没有真实 native 实现** |
| 分类器判定 | `failed` | `_has_codegen_shape(manifest)==False` 落到最后分支 |

`build.py:408-410` 注释明示：`NativeGenerated` 的 DEFAULTS 依据是「subject-id 在
aot-core-ir 中」，**不代表有 canonical body**。而 `fact_chunk.py:314-353` 的分类链：
```
passed=False
  ├─ is_factory_subject && !assertFailed → factoryGap
  ├─ _has_codegen_shape(manifest)        → realDefect
  └─ 否则                                → failed    ← 本批落此
```
`failed` 的语义是「unclassified failure」，暗示本该实现；实际是**未实现**。

### 拍板：C2 + C1

**C2（先做）**：修正分类口径，让「无 shape 且运行时走 catch-all」的记录
**诚实归入未实现桶**（与 stubGap 同等待遇），而非 `failed`。
判据必须来自**运行时证据**（catch-all 命中），不能只看 manifest 缺失 ——
否则就是把红改绿（项目明令禁止的假绿）。

**C1（后做）**：按优先级补齐这批 native 实现 + shape 注册
（`Ascii::IsValid`、`Encoding::GetMaxByteCount/GetMaxCharCount/GetPreamble/GetEncodings`、
`Decoder|EncoderFallbackBuffer::MovePrevious/GetNextChar` 等 ~20+ 方法）。

### 顺序

1. C2 口径修正（小、可验证、先落地）
2. C1 按族分批补实现（每批独立验证）

### blocking_questions: []
### question_clearance: cleared
### clearance_confirmed_by_user: true
