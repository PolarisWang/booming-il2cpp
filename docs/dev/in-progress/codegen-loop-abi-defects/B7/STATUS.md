# B7 — 反射查询的 sentinel 兜底桩（第二条 abort 链）

```yaml
task_id: B7
task_type: plan
phase: investigation
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: in-progress
```

## Scope

B5（`Type::GetProperty/GetField/GetEvent` 改查表）后仍剩 **14 处 `[ABORT-FAULT]`**。
本项定位这第二条链。

## 结论：**catch-all 返回 sentinel 哨兵地址，下游解引用即崩**

### 14 处 abort 的 subject（实测）

```
si=54, 56, 138, 139, 141, 142, 192, 193, 194, 195, 196, 197, 198, 199
```

分三组，但**共用一个根因**：

| 组 | C# 形态 |
|---|---|
| A（2） | `GetExecutingAssembly().GetName().GetPublicKey()` |
| B（5） | `typeof(object).GetMembers(BindingFlags)` → 索引 `[0]` |
| C（7） | `typeof(string).GetMethod("IndexOf", new[]{typeof(char)})!.GetParameters()[0].<X>()` |

### 根因：`GetMembers` / `GetMethod(string, Type[])` 是 **sentinel 兜底桩**

生成物（`native-aot.generated.cpp:31453` / `:31467`）：

```cpp
extern "C" CHAOS_IL2CPP_INTPTR
chaos_external_runtime_..._Type__GetMembers_System_Reflection_MemberInfo___System_Reflection_BindingFlags_(
    CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1) noexcept
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);   // ← 1 字节静态地址
}
```

**关键**：返回的是 `&s_sentinel` —— **非 null 但完全无效**。
所以调用方的 `!`（null-forgiving）**挡不住**：指针非 null，后续
`.GetParameters()` / `[0]` 从 1 字节静态读结构体 → 越界 → abort。

### 为什么不是 B5 的同类

B5 修的是**显式 `CHAOS_IL2CPP_FAIL()`**（主动 abort）。
本链是**静默返回假指针**（被动崩）—— 更隐蔽：既不 abort 也不返 null，
调用方没有任何可判别的信号。

这与本仓库既有的 [[fact-real-kind-does-not-detect-thrown-exception]] 同族：
**"看起来有值" 比 "明确报错" 更危险。**

### sentinel 桩的完整清单（本 chunk 14 处）

```
Assembly::GetManifestResourceStream(String)      ×2（Stream/Type 两个重载）
Assembly::GetModule(String)
Type::GetMembers(MemberInfo[], BindingFlags)     ← 本链 5 项
Type::GetMethod(String)                          ← 会再取到
Type::GetMethod(String, Type[])                  ← 本链 7 项
Type::GetNestedType(String, BindingFlags)
...
```

**其中 `GetMethod(String)` 单参版也是 sentinel** —— 意味着 B5 修好的
`GetProperty` 可能把流量导到这里（si=54/56 就是 `GetName().GetPublicKey()`
之外的路径，需再核）。

## 修法（与 B5 同构）

这三个入口都需要**真实实现**，而不是 sentinel：

| 入口 | 数据源 | 可行性 |
|---|---|---|
| `Type::GetMembers(BindingFlags)` | 需要 members 合并表（fields+methods+properties+nested） | 🟡 已有 `ChaosReflectionGetMembers(type)`（无 flags），可扩展 |
| `Type::GetMethod(String, Type[])` | 需要**参数签名匹配**（不只是名字） | 🟠 当前 descriptor 的 method 表有 `parameter_count`，但无参数**类型**匹配能力 |
| `Assembly::GetModule(String)` | 已有 `ChaosReflectionAssemblyGetModuleByName`？待核 | 🟢 可能已存在 |

**注意**：`GetMethod(String, Type[])` 是**唯一带签名匹配需求**的 ——
现有 `FindReflectionQueryMethod(type, name, parameter_count)` 只按
名字+参数**个数**匹配，`IndexOf(char)` 与 `IndexOf(string)` 会歧义。
要正确实现需扩展 descriptor 携带参数类型，属**跨层**（同 A2 的 descriptor 扩展）。

## 与 B5 的边界

- B5 已完成（`2add27c1e`）：三个**显式 FAIL** 桩 → 查表
- **B7（本项）**：sentinel 假指针桩 → 需真实实现，且 `GetMethod(String,Type[])`
  可能需要 descriptor 扩展

## 下一步

1. **先核**：`GetMethod(String)` 单参版是否也 sentinel（影响 B5 的流量去向）
2. **量化**：14 处中，多少只需 `GetMembers` 真实实现（🟢 低成本），
   多少依赖 `GetMethod(String,Type[])`（🟠 需 descriptor 扩展）
3. 据此决定：本 roadmap 内做低成本部分，高成本部分另立

---

## ✅ 量化结果（2026-09-15）

### 1. 两个 GetMethod 重载**都是 sentinel**

```
Type__GetMethod_System_Reflection_MethodInfo_System_String_          → &s_sentinel
Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Type___ → &s_sentinel
```
（单参版虽有 `ChaosReflectionGetMethod` 的 SimpleForward 注册 + 真实实现
members.cpp:108，但**生成物里仍是 sentinel** —— shape 未生效，原因待查；
与 [[discriminator-lookup-key-never-matched]] 的"注册了但没生效"同族。）

### 2. 14 处 abort 按依赖拆分（实测）

| 组 | si | 依赖 | 修法成本 |
|---|---|---|---|
| B | 138,139,141,142,194,195（6 项） | `GetMembers(BindingFlags)` | 🟠 需构造**托管 MemberInfo[]** |
| C | 192,193,196,197,198,199（6 项） | `GetMethod(String,Type[])` | 🟠 需**参数签名匹配**（descriptor 扩展） |
| A | 54,56（2 项） | GetPublicKey 链 | 🔍 abort 点未定位（见下） |

### 3. 关键判断：即使 shape 匹配也修不好

`Part3.S14.cs` 的 GetMembers resolver 本身 emit `return 0`（注释自述
"stub returning null"）—— 6 项 subject 拿到 null 后 `[0]` 照样崩。
**真实修法**需要 native 构造**托管 `MemberInfo[]` 数组对象**，而
MemberInfo 在本运行时是编码句柄（descriptor 指针），没有托管包装对象模型
—— 这是设计级工作，不是补桩。

### 4. 已排除的假设

| 假设 | 排除方式 |
|---|---|
| `ChaosArrayEmpty_Inline` 与 `chaos_managed_array` 布局错位 → length 读垃圾 → 边界检查 `0 >= garbage` → FAIL_FAST | 两布局逐字段比对：header 8B + shape 1B + pad 7B + type_info 8B + **length 均在 offset 24** —— 一致 |
| si=54/56 的 abort 在 GetPublicKey 链的某个 native 调用 | 链上全部调用逐一读过：GetExecutingAssembly / GetAssemblyName / AssemblyNameGetPublicKey / ArrayEmpty_Inline —— 均无 abort 路径 |

## 待办移交

| 组 | 去向 | 理由 |
|---|---|---|
| A（si=54/56） | **`debug-20-real-defects`**（并发线已建，含 SEH 插桩） | 精确 abort 点需 cdb/调试器；SIGABRT 非 SEH，现有插桩打不到 |
| B（GetMembers×6） | 新立子任务 | 需 MemberInfo 托管对象模型设计 |
| C（GetMethod 2-arg×6） | 新立子任务 | 需 descriptor 携带参数类型（跨层，同 A2 模式） |

## Terminal Notes

- B7 调查完成：根因（sentinel 假指针）+ 完整桩清单 + 14 项量化分组 + 修法成本
- **关键教训**：sentinel 假指针比显式 FAIL 更危险 —— `!` 挡不住、无 abort 信号、
  崩在调用方。与本仓库 [[fact-real-kind-does-not-detect-thrown-exception]] 同族：
  **"看起来有值" 比 "明确报错" 更危险。**
- 建议：sentinel 兜底桩应改为显式 FAIL（诚实的响亮失败），在真实实现落地前
  至少可观测 —— 这是低成本的止血项


## Terminal Notes

- 本项是**调查结论**，非实现
- 关键判别：sentinel ≠ null —— 调用方的 `!` 挡不住，这是本链隐蔽的原因
