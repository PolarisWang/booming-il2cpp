# 反射 chunk 失败归因（新口径）

> 日期：2026-09-14 ｜ 数据源：`artifacts/foundation-dll/System.Private.CoreLib/chunks/reflection/native/entry.exe --fact-json`
> 代码基线：`0f6b70b63`

## 为什么现在能做

此前 `assertFailed` 恒为 false（见 `assert-complete-weak-stub-always-won`），
"断言失败" 与 "断言前崩溃" 在任何观测面上都不可区分。经两轮修复后该字段首次可用：

```
249220748  断言失败可归因 — SDK VERIFY 编译 + 判别式查表键 + 调用发射 ABI
0f6b70b63  移除遮蔽 codegen 的 Assert_Complete 弱桩
```

## 总量

| 桶 | 数量 | 判据 |
|---|---:|---|
| 通过 | 247 | `passed=true` |
| **断言失败** | **2** | `assertFailed=true` |
| **断言前抛出** | **34** | `caught=true, assertFailed=false` |
| 干净失败（无抛出） | 0 | — |
| 合计 | 283 | |

## 关键否定证据：AOT 与 JIT 判定完全一致

283 条逐条比对 AOT vs JIT：**0 处判定不一致**。

⇒ 这 34 项**没有一项是 AOT/IL2CPP 专有的实现缺口**。同样的代码在 JIT 下同样失败，
说明成因在 ATG 测试生成或测试设计层面，不在翻译器/运行时实现。

> 这一点修正了本任务早期的推断。此前的记录称「33 项失败均非 native 缺陷」，
> 那是在无法区分抛出原因时作出的**推断**；现在是有证据的**结论**，且口径更精确：
> 非 AOT 专有。

## 桶 A：断言失败（2 项，真信号）

| si | subject | 说明 |
|---|---|---|
| 54 | `AssemblyName.GetPublicKey` | `assertFailed=true, caught=true` |
| 56 | `AssemblyName.GetPublicKeyToken` | 同上 |

形态 `{passed:false, assertFailed:true, caught:true}` —— **断言确实执行并失败**，
不是崩溃。这是本仓库首次观测到的真实断言失败。

**待办**：逐条核对 ATG 期望值与 BCL 语义，判「实现缺口」还是「期望陈旧」。
注意这两项与 `AssemblyName` 链路相关，而该链路刚经历 `e2de17d96` 的对象模型改动，
需确认是否是那次改动的连带影响。

## 桶 B：断言前抛出（34 项，分三个子类）

### B1 — 传 `null` / `default(T)!` 作参数（20 项）

ATG 对反射类型生成的参数表达式退化为 `default(T)!`。BCL 对 null target 抛
`ArgumentNullException`，AOT 同样抛 —— **语义一致，非缺陷**。

代表（`CombinedSubjects.cs:2085`）：
```csharp
var result_0_0 = CustomAttributeExtensions.GetCustomAttribute(
    default(Assembly)!, default(Type)!);          // 两个参数都是 null
return (object)(result_0_0) != null ? 1L : 0L;
```
`CustomAttributeExtensions.GetCustomAttribute(null, null)` 在真实 .NET 下同样抛 ANE。

si: 58, 76, 88, 90, 92, 94, 106, 107, 108, 109, 128, 138, 139, 141, 194, 195, 208, 209, 253, 260

**性质**：ATG 参数生成问题（给定输入无效）。修它要改 `ValueGenerator` 为反射成员类型
提供真实实例（`typeof(...)` / `GetMethod(...)` 等已有先例）。

**但注意**：这类 subject 的**断言本身不成立** —— 方法抛异常是正确行为，
`Assert` 根本不该期望非 null 返回值。所以更根本的修法是让 ATG 在参数为 null 时
**不生成非 null 断言**，或改用 `Assert.Throws<ArgumentNullException>`。

### B3 — 真实实例但上游 API 未注册（8 项）

使用 `typeof(int)` / `typeof(string)` 等真实实例，无 null 参数，但仍在断言前抛出。

| si | 表达式 |
|---|---|
| 133 | `typeof(int).GetFields(Public\|Static)[0].GetRawConstantValue()` |
| 135/136 | 同上的 `GetOptional/RequiredCustomModifiers()` |
| 142 | `typeof(object).GetMembers(Public\|Instance)[0].GetCustomAttributesData()` |
| 176 | `typeof(int).Module.GetTypes()` |
| 201 | `typeof(string).GetProperty("Length")!.GetIndexParameters()` |
| 211/212 | `typeof(string).GetProperty("Length")!.GetOptional/RequiredCustomModifiers()` |

**✅ 已定案根因（si=176 取证）**：不是测试问题，是**漏注册**。

生成物（`native-aot.page-0006.cpp`）：
```cpp
const auto chaos_result =
    chaos_external_runtime_System_Private_CoreLib_System_Type__get_Module_System_Reflection_Module__();
_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
if (chaos_arg_0 == 0) { chaos_runtime_get_abi_v0()->raise_null_reference_exception(); }
const auto chaos_result = ChaosReflectionModuleGetTypes(chaos_arg_0);   // 此处 chaos_arg_0 == 0
```

`Type::get_Module` **没有在 shape registry 注册**（`grep -rn 'get_Module' RuntimeSupport/*.cs`
无结果），于是落到 catch-all 兜底 —— 该兜底**按设计**零参数、返回 0。
`typeof(int).Module` → 0 → 下一个 null-guard 抛 NRE。

**注意**：这与本次修的 **Extended Gold Direct Link ABI 缺陷**是同一类问题的两个面 ——
catch-all 兜底的签名假设（零参数）与真实调用不符，实参被静默丢弃。
本次修的是"有 AOT body 时 ABI 取错"，这里是"根本没注册"。

**修法**：把 `Type::get_Module` 注册为 SimpleForward → `ChaosReflectionTypeGetModule`
（需确认该 native 入口是否存在；`Module` 相关入口已有一批：
`ChaosReflectionModuleGetTypes`、`ChaosReflectionModuleGetName` 等）。

**其余 7 项需同法逐条查上游**：`GetRawConstantValue` / `Get*CustomModifiers` /
`GetIndexParameters` 等同样可能未注册。这条路径比"改测试"更可能是正确修法。

### B2 — `GetParameters()[0]` 链（6 项）

统一形态：
```csharp
typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!
    .GetParameters()[0].<X>()
```
`!` 与 `[0]` 都假设 `GetMethod` 与 `GetParameters` 返回非空。

si: 192, 193, 196, 197, 198, 199

**性质待定**：需与 B3 同样的取证 —— 查 `GetMethod(string, Type[])` 或
`MethodBase::GetParameters` 是否已注册。若未注册则与 B3 同根（漏注册），
若已注册则是重载解析未命中（返回 null，`!` 抑制后 `[0]` 崩）。

**下钻入口**：构造单独的 `GetMethod("IndexOf", new[]{typeof(char)})` subject，
看返回值是否为 null —— 这能一次性区分「解析失败」与「getter 未注册」。

## 排除的假设

| 假设 | 证据 | 结论 |
|---|---|---|
| 「断言根类型在 chunk 闭包外」（早期记录） | `typeof(int)`/`typeof(string)` 在**通过**的 247 项里同样大量出现（71/33 次） | **证伪** |
| 「AOT 专有实现缺口」 | AOT/JIT 判定 0 处不一致 | **证伪** |
| 「靠吞异常才通过」 | 修复后这些项变为可见抛出 | 部分成立：修复前它们静默返 0 或落 caught 兜底 |

## 下一步优先级

1. **桶 A（2 项）** —— 唯一确认的真断言失败，先核对期望值 vs BCL 语义
2. **B3（8 项）** —— **已确认根因是漏注册**，预期收益最大：
   注册一个 `Type::get_Module` 可能连带解开 `GetTypes` 及其下游。
   其余 7 项按同法逐条查上游 API 是否注册
3. **B2（6 项）** —— 用「单独构造 GetMethod subject」区分解析失败 vs 未注册
4. **B1（20 项）** —— 批量修 ATG 参数生成；但先确认"该不该生成断言"这个更根本的问题
   （方法对 null 抛 ANE 是正确行为，不该期望非 null 返回值）

## 一个跨桶的模式

B3 暴露的 `chaos_external_runtime_...__()` 零参数兜底调用，与本次
commit `249220748` 修的 Extended Gold Direct Link 缺陷是**同一类问题的两个面**：
catch-all 兜底的签名假设（零参数 / NativeInt 返回）与真实调用不符。

值得做一次系统排查：**统计所有仍走 catch-all 兜底的反射 API**，
它们要么该注册，要么该显式标记 not-supported —— 当前的静默返 0 是最坏的中间态。

## 复现命令

```bash
CHAOS_FOUNDATION_DLL="D:/agent/chaos-il2cpp/testing/foundation-dll" \
python -m tests.e2e.verification \
  --assembly System.Private.CoreLib --chunk reflection --stages build

artifacts/foundation-dll/System.Private.CoreLib/chunks/reflection/native/entry.exe --fact-json
```

⚠️ 结果一律读 `artifacts/foundation-dll/...`。`tests/e2e/translation/.../results/` 下
存在 Sep-8 陈旧孪生（`realTotal: 5`），不可作为依据。
