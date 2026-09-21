# Create&lt;T&gt; 引用类型改真构造 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 让 `SubjectInstanceFactory.Create<T>()` 对引用类型返回**真正构造过**的实例
（而非零初始化的 `GetUninitializedObject`），使 `ReaderWriterLockSlim` 等持有不变量的
类型的 subject 具备可用状态。

**架构：** 改 `SubjectInstanceFactory.cs` 的引用类型分支，从 `GetUninitializedObject`
切到真构造路径；补齐 native `ChaosReflectionCreateInstance` 的**无参 `.ctor` 调用**
（现在 `args == 0` 时只分配、不调 ctor）。因为生成的 `CombinedSubjects.csproj`
**ProjectReference** SDK 工程，改源码即自动传播，无 SDK 重装步骤。

**技术栈：** C# (SubjectInstanceFactory, SDK) + C++ (reflection/invoke.cpp) + Python 验证管线

**架构审核模式：** critical（命中 codegen / contracts 主线）

**结构告警重点：** 是否新增平行 helper（否决过 F2 抽象层方案）；`invoke.cpp` 无参分支
补全是否与有参分支重复逻辑（应抽出共用）；SDK 与 codegen 的职责边界是否被打破。

**权责图审核主题：**
- authority 主体：`SubjectInstanceFactory.Create<T>()`（SDK 层）→
  `ChaosReflectionCreateInstance`（native 层）→ fact 分类器（Python 层）
- 预期 proof：`Create<T>()` 对有无参 ctor 的类型返回**已构造**实例（可观察副作用断言）；
  native 无参路径确实调用了 `.ctor`（静态计数断言）；fact 层能如实区分
  「构造失败 → factoryGap」与「方法断言失败 → failed」

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: [`ManagedPrimitiveEntries.AnUnboundInstanceIsNotASuccessfulEnter`（既有契约测试，须保持绿）, `RWLS TryEnterReadLock` 等 6 个 subject 的 fact 记录]
- requiredGates: collector -> registry -> workspace（本任务走 foundation-dll 管线，非 AOT onboarding）

**设计文档：** `docs/dev/in-progress/lock-handle-binding/design-v1-01.md`

**问题清零来源：** brainstorm-approved STATUS.md（`blocking_questions: []` /
`question_clearance: cleared` / `clearance_confirmed_by_user: true`）

**计划来源：** direct-plan

**预期知识沉淀：** `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/`（若最终改动
翻译路径约定）；否则 n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed ->
合并&提交"固定链路。

---

## 〇、执行前必读：两个已核实的关键约束

### 约束 1 — 既有契约测试断言的是「未绑定 = 失败」，**不得破坏**

`tests/unit/runtime-native/runtime-core/threading/managed_primitive_entries_test.cpp`：

```cpp
TEST(ManagedPrimitiveEntries, AnUnboundInstanceIsNotASuccessfulEnter)
{
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(AsIntPtr(&neverConstructed)), 0)
    ...
}
```

其注释写明：**"An UNBOUND instance ... is reported as failure, NOT as success.
This is the whole point of the task."**

**本设计不与之冲突**：真构造后实例是**已绑定**的，该测试用的是**手工构造的
零初始化 FakeRwls**，仍然有效、**应保持绿**。native 侧的「未绑定→答 0」语义
**不改**。本设计改的是**上游**（让实例别再是未绑定的）。

> ⚠️ 执行时**不要**为了让 subject 转绿而改这个测试或改 `EnterWithHandle` 的
> 未绑定分支——那会重演 `ExitReadLock 返回 42L` 的假绿缺陷。

### 约束 2 — RWLS **没有** `.ctor` shape 注册（设计未预见，Step 1 先探）

实测：`grep -c 'Rwls, ".ctor"' S16.cs` = **0**。而已有的 CTS 先例
（`chaos_cancellation_token_source_create`）是**把 id 作为 ctor 返回值**，
与 RWLS 的**「句柄存在实例字段上」**模型**不同构**。

因此「真构造 → RWLS 实例带句柄」**不是现成的**，Step 1 必须先探明：

- 若 `Activator.CreateInstance` 走的 `.ctor` 能通过 `CtorReturnsNativeHandle`
  （`5184a0e59` 建立）机制**同时**做到「创建锁 + 存进实例字段」→ 直接可用；
- 若不能 → 需要新增 RWLS `.ctor` shape，或回到 Q2-c（native 侧按类型补）。

**这一步的结论会决定后续步骤的形态，不得跳过。**

---

## 一、文件清单与职责

| 文件 | 动作 | 职责 |
|---|---|---|
| `src/reference/Chaos.TestFramework.Sdk/SubjectInstanceFactory.cs` | 改 | 引用类型分支改真构造 |
| `src/native/runtime-core/reflection/invoke.cpp` | 改 | `ChaosReflectionCreateInstance` 补无参 `.ctor` 调用 |
| `src/managed/.../RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs` | 可能改 | 若 Step 1 判定需要，新增 RWLS `.ctor` shape |
| `tests/unit/runtime-native/runtime-core/threading/managed_reflection_create_test.cpp` | 新增 | 无参 ctor 调用契约测试 |
| `docs/dev/in-progress/lock-handle-binding/notes/progress-*.md` | 按需 | 阻塞/决策/验证失败时追加 |

---

## 二、任务分解

### Step 1 — 探明真构造路径能否让 RWLS 实例带上句柄（**阻塞性前置**）

- [ ] 读 `ChaosReflectionCreateInstance` 与 `CtorReturnsNativeHandle` 的接线，
      确认无参 ctor 调用的返回值语义
- [ ] 用 `dotnet` 反射确认 `ReaderWriterLockSlim` / `ManualResetEventSlim` /
      `SpinLock` 各自**是否有公开无参 ctor**（`SpinLock` 是 struct，走 `default(T)` 分支不受影响）
- [ ] 判定：真构造后 RWLS 实例的句柄字段**是否会非 0**
- [ ] 把判定写入 `notes/progress-step1.md`，并据此确认/修正 Step 3 形态

**退出条件**：明确写出「真构造 → 实例已绑定」成立/不成立及依据。
**若判定不成立** → 停止，回报并改走 Q2-c（设计 §6.1 已备回退方案）。

### Step 2 — native：`ChaosReflectionCreateInstance` 补无参 `.ctor` 调用

- [ ] 抽出一个共用的 `InvokeCtor(type, obj, argc, argv)` 辅助，
      让 `args != 0` 与 `args == 0` 两条路径共用（消除重复逻辑）
- [ ] `args == 0` 时：`type_find_method(type, ".ctor", 0)` 找无参 ctor 并 `method_invoke`
- [ ] 找不到无参 ctor → 保持现状（返回已分配对象），**不抛**——
      由上层 factory 决定失败语义（Q3：throw → factoryGap）
- [ ] 新增契约测试 `managed_reflection_create_test.cpp`：
      用静态计数断言 `.ctor` **确实被调用**

**验证**：native 测试通过。

### Step 3 — SDK：`Create<T>()` 引用类型改真构造

- [ ] 引用类型分支从 `GetUninitializedObject` 改为 `Activator.CreateInstance(typeof(T))`
- [ ] **保留** null 检查与 `throw`（Q3 裁决；源码注释已证明返 null 会 `ud2`→SIGILL→整 chunk 归零）
- [ ] 更新 XML 文档注释：说明"真构造"契约与无法构造时的失败方式
- [ ] 若 Step 1 判定需要，新增 RWLS `.ctor` shape 注册

**验证**：SDK 编译通过；threading chunk build 通过。

### Step 4 — 探针：threading chunk 改前后对比（**关键决策点**）

- [ ] 记录改前基线：499/523 passed（当前 main 实测值）
- [ ] 在 **worktree 隔离**环境跑 `--chunk threading --stages build,fact`
- [ ] 统计：转绿数 / 转红数 / 新增挂起数
- [ ] 判定（设计 §七 R1 的判据）：**转绿 > 转红 且无新增挂起** → 继续；否则回退

**退出条件**：拿到明确的「N 绿 / M 红 / K 挂起」三元组。
**不满足判据** → 回退 Step 3，改走 Q2-c，并记录原因。

### Step 5 — 全量回归（Step 4 通过后）

- [ ] 跑 `system` / `threading-tasks` / `reflection` chunk 对比
- [ ] 任一回退 → 停止并回报（设计 §九 验收口径）
- [ ] 确认既有契约测试仍绿：
      `AnUnboundInstanceIsNotASuccessfulEnter`（约束 1）
- [ ] 确认 snapshot 套件 108/108（`SNAPSHOT_UPDATE=1` 后**再跑一次 clean run**）

### Step 6 — 收尾

- [ ] 结构告警与架构审视
- [ ] 测试通过（上述全部）
- [ ] 归档 `docs/dev/in-progress/lock-handle-binding/` → `docs/dev/completed/`
- [ ] 合并 & 提交（commit message 含 `root_cause` / `fix_strategy` / `regression_check`）

---

## 三、明确不做（设计 §十一）

| 排除项 | 理由 |
|---|---|
| C 类（MRE 等必然阻塞的 subject） | 运行时行为符合 .NET 语义；归 ATG，与 `Wait_7__0` 同族 |
| B 类撤 SpinLock 对象头守卫 | 独立的**前置清理**，不属"真构造"主线；另开小改动 |
| 改 `EnterWithHandle` 的未绑定分支 | 会破坏约束 1 的契约测试 |
| 生产路径的 `GetUninitializedObject` 语义 | 本设计只动**测试 fixture** 的 factory |
| async/Task 家族 | 上游 roadmap 已排除 |

---

## 四、风险与缓解（承接设计 §七）

| # | 风险 | 本计划中的缓解 |
|---|---|---|
| R1 | 大量 subject 行为改变 | **Step 4 探针**，不满足判据即回退 |
| R2 | 性能退化 | 改动只在 fixture 路径，不进生产热路径 |
| R3 | 无参 ctor 不存在的类型 | 落 `factoryGap`（Step 2 明确不抛） |
| R4 | ctor 副作用（Timer/Mutex） | Step 4 统计转红项逐条归因 |
| R5 | C 类仍阻塞 | C 类不在本计划范围；`47dab8dff` 已修超时保留 |
| R6 | 主树并发度高 | **worktree 隔离**；跑前核对 `git status` |

---

## 五、验证命令

```bash
# native 契约测试
cmake --preset windows-x64-reference
cmake --build artifacts/presets/windows-x64-reference --target chaos_managed_reflection_create_test
artifacts/native-runtime-core-test/Debug/chaos_managed_reflection_create_test.exe

# 既有契约测试（约束 1，必须保持绿）
cmake --build artifacts/presets/windows-x64-reference --target chaos_managed_primitive_entries_test

# 探针 / 全量
export CHAOS_FOUNDATION_DLL=$PWD/testing/foundation-dll
export PYTHONPATH=$PWD/tests/e2e
python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.CoreLib --chunk threading --stages build,fact --native-config check

# snapshot
dotnet build tests/unit/managed/snapshot/FixtureAssembly/ -c Release   # 前置：非 ProjectReference
SNAPSHOT_UPDATE=1 dotnet test tests/unit/managed/snapshot/
dotnet test tests/unit/managed/snapshot/   # ← clean run 才算验证
```

---

## 六、执行时如何维护文档

- `STATUS.md` 是**主记录**：每完成一个 Step 更新 `phase` 与结论
- `notes/progress-*.md` **不是**例行更新点，只在**阻塞、重要决策、验证失败**时追加
- Step 1 与 Step 4 的判定结论**必须**写入 `notes/`（它们是后续恢复的关键上下文）
