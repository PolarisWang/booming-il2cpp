# roadmap-v1-01 — Threading Phase A/B/C：receiver 接线 + 测试运维 + benchmark

> **任务 ID**：`threading-receiver-plan`
> **上游设计**：`design-v1-01.md`
> **日期**：2026-09-15
> **dispatch_model**：`sequential`
> **worktree**：`worktree-threading-ph2-receiver`

---

## 1. 目标

补齐 `threading-production-readiness`（Phase 0–4 已交付）遗留的三项硬缺口，
使 threading 子系统达到可交付的工业化品质：

| 缺口 | 现状 | 目标 |
|------|------|------|
| **A. receiver 接线** | 有参数的实例方法**全部**走 fallback（codegen 不传 receiver） | 通过 lowering 层注入 receiver 槽位，接好 12+ 入口 |
| **B. 测试运维** | 36 个 ctest 中大批 `Not Run`（二进制未构建） | 全部构建并跑通，区分真失败 |
| **C. benchmark** | 4 个 HTML 全空模板 | 产出真实 timing 数据 |

---

## 2. 范围边界

**做**：
- codegen lowering 层：instance 方法 SimpleForward 的 receiver 槽位注入
- 恢复并验证 T2.2/T2.4/T2.5 中被删掉的有参数注册
- native 测试补全 + ctest 全绿
- benchmark SDK 路径对齐 + 跑通

**不做**（显式排除，登记为 watch_item）：
- **async/Task 家族**（1965 个 fallback 调用点，占 threading-tasks chunk 65%）→ 独立 roadmap
- **ThreadPoolBoundHandle / Overlapped**（IO 完成端口）→ out_of_scope
- **SubjectInstanceFactory.Create<T>** 泛型降级 → 非 threading 域

---

## 3. 非目标

| 排除项 | 理由 |
|---|---|
| async/Task 家族 | 系统性缺口，两个 roadmap 规模，需独立规划 |
| IO 完成端口 | 当前 ThreadPool 不覆盖，属架构边界 |
| 前缀过捕获 | 审计确认未发生 |
| POSIX 平台 | 仅 Windows 目标 |

---

## 4. 核心问题（实测证据）

**现象**：生成代码中，有参数的实例方法不接收 receiver。

```cpp
// ReaderWriterLockSlim::TryEnterReadLock(int) —— 只传 timeout
extern "C" CHAOS_IL2CPP_INTPTR
chaos_external_runtime_..._TryEnterReadLock_System_Boolean_System_Int32_(
    CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{ return ChaosReaderWriterLockSlimTryEnterReadLockInt32(chaos_fn_arg_0); }

// ReaderWriterLockSlim::EnterReadLock() —— 唯一槽位即 receiver
extern "C" CHAOS_IL2CPP_INTPTR
chaos_external_runtime_..._EnterReadLock_System_Void__(
    CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{ return ChaosReaderWriterLockSlimEnterReadLockInfinite(chaos_fn_arg_0); }
```

**机制**：`CreateDefinitionFromShapeEntry()`
（`NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs:831`）把
`entry.ParameterAbis` 直接当作 shim 的完整参数列表：

```csharp
var parameterSignature = FormatAbiSlotParameterSignature(entry.ParameterAbis);
var argCount = entry.ParameterAbis.Count;
var args = string.Join(", ", Enumerable.Range(0, argCount)
    .Select(i => $"chaos_fn_arg_{i}"));
```

`ParameterAbis` 来自注册时声明的**managed 参数**，**不含 receiver**。
零参数方法恰好因为 `EnterReadLock()` 的注册写了 1 个 placeholder 槽位
（注释写的是「lowering 无条件赋值结果给变量，Void 槽位会 C3313」），
而 lowering 在该方法的求值栈上恰好只剩 receiver —— **巧合**而非设计。

> **这是本 roadmap 最重要的认知**：零参数方法「能用」不是因为 receiver
> 被正确传递，而是因为一个为了绕开 C3313 而加的占位槽，数量上恰好与
> receiver 相等。一旦方法有参数，占位槽被真实参数占据，receiver 就丢了。

---

## 5. 每阶段完成定义

### Phase A — receiver 槽位注入（核心）

- `goal`：instance 方法的 SimpleForward 分派自动包含 receiver
- `exit_criteria`：
  - 生成代码中 `TryEnterReadLock(int)` 的 shim 有 **2** 个槽位（receiver + timeout）
  - 12+ 个有参数入口从 `ChaosExternalRuntimeFallback` 变为真实 `Chaos*` 调用
  - **零参数方法不回归**（EnterReadLock 等仍是 1 个槽位）
  - 其他 chunk 不回退（至少验证 2 个非 threading chunk 构建不受影响）
- `deliverables`：
  - `NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs` 的 receiver 注入
  - 恢复 T2.2/T2.4/T2.5 被删的注册
  - native 测试覆盖新入口
- `dependencies`：无
- `resolved_decisions`：
  - **方案 A**（用户拍板）：改 lowering 层，一次覆盖全部入口
  - 否决方案 B（InlineShapeDescriptor 逐条手写）：12 条重复劳动且不解决根本
- `watch_items`：
  - receiver 槽位的 TypeSubjectId 怎么填（null vs 声明类型）
  - 是否影响 TaskAwaiter.GetResult 等既有注册

### Phase B — 测试运维

- `goal`：threading ctest 真实全绿（非 Not Run 掩盖）
- `exit_criteria`：
  - 36 个 ctest 全部有二进制且执行
  - 真失败逐条定位并修复或显式登记
  - 并发/压力测试稳定（连跑 3 轮一致）
- `deliverables`：补齐的二进制 + 失败分类报告
- `dependencies`：Phase A
- `watch_items`：`test_threading_stress` 等长跑测试的超时阈值

### Phase C — benchmark 填充

- `goal`：4 个 threading benchmark HTML 有真实 timing
- `exit_criteria`：`perf/` 目录存在；HTML 非空模板
- `deliverables`：benchmark 数据 + HTML
- `dependencies`：Phase A/B
- `resolved_decisions`：捆绑在同一 worktree（用户拍板）
- `watch_items`：SDK 路径分歧（`testing/foundation-dll/sdk/` vs
  `tests/e2e/translation/sdk/`）需对齐

---

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|
| `A.1` | A | planned | tbd | 定位 instance 判定来源（`IsStatic` 在哪个模型上可用） | — | 定位报告 | 明确 receiver 注入点 | 只读 | 小 |
| `A.2` | A | planned | tbd | lowering 层注入 receiver 槽位 | A.1 | `ExternalRuntimeHelpers.cs` 改动 | 生成 shim 有 receiver 槽 | `src/managed/Chaos.IL2CPP.Generator/` | 中 |
| `A.3` | A | planned | tbd | 恢复 T2.2 RWLock TryEnter×6 注册 | A.2 | `Part1.S16.cs` | 6 入口走真实调用 | 同上 | 小 |
| `A.4` | A | planned | tbd | 恢复 T2.4 MRES Wait(int/TS/token)×4 注册 | A.2 | 同上 | 4 入口走真实调用 | 同上 | 小 |
| `A.5` | A | planned | tbd | SpinLock/MRES 的 native 入口签名改回含 receiver | A.2 | `managed_primitive_entries.{h,cpp}` | 签名与 shim 匹配 | `src/native/runtime-core/` | 中 |
| `A.6` | A | planned | tbd | native 测试覆盖新接线入口 | A.3/A.4/A.5 | 测试文件 | 新入口有行为断言 | `tests/unit/.../threading/` | 中 |
| `A.7` | A | planned | tbd | 跨 chunk 回归验证（≥2 个非 threading chunk） | A.2 | 验证报告 | 无回退 | 只读 | 中 |
| `B.1` | B | planned | tbd | 全量构建 threading 测试二进制 | A.* | 36 个 exe | 全部可执行 | `artifacts/`（不入库） | 小 |
| `B.2` | B | planned | tbd | 跑通并分类真失败 | B.1 | 失败分类报告 | 每项有结论 | `tests/unit/.../threading/` | 中 |
| `B.3` | B | planned | tbd | 稳定性验证（连跑 3 轮） | B.2 | 稳定性证据 | 3 轮一致 | 同上 | 小 |
| `C.1` | C | planned | tbd | 对齐 SDK 路径分歧 | — | `tool_helpers.py` 或构建脚本 | benchmark 能找到 SDK | `tests/e2e/verification/` | 小 |
| `C.2` | C | planned | tbd | 跑 benchmark + benchmark_report | C.1 | perf 数据 + HTML | HTML 非空模板 | `artifacts/`、`docs/verification/` | 中 |

---

## 7. 执行顺序

```
A.1 ─→ A.2 ─┬─→ A.3 ─┐
            ├─→ A.4 ─┼─→ A.6 ─→ B.1 ─→ B.2 ─→ B.3 ─┐
            ├─→ A.5 ─┘                            │
            └─→ A.7                               │
                                                  └─→ C.1 ─→ C.2
```

**关键路径**：A.1 → A.2 → (A.3/A.4/A.5) → A.6 → B.1 → B.2 → C.2

---

## 8. 风险

| 风险 | 等级 | 缓解 |
|------|------|------|
| A.2 改动影响其他 chunk 的 instance 方法分派 | 🔴 | A.7 强制跨 chunk 验证，任一回退立即回滚 |
| receiver 槽位的类型信息不足（无 subject id） | 🟡 | 先按 INTPTR 注入，验证通过再考虑精确类型 |
| 既有注册（TaskAwaiter 等）因 receiver 注入而破坏 | 🔴 | A.2 后立即跑全量 chunk 构建，对比 fallback 数量 |
| benchmark SDK 路径分歧根因比预期复杂 | 🟡 | C.1 先只读诊断，确认后才改 |
| ctest 「Not Run」中藏真失败 | 🟡 | B.2 优先做，不假设 Not Run = 通过 |

---

## 9. 三优先级权衡

直接继承父 roadmap：**P1（性能最优）> P2（方案完美性）> P3（HotUpdate）**。

本 phase 无新冲突：
- receiver 注入是**正确性**修复，不涉及性能取舍
- 不触碰 hotupdate 路径
