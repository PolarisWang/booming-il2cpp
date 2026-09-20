# void writer 方法副作用断言 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 让 JSON/XML writer 的 void 方法从 `unassertable` 变为可验证，消除「实现了但无法断言」的盲区。

**架构：** 两条线**机制不同**——XML 侧新增 native 输出读取接口 + probe 捕获输出作期望值；JSON 侧复用 ATG 已有的「外部 stub 应抛异常」机制并**改进为类型校验**（当前是 `catch { }` 不验类型）。

**技术栈：** C# (ATG) / MSVC C++ (native) / shape registry (codegen)

**架构审核模式：** critical（触及 codegen + ATG + native 三条主线）

**结构告警重点：**
- **重复逻辑**：`TestEmitter.cs:458-472` 已有「stub 应抛异常」分支，JSON 侧应**扩展**而非新增平行分支
- **文件职责**：`xml_writer_stubs.cpp` 已 1074 行，新增 `GetOutput` 是否应拆文件
- **codegen 与 Scriban 分层**：本改动全在 C# 宿主侧，**不得**下沉到 Scriban 模板

**权责图审核主题：**
`断言期望值的 authority` —— 谁有权决定「正确行为是什么」？
预期 proof：期望值**全部来自 .NET 8 实测/probe**，不含人工编造的期望。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `atg-assertion`
- capabilityItem: `void-sideeffect-assertion`
- ownerSubjectId: `n/a`（改动跨 ATG/native/codegen，非单一 subject）
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects: `[XmlTextWriter void 方法 ~50 个、Utf8JsonWriter void 方法 ~104 个的 resultKind 迁移]`
- requiredGates: `collector -> registry -> workspace`

**设计文档：** `docs/dev/in-progress/void-writer-sideeffect-assertion/problem-and-options.md`

**问题清零来源：** brainstorm-approved STATUS.md（`blocking_questions: []` / `clearance_confirmed_by_user: true`）

**计划来源：** direct-plan

**预期知识沉淀：** 记忆 `void-writer-assertion-mechanism` + 更新 `json-xml-industrialization` 报告

**三优先级权衡：**

| 优先级 | 本计划表现 | 说明 |
|:-------|:----------|:-----|
| **P1 性能** | ✅ 中性 | 断言只在**测试期**执行；`ChaosXmlWriterGetOutput` 不进生产路径（仅测试调用） |
| **P2 架构** | ✅ 优 | JSON 侧**扩展**既有分支而非新增平行逻辑；期望值统一来自 probe，不引入人工常量 |
| **P3 热更** | ✅ 中性 | 不涉及 hotpatch 路径 |

**无优先级冲突。** 若 Step A 的类型匹配 spike 失败，**不降级为假绿**（宁保持
`unassertable`）—— 这是 P2 让位于「诚实性」而非性能。

**收尾约束：** 执行完成后必须进入「结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交」固定链路。

---

## 0. 前置准备

- [ ] 确认工作树无并行 agent 未提交改动（`git status` 逐文件核对）
- [ ] 记录基线：跑 `System.Private.Xml/xml` chunk，保存 fact.json
  - 预期：`unassertable=173`，其中 void 部分待验证
- [ ] **开 worktree 隔离**（本任务改 native + ATG + codegen，风险高）：
  ```bash
  git worktree add .claude/worktrees/void-assert <base> --detach
  # 数据根必须指向主检出
  CHAOS_FOUNDATION_DLL=<主检出>/testing/foundation-dll
  ```

---

## 1. Step A：JSON 侧 — 改进异常断言（成本最低，先做）

**文件：** `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/TestEmitter.cs:458-472`

**现状问题**（代码注释自承）：
```csharp
try { callExpr; throw new System.Exception("AOT stub did not throw"); } catch { }
```
**`catch { }` 捕获任何异常，不验证类型** —— 只能证明"抛了"，不能证明"抛对了"。

- [ ] 改为**类型校验**：用 probe 记录的 `ExceptionType` 生成具体断言
  - `ProbeResult.HasException` / `ExceptionType`（`Models.cs:44-55`）**已有数据**
  - ⚠️ **约束**（代码注释自承）：*"The AOT side raises a bare `chaos_managed_exception`
    without a managed type attached, so the exact exception class cannot be matched"*
  - → **先 spike 验证**：AOT 侧异常能否匹配类型？若不能，本步降级为
    「断言抛出了异常 + 异常消息前缀匹配」（消息是 `<type>: <msg>` 格式？需实测）

- [ ] 若类型不可匹配，**至少**改为断言「抛出且消息非空」，排除"静默返回"

**验证：** 跑 `text-json` chunk，`Utf8JsonWriter` 的 `unassertable` 应部分转为 `real`

---

## 2. Step B：native — 新增 `ChaosXmlWriterGetOutput`

**文件：** `src/native/runtime-core/runtime_stubs/xml_writer_stubs.{h,cpp}`

- [ ] 新增接口：
  ```cpp
  /// 返回 writer 当前完整输出（托管字符串）。
  /// 用于让 void 写入方法的测试能验证副作用。
  CHAOS_IL2CPP_INTPTR ChaosXmlWriterGetOutput(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
  ```
- [ ] 实现：读 `WriterState::buf`（已存在），用
      `ChaosStringCreateFromUtf8(buf, len)`（`interop_stubs.cpp:112` 已有）转托管字符串
- [ ] 空/无效 handle 的处理：返回空字符串而非抛异常（避免引入新失败）
- [ ] **结构告警**：`xml_writer_stubs.cpp` 已 1074 行，评估是否拆文件

**验证：** 单元层面——构造 writer → 写内容 → 调 `GetOutput` → 比对预期

---

## 3. Step C：shape 注册新接口

**文件：** `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S19.cs`

- [ ] 在 `XmlTextWriter` 的注册区（`~:209-213` 附近）注册 `GetOutput`
- [ ] 参考现有模式：
  ```csharp
  registry.Register("System.Xml.XmlTextWriter", "<方法名>",
      ..., ShapeKind.SimpleForward, "ChaosXmlWriterGetOutput", ...);
  ```
- [ ] ⚠️ **需确认**：ATG 侧调用的方法名（托管侧是什么 API？）
  - 若是 `ToString()` 或 `GetStringBuilder()`，语义可能不同，需对齐

**验证：** codegen 产物中该调用点解析到 `ChaosXmlWriterGetOutput`

---

## 4. Step D：ATG — 扩 probe 捕获 writer 输出

**文件：** `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/ProbeEmitter.cs`、`Models.cs`

- [ ] 参照已有的 `serializableOutRefs`/`_outValues` 机制（`ProbeEmitter.cs:250-256`）
- [ ] 扩展 `ProbeResult` 捕获「调用后的 writer 输出文本」
  - ⚠️ **W3**：改变 schema，需确认下游兼容
- [ ] Probe 侧需要能调用「读取输出」的等价物（.NET 里是 `StringWriter.ToString()`）

**验证：** 跑 probe，确认输出被记录

---

## 5. Step E：ATG — 为 void writer 方法生成副作用断言

**文件：** `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/TestEmitter.cs`

- [ ] 复用已有的识别列表（`:985-1030` `Utf8JsonWriter` / `XmlTextWriter` 分支）
- [ ] 新增分支：void writer 方法 → 生成
  ```csharp
  var w = new XmlTextWriter(new StringWriter());
  w.WriteCData("abc");
  Assert.AreEqual("<![CDATA[abc]]>", <GetOutput 等价物>(w));
  ```
- [ ] ⚠️ **期望值来自 probe**（Step D），**不得**人工编造
- [ ] 若 probe 未捕获到输出 → **保持原 `unassertable`**，不得退化为假绿

**验证：** 生成体中出现断言语句，且不再无条件 `return 42`

---

## 6. 验证

### 6.1 单点验证

- [ ] 跑 `System.Private.Xml/xml` chunk（build+fact）
- [ ] **目标**：`unassertable` 中 void 部分**下降**，`real` **上升**
- [ ] 跑 `System.Text.Json/text-json` chunk 同理
- [ ] 逐 subject 对比迁移矩阵，确认无回退

### 6.2 防止假绿（🔴 关键门槛）

- [ ] **断言必须真的可能失败**：构造一个错误实现，确认断言能捕获
  - 例：临时改 `WriteCData` 输出错误内容 → 断言应失败
- [ ] 确认 `stubGap` **没有**被转成 `real`（那仍是未实现）

### 6.3 回归

- [ ] 跑其他 chunk 确认无副作用（至少 1 个非 JSON/XML chunk）
- [ ] ⚠️ 避开 threading chunk（已知挂死）

---

## 7. 收尾

- [ ] 结构告警与架构审视（按头部关注点逐条过）
- [ ] 权责图审核：确认所有期望值来自 .NET 实测
- [ ] 测试通过
- [ ] 更新 `STATUS.md` + 归档
- [ ] `git diff --cached --name-only` 逐文件核对
- [ ] 提交（含 `root_cause` / `fix_strategy` / `regression_check`）
- [ ] 知识沉淀

---

## 8. 风险与回退

| 风险 | 触发信号 | 回退 |
|:-----|:---------|:-----|
| **R1 AOT 异常类型不可匹配** | Step A spike 失败 | 降级为消息匹配或"抛出了" |
| **R2 断言过严 → 假失败** | 大量新增 failed | 放宽断言；用 .NET 实测校准 |
| **R3 断言过松 → 假绿** | 断言恒真 | **§6.2 必须做**：构造错误实现验证 |
| **R4 probe schema 改动破坏下游** | 下游解析失败 | 新增字段而非改既有字段 |
| **R5 TPG 陈旧** | 改动不生效 | 重建 TPG（记忆 `tpg-bundles-stale-generator-dll`） |
| **R6 并发 WIP 污染** | 提交时出现无关文件 | 逐文件 add |

---

## 9. 未验证 / 诚实标注

- **Step A 的类型匹配可行性未验证** —— 需 spike（代码注释暗示可能不可行）
- **Step D 的 probe 扩展范围未评估** —— 可能比预估复杂
- 本计划的**期望值全部依赖 .NET 8 实测**，若 probe 环境与 AOT 语义有差异，断言会失准
- AOT/JIT 口径不一致（356 条）**不在本计划范围**
