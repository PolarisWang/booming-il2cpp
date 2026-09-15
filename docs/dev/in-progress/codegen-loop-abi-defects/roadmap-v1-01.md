# codegen-loop-abi-defects — Roadmap v1

> **task_id**: codegen-loop-abi-defects
> **task_type**: roadmap
> **创建日期**: 2026-09-15
> **entry_skill**: dev-brainstorm → dev-roadmap
> **dispatch_model**: **sequential**（子任务强依赖、共享同一批 codegen 文件）
> **上游输入**: `docs/dev/in-progress/reflection-production-readiness/FAILURE-TRIAGE-2026-09-14.md`

---

## 1. 目标

修掉 2026-09-14 反射 chunk 失败归因挖出的两个 **codegen 产物正确性**缺陷族：

- **循环族**：C# `for`（body-first/condition-last IL 形态）的 lowering 不正确
- **ABI 族**：托管指针经 `CHAOS_IL2CPP_INT64` 返回槽漏出 / `return 0` 桩

最终判据：反射 chunk 的 `caught` 计数显著下降，且 `Assert.AreEqual(byte[])`
在两空数组上不再 `__fastfail`。

---

## 2. 范围边界

| 族 | 子任务 | 层 | 触及文件 |
|---|---|---|---|
| 循环 | L1 `IRWhileLoop` 条件循环内重算 | emitter | `NativeAotLoweringPlanner.StructuredIR.Emit.cs` |
| 循环 | L1-v 跨 chunk 回归验证 | 验证 | 无（只跑管线） |
| 循环 | L2 循环体块切分/排序（latch 外置） | 恢复层 | `NativeAotLoweringPlanner.StructuredControlFlow*.cs` |
| ABI | A1 标记指针经 INT64 返回槽漏出 | codegen/ABI | 待定位（ATG 或 codegen） |
| ABI | A2 `return 0` 桩 native 语义 | native + codegen | `remaining_stubs.cpp`、`reflection_query_model.h`、registry |

**全局优先级约束（阶段切分前评估）**：

- L1 选**方案 B**（`while(cond)` + 条件末尾重算）而非方案 A（`while(true)+break`）：
  P1 性能优先 —— 保持自然循环形式，不引入每轮额外分支。**无违反优先级顺序的裁决**。
- L1-v 的跨 chunk 回归是 P2（方案完美性）向 P1（性能）让位后的必要补偿：
  改动落在共享热路径，必须先量后改。

---

## 3. 非目标

- `IRDoWhileLoop` 的条件语义（本 roadmap 只动 `IRWhileLoop`；若同类问题存在另立）
- 反射 API 覆盖面补齐（属 `reflection-production-readiness`）
- 漏注册清扫（主体已在 `ab75ea1fd` 完成）
- HotUpdate 路径（不改运行时契约）
- `IRSwitch` / 异常分区的循环交互（不在本次发现的缺陷面内）

---

## 4. 阶段列表

| Phase | goal | 并行性 |
|---|---|---|
| **P0** | L1 取证复核 + 方案 B 实现 | 串行 |
| **P1** | L1-v 跨 chunk 回归 | 串行（依赖 P0 产物） |
| **P2** | L2 取证明确定位 + 修复 | 串行 |
| **P3** | A1 根因定位 + 修复 | 串行（代码文件与 P0–P2 重叠，不并行） |
| **P4** | A2 descriptor 扩展 + 实现 | 串行 |
| **P5** | 收口 | 串行 |

---

## 5. 每阶段完成定义

### P0 — L1 实现

- **goal**：`EmitIRWhileLoop` 两个分支按方案 B 实现条件循环内重算
- **exit_criteria**：生成物为 `while(cond){…; <cond>}`；codegen 2252/2252 绿；反例实机执行并记录
- **deliverables**：`StructuredIR.Emit.cs` 改动 + 反例记录（commit message）
- **dependencies**：无
- **resolved_decisions**：方案 B（用户 Q1）；不用方案 A
- **watch_items**：条件指令若含**非纯指令**则副作用风险成立 → 该循环回退方案 A

### P1 — L1-v 跨 chunk 回归

- **goal**：确认改动未破坏其它 chunk 的 while 循环
- **exit_criteria**：4 chunk（reflection/system/text/threading）build 全绿；全量 16 处 while 逐处核对形态
- **deliverables**：回归记录（各 chunk fact.json + 生成物核对）
- **dependencies**：P0
- **resolved_decisions**：回归范围 B2（用户 Q2）
- **watch_items**：若某 chunk 出现新红，需区分「本改动引入」vs「预存在」（全量跑两态对比）

### P2 — L2 定位 + 修复

- **goal**：latch（`i++`）落回循环体内
- **exit_criteria**：定位到 `file:line`；修复后 si=54/56 转绿（`assertFailed=true` 或通过）
- **deliverables**：根因结论 + 修复 + 反例
- **dependencies**：P1
- **resolved_decisions**：L2 投入（用户 Q4），已取证确认存在
- **watch_items**：**我已两次判断失误**，必须以直接行号取证为准，不得靠推理

### P3 — A1 定位 + 修复

- **goal**：确定标记指针漏出的层（ATG 返回类型推断 vs codegen 引用类型返回槽）
- **exit_criteria**：定位到层；修复后 6 项 `value` 不再是标记指针
- **deliverables**：根因结论 + 修复 + 反例
- **dependencies**：P0（AGP 文件可能重叠，实际串行在其后）
- **resolved_decisions**：并入本 roadmap（用户 Q5）
- **watch_items**：数值随构建波动（4→6 项），以「形态」而非「条数」为判据

### P4 — A2 实现

- **goal**：`ChaosReflectionPropertyGetConstantValue` 等返回真实语义值
- **exit_criteria**：`GetRawConstantValue` 系列返回正确值；descriptor 扩展向后兼容
- **deliverables**：descriptor 字段 + codegen 发射 + native 实现
- **dependencies**：P3
- **resolved_decisions**：字段**末尾追加**（既有约定）
- **watch_items**：跨层改动需确认旧产物不破

### P5 — 收口

- **goal**：反射 chunk `caught` 计数下降；对标确认
- **exit_criteria**：`caught` 显著下降；与 `.NET 8` 语义对标一致
- **deliverables**：归因报告更新 + STATUS 归档
- **dependencies**：P4
- **resolved_decisions**：三档判定制沿用
- **watch_items**：HotUpdate 回归

---

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| L1-1 | 0 | completed | main | `EmitIRWhileLoop` 两分支实现方案 B | — | seq-1 | 生成物形态正确 | `StructuredIR.Emit.cs` | 条件在循环内重算 | `src/managed/Chaos.IL2CPP.Generator/Emission/` | 中 |
| L1-2 | 0 | completed | main | 反例：就地回退 → 循环必须变红/错 | L1-1 | seq-1 | 反例实机执行 | commit message 记录 | 反例可复现且回退干净 | 同上 | 小 |
| L1-v-1 | 1 | completed | main | 4 chunk 回归 + 16 处 while 核对 | L1-2 | seq-2 | 4 chunk 全绿 | 回归记录 | 无新红（或已甄别预存在） | 无（只跑管线） | 中 |
| L2-1 | 2 | completed | main | 取证明确定位 latch 外置的切分点 | L1-v-1 | seq-3 | 定位到 file:line | 根因结论 | 定位证据可直接复现 | `StructuredControlFlow*.cs` | 中 |
| L2-2 | 2 | completed | main | 修复 latch 归属 | L2-1 | seq-3 | 修复 + 反例 | 代码 + commit | si=54/56 转绿 | 同上 | 中 |
| A1-1 | 3 | completed | main | 定位标记指针漏出的层 | L2-2 | seq-4 | 定位到层 | 根因结论 | ATG vs codegen 二选一定案 | 待定位 | 中 |
| A1-2 | 3 | completed | main | 修复 | A1-1 | seq-4 | 6 项 value 正常 | 代码 + commit | 不再出现标记指针 | 待定位 | 中 |
| A2-1 | 4 | completed | main | descriptor 扩展设计 | A1-2 | seq-5 | 设计定稿 | 设计摘要 | 字段末尾追加、向后兼容 | `reflection_query_model.h` | 中 |
| A2-2 | 4 | completed | main | native 语义实现 | A2-1 | seq-5 | 语义值正确 | 代码 + commit | `GetRawConstantValue` 正确 | native + registry | 大 |
| P5-1 | 5 | planned | main | 收口 + 对标 | A2-2 | seq-6 | caught 下降 | 报告 + 归档 | 对标一致 | 无 | 小 |

---

## 7. 依赖

```
L1-1 → L1-2 → L1-v-1 → L2-1 → L2-2 → A1-1 → A1-2 → A2-1 → A2-2 → P5-1
```

**强串行**：所有子任务改同一批 codegen 文件，且 L1 的改动会重排生成物 ——
A1/A2 的取证若在 L1 之前做，读数会漂移。

---

## 8. 风险

| 风险 | 等级 | 触发条件 | 缓解 | 备选路径 |
|---|---|---|---|---|
| 方案 B 条件重复求值引入副作用 | 🟡 中 | 条件块含非纯指令 | 逐条核 `ConditionInstructions` 纯性 | 该循环回退方案 A（`while(true)+break`） |
| L1 影响所有 while 循环 | 🟡 中 | 共享热路径 | Q2=B2 跨 chunk 回归 | 若回归暴露问题，收窄为仅 `isReversed` 路径生效 |
| **L2 定位错误** | 🟠 中高 | 我已两次判断失误 | P2 强制直接行号取证 | 若三轮未定位，转 `dev-brainstorm` 重估 |
| A2 跨层扩展 descriptor | 🟠 中 | 影响 codegen→native 契约 | 字段末尾追加 | 若破坏兼容，改用 side-table |
| 多 agent 并发改同批文件 | 🟠 中 | 本仓库高频 | 开工前 `git diff --cached --name-only` | worktree 隔离 |

---

## 9. 备选路径

- **若 L1 方案 B 的反例无法构造**（即回退后循环不错）→ 说明 L1 不是活跃缺陷，
  关闭 L1，直接进 P2
- **若 L2 三轮未定位** → 转 `dev-brainstorm`，重新评估是否与 L1 同源
- **若 A1 定位到 ATG** → A2 的 descriptor 工作可能缩小（ATG 侧修即可）

---

## 10. 当前建议推进顺序

`L1-1` → `L1-2` → `L1-v-1` → `L2-1` → `L2-2` → `A1-1` → `A1-2` → `A2-1` → `A2-2` → `P5-1`

理由：L1 是其余所有取证的前提（它重排生成物）。A 族在 L 族稳定后做，避免读数漂移。

---

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `dispatch_model: sequential`（**不需要 DISPATCH.md**）
- `recommended_next_child: L1-1`
- 自动启动前先做最小 `preflight self-review`

---

## 🔴 调查陷阱（必读，已各导致数轮误判）

1. **pipeline 捕获并过滤 TPG stderr** —— 用 `| grep DIAG` 看 pipeline 输出**看不到诊断**，
   从而误判"没触发"。**诊断阶段必须直接跑 TPG**：
   ```bash
   D=artifacts/foundation-dll/System.Private.CoreLib/chunks/reflection
   CHAOS_DIAG_XXX=1 dotnet exec \
     src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Debug/net8.0/Chaos.IL2CPP.Tools.TestProjectGenerator.dll \
     generate-dll --dll $D/managed/CombinedSubjects.dll \
     --metadata $D/managed/subjects/subjects.metadata.json \
     --output /tmp/tpgout --config-tier check --clean 2>&1 | grep DIAG
   ```
2. **`strings -a <dll>` 找不到 .NET 字符串**（UTF-16）。
   用 `open(dll,'rb').read().count(s.encode('utf-16-le'))`
3. **回归测试用 `tests/unit/managed/codegen/`**；`tests/managed/...` 是副本，
   会因 repo-root 定位失败报 **608 个假红**
4. **读生成物前确认构建新鲜**，否则得出相反结论（L2 的判断因此反复过一次）
5. **构建偶被并发 agent 的文件锁阻断**（`MSB3027 ... used by another process`）——
   `dotnet build-server shutdown` 后重试

---

## 附：证据链（实现时对拍用）

### IL 真相（`Assert.AreEqual(byte[])`，`ilspycmd -il` 实测）

```
IL_006e: ldc.i4.0 ; stloc.1                 (i = 0)
IL_0070: br.s IL_00d9                       ← 跳到条件
IL_0072: ... loop body ...
IL_00d5: ldloc.1 ; ldc.i4.1 ; add ; stloc.1 (i++)     ← latch
IL_00d9: ldloc.1 ; ldarg.0 ; ldlen ; clt    (i < len) ← 条件/header
         brtrue IL_0072
```

期望产出 = `while (i < len) { <body>; i++ }` —— **方案 B 天然匹配**
（条件块在 IL 里就位于 latch 之后、body 之前）。

### 当前生成物（committed `24060f15a` 重建，`native-aot.generated.page2.cpp`）

```cpp
1449:  chaos_locals[4] = _s6;      // i++   ← 循环外（L2）
1454:  _s6 = (i < len);            // 条件   ← 循环外（L1）
1456:  while (_s6 != 0) {          // 用陈旧值
1459:      _s3 = chaos_locals[4];  // 循环内只读
```

### 恢复层事实（`Recovery.cs` `BuildLoop` 的 `isReversed` 分支）

- `bodyStart = min(body) = 11`、`bodyEnd = headerIndex - 1 = 15`
- block 15（@off 275）= **latch（`i++`）** ⇒ **范围是对的**，
  缺陷在体块的**切分/排序**（`RecoverStructure(11, 15, loopHeaderOffset=header)` 的返回
  把 latch 排到了 `IRWhileLoop` 之外）

### A1 现象

6 项（数值随构建波动）：`si=27/29/33/159/170/174`，
`value` 均为 `0x80007ff7c078b080` 形式（高位置位 = 标记指针）。
形态：`typeof(...).Assembly|Type.<返回集合的方法>()` + `!= null ? 1L : 0L` 哨兵。

### A2 已定位片段（`remaining_stubs.cpp:1018`）

```cpp
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetConstantValue(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(...);
    if (decoded == nullptr) return 0;
    return 0;                       // ← 解完描述符直接返 0
}
```
`ReflectionQueryPropertyDescriptor`（`reflection_query_model.h:81`）**无 constant value 字段**。
