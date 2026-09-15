# codegen-loop-abi-defects — STATUS

```yaml
task_id: codegen-loop-abi-defects
task_type: roadmap
phase: roadmap
创建日期: 2026-09-15
entry_skill: dev-brainstorm → dev-roadmap
parent_task_id: null
roadmap_or_plan: roadmap-v1-01.md
关键文档:
  - docs/dev/in-progress/codegen-loop-abi-defects/roadmap-v1-01.md
  - docs/dev/in-progress/reflection-production-readiness/FAILURE-TRIAGE-2026-09-14.md
上游输入: FAILURE-TRIAGE-2026-09-14.md（反射 chunk 失败归因新口径）

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: L1-1
latest_stop_point: roadmap 定稿 + 上游 clearance 已确认；待启动 L1-1
```

---

## 最近摘要

**2026-09-15**：由 `dev-brainstorm` 完成方案拍板并分流为 roadmap。

- **范围**：循环族（L1 `IRWhileLoop` 条件重算 / L1-v 跨 chunk 回归 / L2 latch 外置）
  + ABI 族（A1 标记指针漏出 / A2 `return 0` 桩语义）
- **6 阶段 / 10 子任务 / `dispatch_model: sequential`**（强依赖链、共享同一批 codegen 文件）
- **用户拍板**：L1 用方案 B（P1 性能优先）｜回归范围 B2 跨 chunk｜L2 投入｜A1/A2 并入
- **关键前置已交付**：`24060f15a`（循环恢复三处缺陷，codegen 2252/2252）已推送

**两族缺陷均已取证确认**：
- L1：`EmitIRWhileLoop` 把条件指令发在 `while` 之前 → 陈旧值（真则死循环）
- L2：latch（`i++`）被发在循环外（生成物 1449 行 vs `while` 1456 行）；
  恢复层 `bodyStart/bodyEnd` 范围正确，缺陷在体块**切分/排序**
- A1：6 项 `value` 是标记指针经 `CHAOS_IL2CPP_INT64` 返回槽漏出；根因未查
- A2：`ChaosReflectionPropertyGetConstantValue` 直接 `return 0`；descriptor 无该字段

## 下一步

🔴 **阻塞（blocking-only 停止策略触发）**

`L1-1` 的代码**已实现且编译通过**（+59 行，`StructuredIR.Emit.cs`），
但**无法验证**：`main` 上存在**预存在的 565 项 codegen 测试失败**。

**根因（已定位，属 xml 线在制品，不在本 roadmap 边界）**：
commit `81de30514`（feat(xml): M6 XmlTextWriter 注册）中
`XmlTextWriterParamTypes("WriteStartDocument")` 对两个重载返回**同一个空参数列表**，
导致 `Part1.cs:911` 与 `:917` 注册出**相同的 key**，抛
`Shape already registered: 'System.Xml.XmlTextWriter::WriteStartDocument()'`，
在 `BuildDefault()` 阶段炸掉所有 planner 构造。

**判定证据**：pristine HEAD 干净 worktree 上同样 565 失败；加入本改动后**数量不变**
⇒ 与本改动无关。

**解除条件**：xml 线修掉该重复注册（按 arity 区分，或第二处改用独立 key）。

**2026-09-15 复核**：xml 线正在重构该注册（把 `XmlTextWriter` 块从 `Part1.cs`
整体移到新文件 `Part1.S19.cs`，`Part1.cs` −125 行），**但重复键未修** ——
`S19.cs:128` 的 `XmlTextWriterParamTypes` 对两个 `WriteStartDocument` 重载
仍返回同一空参数表。实测仍是 **565 failed / 1687 passed**。

⇒ 阻塞保持。按用户选择「等 xml 线修掉」，本 roadmap 暂停于此，
`recommended_next_child` 保持 `L1-1`（代码已就绪，待解除后重跑验证）。

**未擅自修改他人提交**（该文件属 `worktree-xml-writer-m6` 活动边界）。

详见 `L1-1/STATUS.md`。

---

## 并行进展（绕过 L1 阻塞）

**A1-1 已定位**（不受 codegen 测试套件阻塞）：

根因在 **codegen 层**（IR→C++ 的 local↔slot 映射），**不在 ATG**。
取证样本 `si=159 ModuleTests::IsResource_1__0`（C# 返回 **bool**，
逻辑上不可能产出托管指针 → 缺陷必在下游）：

- **IR 正确**：`stloc.1` 存 `?:` 结果，`ret` 取 `ldloc.1`
- **emit 错误**：`chaos_locals[1] = ChaosStoreInt64(_s1);` —— 写的是 `GetModule()`
  的**接收者句柄**（→ 正是观测到的标记指针），而 `?:` 的结果 `_i2` **算出后被丢弃**
- 6 项形态**逐字相同** ⇒ 单一根因

⇒ `recommended_next_child` 可推进到 **A1-2**（不受 xml 线阻塞）。
详见 `A1-1/STATUS.md`。

---

## 边界拍板

**覆盖**：循环 lowering（L1 `IRWhileLoop` 条件重算 / L1-v 跨 chunk 回归 /
L2 循环体块切分排序）+ ABI 返回槽（A1 标记指针漏出 / A2 `return 0` 桩语义实现）。

**不覆盖**：`IRDoWhileLoop` 条件语义；反射覆盖面补齐（属 reflection 线）；
漏注册清扫（已在 `ab75ae1fd` 完成主体）；HotUpdate 路径。

## authority 决策

- **L1 实现形态 = 方案 B**（保留 `while(cond)`，条件指令循环体末尾重算）—— 用户拍板，P1 优先
- **回归范围 = B2**（跨 chunk：reflection/system/text/threading）—— 用户拍板
- **L2 投入**（保留为独立子任务）—— 用户拍板；已直接取证确认存在
- **A1/A2 并入本 roadmap** —— 用户拍板；同源 + 共同验证载体
- 三档判定制、in-place revert 反证纪律：沿用项目既有范式

## 当前结论

两族缺陷均源自 2026-09-14 反射 chunk 的失败归因（`assertFailed` 首次可用后）。

**循环族**：C# `for`（body-first/condition-last IL 形态）在嵌套于 `if` 臂内时
曾整体丢失循环结构（已由 `24060f15a` 修掉三处恢复层缺陷，codegen 2252/2252）。
**剩余两处已取证确认**：
- **L1**：`EmitIRWhileLoop` 把条件指令发在 `while` 之前 → `while` 用陈旧值
  （真则死循环）。两个分支（`brtrue/brfalse` 与比较操作码路径）都有。
- **L2**：latch（`i++`）被发在**循环外**（生成物行 1449 vs `while` 在 1456）。
  恢复层的 `bodyStart/bodyEnd` 范围是对的（含 latch），是体块的**切分/排序**有缺陷。

**ABI 族**：
- **A1**：6 项 `value` 是标记指针（`0x80007ff7...`）经 `CHAOS_IL2CPP_INT64` 返回槽
  漏出；根因未查（ATG 推断 vs codegen 返回槽）。
- **A2**：`ChaosReflectionPropertyGetConstantValue` 等解完描述符直接 `return 0`；
  且 descriptor 无 constant value 字段 ⇒ 需跨层扩展。

## 阶段切分

| Phase | 目标 | 出口 |
|---|---|---|
| P0 | L1 取证复核 + 方案 B 实现 | 生成物形态正确；2252/2252 绿 |
| P1 | L1-v 跨 chunk 回归 | 4 chunk 全绿，16 处 while 逐处核对 |
| P2 | L2 定位 + 修复 | latch 在循环内；si=54/56 转绿 |
| P3 | A1 定位 + 修复 | 6 项 value 正常 |
| P4 | A2 descriptor 扩展 + 实现 | 语义值正确 |
| P5 | 收口 | caught 计数下降 |

## 风险评估摘要

| 风险 | 等级 | 缓解 |
|---|---|---|
| 方案 B 条件下令重复求值引入副作用 | 🟡 中 | 条件为纯比较；实现对每个指令核纯性，非纯则该循环回退方案 A |
| L1 改动影响所有 while 循环 | 🟡 中 | Q2=B2 跨 chunk 回归 |
| L2 定位错误（上一轮已两次判断失误） | 🟠 中高 | P2 强制先取证再改 |
| A2 跨层扩展 descriptor | 🟠 中 | 字段末尾追加，新旧兼容 |
| 多 agent 并发冲突 | 🟠 中 | 开工前核对 `git diff --cached --name-only` |

## 三优先级权衡结论

- **P1 性能**：方案 B 优于方案 A（保持 `while(cond)` 自然循环，无每轮额外分支）—— 选 B 首要理由
- **P2 架构**：L1 限 emitter 单函数；L2 在恢复层；A2 扩 descriptor 属既有模式延伸
- **P3 热更**：不改运行时契约；P5 回归确认

## 🔴 调查陷阱（已两次导致误判，必读）

1. **pipeline 过滤 TPG stderr** → 用 `| grep DIAG` 看不到诊断，误判"没触发"。
   **诊断必须直接跑 TPG**（命令见 roadmap）
2. **`strings -a` 找不到 .NET 字符串**（UTF-16）；用 `encode('utf-16-le')`
3. **测试项目路径**：用 `tests/unit/managed/codegen/`；
   `tests/managed/...` 会报 608 个假红
4. **读生成物前确认构建新鲜**，否则会得出相反结论（L2 判断因此反复过一次）

## 下一步

`recommended_next_child: L1-1` —— 实现 `EmitIRWhileLoop` 的方案 B。
