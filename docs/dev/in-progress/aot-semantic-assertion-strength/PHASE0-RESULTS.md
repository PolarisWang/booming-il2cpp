# Phase 0 结果：断言生成机制已建立，但判别式未通过反向验证

> 日期：2026-09-13 ｜ 状态：**机制可行，判别式待重做**

## 已完成的（机制本身）

按方案 A 在 ATG 建立「判别性期望表」：`TestEmitter.DiscriminatingExpectations`
（declaringType + method → 谓词表达式），命中时绕过 probe 的哨兵、发射强断言。

**机制工作正常**，生成物确认：

```csharp
public long GetExecutingAssembly_3__0()
{
    var result_3_0 = global::System.Reflection.Assembly.GetExecutingAssembly();
    var __corelib_assembly = typeof(int).Assembly;
    var __assemblies_match = result_3_0 != null && result_3_0.GetName().Name != null
        && result_3_0.GetName().Name!.Length > 0
        && result_3_0.GetName().Name != __corelib_assembly.GetName().Name;
    Assert.IsTrue(__assemblies_match, "...");
    return 1L;
}
```

- 表为空时**行为与改动前逐字节一致**（无匹配走原路径）
- 不触及 manifest 主线、不触发快照测试要求（符合治理规则 2/5/6 的评估）

## 途中修正的两个自身缺陷（已解）

1. 谓词初版写成 `{ ... }` 语句块 → 非合法表达式 → CS0106。改为纯表达式。
2. 谓词用 `Type.GetType("System.Object")` 取 CoreLib 名 → 该调用在本 chunk
   **无 native body，走 ChaosExternalRuntimeFallback 返回 0** → 解引用触发
   NRE，掩盖真实结果。改用 `typeof(int).Assembly`（本 chunk 已有成功用例）。

## ❌ 未通过：Phase 1 反向验证

**关掉 REF-RISK-7 接线后，断言仍然通过** —— 判别式无效。

接线关闭已确认有效（生成物中 `ChaosExecutingImageScope` 计数 0），
故问题不在接线开关，而在**判别式本身**。

### 已排除的解释

- 不是"接线没关"：实测注入点 0
- 不是"断言没执行"：生成物中 `Assert.IsTrue` 在位

### 待查明（按三修规则停下）

接线关闭时应回退到 `kImageCoreLib`，其 `image_name_utf8` 为 `"System.Private.CoreLib"`；
`typeof(int).Assembly.GetName().Name` 也应是同一字符串，理论上应**相等**→断言应失败。
实际却通过，说明**至少一侧在 AOT 下未返回该字符串**。两种可能：

1. `Assembly.GetName().Name` 在 AOT 下的降级路径返回值与预期不同（需实测该值）
2. `typeof(int).Assembly` 在 AOT 下解析到的不是 CoreLib 镜像

**下一步**：在生成的测试体里临时输出这两个字符串（或其一是否为 null），
一次运行即可定性。**不再靠推理改谓词** —— 本项已用掉三次尝试。

## 结论

- **基础设施可用**：判别性期望表机制成立，可承载任意强断言
- **判别式需要一次实测**：拿到 AOT 下 `Assembly.GetName().Name` 与
  `typeof(int).Assembly` 的真实值后，谓词可一次写对
- 本项**不应在判别式未验证的情况下合入** —— 一个"永远为真"的断言比没有断言更糟

---

## 根因定位（实测，2026-09-13 续）

反向验证失败的真因**不在判别式**，而在 **runner 的失败归因逻辑**。

### 证据链

1. 接线关闭的产物中，该 subject 记录为 `passed=true, value=0`
2. 但我的断言成功时 `return 1L` —— **`value=0` 说明断言失败、未走到 return**
3. `stderr` 中 **没有任何 `[ASSERT FAIL]`** —— 断言抛出的 `AssertionException`
   被 dispatcher 的异常处理吞掉了

### runner 的判定逻辑（`TestProject.RuntimeEntry.cpp.scriban:427-431`）

```cpp
#ifndef CHAOS_IL2CPP_JIT_MODE
        bool assertFailed = !caught &&
            (Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() != 0);
        Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset();
#else
        bool assertFailed = false;
#endif
        bool passed = !caught && !assertFailed;
```

`assertFailed` 需要 `!caught`；而断言抛异常时 `caught=true`，
`passed = !caught && ...` 也应为 `false`。**但实测 `passed=true`** ——
说明该 subject 走的不是这条分支（或 `caught` 未被置位）。

### 关键结论

**runner 对「断言失败」的归因与实测不符** —— 这本身就是
"验证不可信"的又一层体现，且**比本任务的原始目标更基础**：

- 原目标：让断言能判别语义（**机制已建成**）
- 新发现：即使断言判别正确，**runner 可能仍报 passed**

**在 runner 归因修好之前，任何"强断言"都无法可靠地产生红灯。**

## 建议

本项**扩围为两个阶段**，且顺序必须调整：

| 阶段 | 内容 | 理由 |
|---|---|---|
| **P-1（前置）** | 修 runner 的断言失败归因 | 否则强断言无效 |
| **P0** | 判别性期望表（**已完成**，待 P-1 后验证） | 机制就绪 |
| **P1** | 反向验证 | P-1 + P0 就绪后一次通过 |

**未合入**：在归因修复前，判别式无法被证明有效；一个不可靠的红灯与假绿同样有害。

---

## 三层缺陷链（最终定位，2026-09-13）

追查反向验证失败的过程中，暴露出**三层各自独立的验证缺陷**。任一层存在，
"强断言"都无法可靠产生红灯：

### 层 1：断言框架的失败标记是 ThreadStatic

`src/reference/Chaos.TestFramework.Sdk/Assert.cs:12`

```csharp
[ThreadStatic] private static int s_exitCode;
internal static void RecordFailure() => s_exitCode = 1;
internal static int Complete() { int c = s_exitCode; s_exitCode = 0; return c; }
```

`Assert.Fail`（:250）设 `s_exitCode = 1` 后抛 `AssertionException`。
若失败发生在与 `Complete()` **不同的线程**，`Complete()` 读到的是那个线程的 0。

### 层 2：runner 的归因表达式把 caught 与 assertFailed 耦合

`TestProject.RuntimeEntry.cpp.scriban:427-431`

```cpp
bool assertFailed = !caught && (Assert_Complete() != 0);
bool passed = !caught && !assertFailed;
```

`assertFailed` 要求 `!caught`。当断言抛出的异常在 dispatcher 内被吞掉、
`caught` 保持 false 时，判定**完全依赖** `Assert_Complete()` —— 即层 1 的
ThreadStatic 读值。

### 层 3：JIT 模式直接不检测断言失败

同文件 `#else` 分支：

```cpp
#else
        bool assertFailed = false;      // JIT 模式恒 false
#endif
```

**JIT 路径下断言失败永远不会被上报。**

### 实测印证

接线关闭的产物中该 subject 为 `passed=true, value=0`：
- `value=0` 证明断言失败、未走到 `return 1L`
- `passed=true` 与 `passed = !caught && !assertFailed` 矛盾，
  除非 `caught=false` 且 `Complete()=0` —— 正是层 1 + 层 2 的组合。
- `stderr` 无 `[ASSERT FAIL]`，与"异常被吞"一致。

## 影响面（超出本任务）

这不只影响反射线：**任何 subject 的断言失败都可能被报成 passed**。
`fact_chunk.py` 已经把 `assertFailed` 当作"genuine verification failure"的判据
（见其注释），但该判据的上游本身就不可靠。

## 结论与建议

本任务（让验证能观测 AOT 语义）**被这三层阻断**。正确顺序：

| 阶段 | 内容 | 状态 |
|---|---|---|
| **P-1** | 修三层归因缺陷（ThreadStatic → 进程级；解耦 caught/assertFailed；补 JIT 分支） | **未开始** |
| **P0** | 判别性期望表 | ✅ **机制已完成**（本分支） |
| **P1** | 反向验证 | 待 P-1 |

**未合入 main**：判别式在归因修好前无法被证明有效。WIP 保留在本分支。
