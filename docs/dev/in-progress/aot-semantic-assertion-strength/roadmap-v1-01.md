# AOT 语义断言强度 — Roadmap v1

> **task_id**: aot-semantic-assertion-strength
> **创建日期**: 2026-09-13
> **entry_skill**: dev-project-test-governance → dev-roadmap
> **上游输入**: `docs/dev/in-progress/reflection-production-readiness/AOT-SEMANTIC-VERIFY-DESIGN.md`
> **dispatch_model**: sequential（worktree 串行）

**目标：** 让 `entry.exe` 内的断言能**判别语义**，使 REF-RISK-7 这类修复**可被证明**。

**为什么需要独立任务：** 本项是**基础设施**（服务整个仓库的验证能力），不是反射功能修复。
落点在测试治理主线上，按 `dev-project-test-governance` 必须先冻结 obligation 与边界。

---

## AOT/IL2CPP/Test Governance Intake（**冻结**）

- **capabilityFamily**: `aot-verification-assertion-strength`
- **capabilityItem**: 对 probe 无法捕获返回值的 API，生成**判别性断言**而非非 null 哨兵
- **ownerSubjectId**: `reflection`（首个 consumer；模式本身可泛化）
- **proofRequired**: **true** —— 本项的存在意义就是"让证明成为可能"，自身必须有 proof
- **benchmarkRequired**: false —— 无性能含义
- **hotupdateImpact**: None —— 只影响断言生成，不改运行时行为
- **formalVerificationObjects**: `[entry.exe 内新增的判别性 subject, fact 结果的 real/stubGap 分类]`
- **requiredGates**: `collector -> registry -> workspace`（按 AOT 自测规范 2.1）

---

## 1. 问题陈述（已实测确证）

反射线的"AOT 验证"**无法观测语义**，根因不在"没有机制"，而在**断言强度由 probe 决定**：

`Assembly.GetExecutingAssembly()` 生成的断言：

```csharp
var result_3_0 = global::System.Reflection.Assembly.GetExecutingAssembly();
return (object)(result_3_0) != null ? 1L : 0L;   // ← 只查非 null，无 Assert
```

- 该形态由 `ATG/Program.cs:913-915` 生成
- 该类型的 `probe-results.json` **捕获为空** → 退化为哨兵
- CoreLib 回退同样非 null → **无从判别 REF-RISK-7 是否生效**

**反向测试坐实**：关掉 codegen 接线后，fact 与契约**都仍通过**。

---

## 2. 方案对比（含治理成本）

| | 方案 A：增强 ATG 断言 | 方案 C：手写 AOT subject |
|---|---|---|
| 落点 | `TestEmitter.cs` / `Program.cs` | `chunk/managed/subjects/` + manifest |
| 治理规则 6（快照测试） | **不触发**（非 Planner/Emitter 核心） | 不触发 |
| 治理规则 2/5（manifest 主线） | **不触及** | **触及**（需接 `--custom-cs-files` 到 reflection chunk 流程） |
| 覆盖方式 | 按 API 模板，可泛化 | 手工指定，精确 |
| 成本 | 中（需新建"不依赖 probe 的期望"通路） | 中高（需理解 metadata/coverage 合并） |
| 风险 | 中（改 ATG 断言生成） | 中（改测试治理主线） |

**已核实**：ATG **无任何按 API 定制断言的既有机制**（`grep SpecialAuthoring/KnownExpectation` 无结果），
故方案 A 需要**新建**该机制。

---

## 3. 方案（推荐 A）

### 3.1 核心设计

为 ATG 增加一张**声明式的"判别性期望"表**，对特定 API 绕过 probe、直接发射强断言：

```
ReflectiveAssertionExpectations:
  Assembly.GetCallingAssembly   → 归属判别（== 本程序集名，且 != CoreLib）
  Assembly.GetExecutingAssembly → 同上
```

理由：
1. **不触及 manifest 主线**（规则 2/5），治理成本最低
2. **不触发快照测试要求**（规则 6 只覆盖 Planner/Emitter 核心与模板）
3. 该表可**渐进扩展**——每遇到一个"probe 抓不到但语义可判别"的 API 就加一条，
   不需要一次性设计完备

### 3.2 为什么不选 C

C 的断言可以任意强，但需要接 `--custom-cs-files` 到 reflection chunk 的 metadata/coverage
合并流程（规则 2/5 覆盖范围）。本项是**基础设施**，应优先选"小侵入、可渐进"的路径。

---

## 4. 阶段切分

| Phase | 目标 | 退出标准 |
|---|---|---|
| **0** | 建"判别性期望"机制 + 覆盖 2 个访问器 | 生成物中出现强断言；`entry.exe` 内执行 |
| **1** | **反向验证**（本项的核心验收） | 关掉 REF-RISK-7 接线 → 断言**必须转红** |
| **2** | 泛化到其余"probe 抓不到"的 API | 按需扩展表；每项同样做反向验证 |

**Phase 1 是本任务价值的试金石**：若关掉接线断言不转红，说明机制无效。

---

## 5. 风险

| 风险 | 缓解 |
|---|---|
| **断言过强导致假红**（AOT 与 managed 语义本就不同） | 对照实验：先在**接线正常**时确认通过，再关掉确认转红（双向） |
| 触及 ATG 断言生成，影响其他 chunk | 表为空时行为与现状**逐字节一致**（无匹配即走原路径） |
| 期望值（本程序集名）如何注入 | codegen 已有该信息（`kReflImage` 的 image_name）；优先复用它 |

---

## 6. 当前建议推进顺序

Phase 0 → 1 →（视结果）2。

**注意**：Phase 1 的反向验证需要能临时关闭 REF-RISK-7 接线，故 Phase 0 的实现应保证
该开关可控（例如通过已有的 `MethodCallsAssemblyAccessor` 判定点）。

---

## 7. 子任务映射

| task_id | phase | status | purpose | depends_on | deliverables | exit_criteria |
|---|---|---|---|---|---|---|
| S0-1 | 0 | **completed** | 建判别性期望表 + 覆盖 2 个访问器 | — | ATG 表 + 发射逻辑 | 生成物含强断言 ✅ |
| S1-1 | 1 | **completed** | 反向验证（核心验收） | S0-1 | 验证记录 | ✅ 关接线→转红；开接线→转绿（双向闭环） |
| S2-1 | 2 | planned | 泛化到其余 API | S1-1 | 扩展的表 | 按需，每项反向验证 |

**Phase 0/1 完成记录（2026-09-14）**：判别式定稿为
`Name == "CombinedSubjects"`（subjects 镜像固定名，仅依赖 String.op_Equality——
有 native body 的唯一比较原语）。配套修复：GetAssemblyName 返回托管形状对象
（16B 前缀 + runtime_name_value，修复 offset-16 读取错位）、
ReferenceMatchesDefinition/GetVersion/GetName/GetFullName 从对象模型解码。
双向验证：接线开→val=1 绿；关→红。详见 `PHASE0-RESULTS.md`。
