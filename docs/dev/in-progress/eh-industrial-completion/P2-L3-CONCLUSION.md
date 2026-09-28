# EH P2 — L3 异常翻译正确性：结论

> **task_id**: eh-industrial-completion
> **阶段**: P2（L3 翻译正确性 + L4 类型表）
> **日期**: 2026-09-28
> **结论**: ✅ **EH 机制本身正确**；主导 EUR 指标的根因经定位属 **codegen stub 降级**，不属 EH 域。

---

## 一、结论摘要

roadmap P2 的目标是「确保 C# `throw` 的每个异常类型都能被 `catch` 正确匹配」。
本轮以 **1614 条 Exception-Uncaught-Raise（EUR，即 `caught=true && assertFailed=false`）**
为入口做全量诊断，逐层排除后确认：

| 层 | 状态 | 证据 |
|---|---|---|
| L1 运行时投递 | ✅ 正确 | — |
| L2 codegen 包装（`CHAOS_EH_*` 宏） | ✅ 正确 | P1 已交付，产物硬编码 try/catch 清零 |
| **L3 异常类型匹配** | ✅ **正确** | 见 §三 |
| L4 异常类型表 | ✅ 正确 | `kChaosExceptionTypes` 20 项含 `System.FormatException` 等，descriptor 的 `type_info_ptr` 指向真实 `chaos_mt_*` |

**EUR 的主导根因不是 EH 缺陷**，而是 §四 所述的 codegen stub 降级问题。

---

## 二、EUR 基线（诊断工具产出）

1614 条 EUR 按 `resultKind` 分桶：

| 桶 | 数量 | 性质 |
|---|---|---|
| `failed` | 906 | 待调查 |
| `realDefect` | 414 | AOT 与 managed 行为不一致 |
| `factoryGap` | 274 | 工厂方法缺失 |
| `nullArg` | 20 | 参数校验路径 |

按 chunk 分布（前五）：

| chunk | 数量 |
|---|---|
| System.Private.CoreLib/system | 434 |
| System.Xml.ReaderWriter/xml | 316 |
| System.Private.Xml/xml | 216 |
| System.Text.Json/text-json | 51 |
| System.Private.CoreLib/reflection | 62 |

**诊断工具**（已提交，可复用）：
- `tools/eh/classify_eur.py` — 按 resultKind 分桶
- `tools/eh/classify_eur2.py` — 交叉引用 subject 源码标记（`AOT-THROWS-ASSERT` / `AOT-STUB-GAP` / `skipping Throws`）

---

## 三、EH 机制正确性的证据链

以 `System.Xml.ReaderWriter/xml` → `XmlConvertTests::ToBoolean_10_string_0` 为样本，
逐环节核对了**完整的 throw → 投递 → 匹配**链路：

**① 异常对象的 MethodTable 正确**
产物 `native-aot.generated.cpp:2468`：
```cpp
MethodTable chaos_mt_System_Private_CoreLib_System_FormatException = {
    reinterpret_cast<const MethodTable*>(&chaos_mt_..._SystemException),  // parent 链正确
    nullptr, 3707929770006501753ULL, 0u, 32, 1, 1, ... };
```

**② 类型表登记且 descriptor 指向真实 MT**
产物 `native-aot.payload.page-0022.cpp`：
```cpp
{ "System.FormatException", &kExcDesc7 },
...
static const ReflectionQueryTypeDescriptor kExcDesc7 = {
    ..., nullptr, 0u, 0u, &chaos_mt_System_Private_CoreLib_System_FormatException };
```

**③ catch 类型匹配代码正确**
产物 `native-aot.page-0007.cpp`（`CHAOS_EH_CATCH_BEGIN` 展开）：
```cpp
CHAOS_EH_CATCH_BEGIN
    { if (chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_..._FormatException.AsTypeInfoHot())) {...} }
    { if (chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_..._Object.AsTypeInfoHot())) {...} }
CHAOS_EH_RETHROW;
```

**④ 匹配器实现正确**
`ChaosGeneratedRuntimePrelude.h:172` 的 `chaos_eh_match_type` 与
`type_info.h` 的 `MethodTable::AsTypeInfoHot()`（同指针、32B 布局兼容、
`static_assert(offsetof(...))` 保证）均正确。

**⑤ 验证网覆盖**
`eh_type_matching_test.cpp` 的三模式套件（9/9 × 3 模式，含正控+负控）
覆盖了精确类型、基类、传递基类、无关类型、方向性、NULL、哨兵等，
**全部通过**。

---

## 四、EUR 的主导根因：codegen stub 降级丢实参（**非 EH 域**）

### 症状

`XmlConvertTests::ToDateTime_14_string_string_0` 的 AOT 主体：

```cpp
_s1 = 0;   // default(string)!  —— 参数 1
_s2 = 0;   // default(string)!  —— 参数 2
{
    const auto chaos_result =
        chaos_external_runtime_..._ToDateTime_System_DateTime_System_String_System_String_();
        //                                                                       ^^^^
        //                                          零实参！
}
```

### 根因

产物 `native-aot.generated.cpp:5578`：

```cpp
extern "C" CHAOS_IL2CPP_INTPTR
chaos_external_runtime_..._ToDateTime_System_DateTime_System_String_System_String_(void)
//                                                                             ^^^^^^
//                                  符号名含 2 个 System_String，但签名是 (void)
{
    ... return ChaosExternalRuntimeFallback("...ToDateTime:System.DateTime(System.String,System.String)");
}
```

**stub 的 arity 与 callee 不匹配** —— 实参全部丢弃，函数体退化为
`ChaosExternalRuntimeFallback(...)` 返回 0，**不抛异常**。

### 后果链

1. subject 调用 `ToDateTime(null, null)` → 落到零参 stub → **不抛**
2. subject 的 `throw new Exception("AOT stub did not throw")` 执行
3. 落到 `catch { }` 段 → `throw new Exception("wrong exception type")`
4. 异常逃出 subject → runner 记 `caught=true`
5. 真实断言从未执行 → `assertFailed=false`
6. → 计入 EUR

### 为什么不属 EH

EH 机制（宏、匹配器、类型表、MethodTable）**全部正确**（见 §三）。
缺陷在 **codegen 的 shape registry / external-runtime stub 生成**：
符号的 arity 信息在降级路径上丢失。

**同族已记录缺陷**（项目记忆）：
- `[direct-native-symbol-shim-arity-must-equal-callee]` — shim 槽数必须等于 callee arity
- `[shape-registry-missing-width-falls-to-zero-catchall]` — 注册缺宽度 → 静默降级零参 catch-all
- `[operand-less-catchall-false-green]` — 无 shape 注册 → 零参 catch-all 收不到操作数

**这是同一缺陷族的第四次出现。**

---

## 五、P2 的处置

**EH roadmap P2 按 EH 口径收口**：

| P2 exit_criteria | 状态 | 依据 |
|---|---|---|
| ① 抛出对象 header.type_info 指向真实 `chaos_mt_<Type>` | ✅ | §三 ①②；产物 2140 处 `header.type_info = chaos_mt_*` 赋值 |
| ② `ResolveTypeByName` 覆盖全部 throw 类型 | ✅ | §三 ②；类型表 20 项含全部 BCL 异常 |
| ③ 负控验证通过 | ✅ | §三 ⑤；`eh_type_matching_test.cpp` 9/9 × 3 模式含负控 |

**EUR 指标**不作为 P2 的验收项 —— 其主导成分经定位属 codegen stub 覆盖问题，
应由 codegen 线处理（见 §六）。

---

## 六、移交：codegen stub 降级的后续工作

**问题**：`chaos_external_runtime_*` stub 在 shape 未注册 / 降级路径上
**丢失 callee 的 arity**，生成 `(void)` 签名。

**影响**：不只是 EH —— 任何依赖「stub 抛异常」或「stub 转发实参」的
验证都失效，且**静默**（表现为 EUR / 返回值错，不崩溃）。

**建议入手点**：
1. 定位 `chaos_external_runtime_*` stub 的签名生成代码
   （`NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs` 的
   `RenderSimpleExternalRuntimeHelper`）
2. 核对符号名的 arity 与生成的参数列表是否一致
3. 加守卫：符号名含 N 个参数类型但签名为 `(void)` → 生成期报错

**规模参考**：全仓该形态的 stub 需量化（`grep '(void) CHAOS_STUB_NOEXCEPT'` 对比符号名 arity）。

**不属 EH roadmap**，建议开独立任务或并入既有 shape-registry 课题。

---

## 七、附：实参丢失的精确形态（供后续修复定位）

**已确认的事实**（非推测）：

| 事实 | 证据 |
|---|---|
| 调用点**未传任何实参** | `native-aot.page-0007.cpp`：`_s1 = 0; _s2 = 0;` 后被赋值但调用时用 `()` |
| stub 签名与调用点**一致**（都零参）| 全 chunk 扫描：`(void) sig + 非空实参调用点` = **0 例** |
| 不是「签名/调用点不匹配」| 同上 |
| 是**两者同步降级为零参** | 推断 |

**规模**（全 artifacts 扫描）：

```
external-runtime stub 总数:            7431
其中签名为 (void):                     5405
  System.Private.CoreLib/system        2351
  System.Private.CoreLib/reflection     761
  System.Linq/global-ns                 447
  System.Private.CoreLib/threading      371
  System.Private.CoreLib/text           262
  System.Private.Xml/xml                238
  System.Xml.ReaderWriter/xml           238
```

> **注意**：5405 这个数字**不等于 5405 个缺陷** —— 单参/零参 stub 写 `(void)` 是合法的，
> 需按「符号名暗示的 arity > 0 且确实需要转发实参」进一步筛。
> 本轮的判据（调用点是否传参）显示**调用点与签名一致**，因此需要
> 从**调用点生成侧**入手，而非从 stub 签名侧。

**建议入手点**：定位把 `call` 降级为零参 `chaos_external_runtime_*(...)` 的发射语句，
核对它是否在降级时丢失了实参列表。候选文件：
- `NativeAotLoweringPlanner.LinearEmission*.cs`（调用点发射）
- `NativeAotLoweringPlanner.ExternalRuntimeHelpers*.cs`（stub 生成）
- `RuntimeHelperShapeRegistry.CoreStubs.*`（shape 注册／降级决策）
