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

---

## ✅ 部分修复已落地（commit `1343fabe0`）

| 修复 | 内容 |
|---|---|
| **Part1.S5 GetMethod 通配 shape 遮蔽** | GenericShape 先于 TryMatchShape 执行，S15 的精确 SimpleForward 注册（含指向真实实现的 1 参版）**永远被遮蔽**。现按 paramTypes 分流：1 参/2 参转发 `ChaosReflectionGetMethod` 真实实现，其余保持桩 |
| **members.cpp 参数个数偏移错位** | `param_count` 读 offset 8（element_type_shape+padding）——两种数组布局的 length 都在 **offset 24**。注释声称 "16 bytes header" 与代码自相矛盾；该路径此前无真实调用者故未暴露 |
| S15 补 GetMethod(String, Type[]) 精确注册 | 被通配遮蔽，保留作后备 |

### 层叠图谱（C 组 si=192 的完整依赖链，逐层实测 — 2026-09-15 更新）

```
层1  ✅ 已修（1343fabe0 续）：newarr 分配实际已发射（ChaosArrayNew1D 在生成体中），
     真 abort 源是 stelem.ref 的 chaos_is_array_store_compatible 对编码句柄读垃圾
     header → FAIL_FAST。已对全部 stelem.ref 跳过该检查（null/bounds 检查保留）。
     实测：C 组 8 项 ABORT → NRE（caught=true），全局 ABORT 14→6
  ↓ 已暴露
层2  ✅ 已修（8bd4c84f8）：methods 表已发射进描述符
  ↓ 已暴露
层1b 🔴→🟡 当前阻断（已精确定位，2026-09-15 cdb+stderr 探针闭环）：
     双重值模型错配，两层叠加：
     (a) typeof 折叠（A2.7）推裸 TypeInfoHot*，而【generated 对象模型】
         （native-aot.generated.cpp:28207 ChaosReflectionGetMethod，自
         ReflectionObjectEmission.cs 发射）把接收者强转为托管
         chaos_type_System_Type* 并读 runtime_type_handle 字段
         （= 0x02000000|FNV32(subjectId)，TLS 缓存的托管 Type 对象）。
         裸指针读出垃圾 handle → resolve_method_handle 不命中 → 返 0 → NRE。
     (b) runtime 侧 GetTypeDescriptorFromHandle 三条解码路径也解不了裸
         TypeInfoHot* —— 已修：ChaosRegisterExternalType 现按描述符的
         type_info_ptr 建【TypeInfoHot*→descriptor 反查表】
         （ChaosFindReflectionTypeByTypeInfo），emitter 仅对
         MethodTable 已定义的类型（_referenceTypeBaseSubjectIds ∪
         _valueTypeStructSubjectIds）填 type_info_ptr。落地后
         passed 249→253。
  → 剩余修复方案（下一会话）：generated 对象模型入口加
     chaos_reflection_coerce_type_value(v)：
     * `*(void**)v != nullptr` → 已是托管对象，原样返回
     * 否则裸 TypeInfoHot* → ChaosFindReflectionTypeByTypeInfo(v) →
       desc → 用 desc->reserved_flags（emitter 填
       0x02000000|FNV32(subjectId)，即 GetTypeHandleLiteral 常量）→
       chaos_reflection_create_type_value(handle)（TLS 缓存，P1 可接受）
     * 对象模型全部 reinterpret 接收者处先过 coerce
     ⚠️ 改 fold 本身（直接推 create_type_value）会波及其他 chunk 的
     enum helper 消费方（fold 注释声称服务 Enum.Format/Parse），风险大；
     coerce 方案只动对象模型入口，fold 保持不变。

     ⛔ **2026-09-15 首次实施失败（已回退，253 恢复）**：
     - 方案变形：reserved_flags 被 enum flag 位占用（enum_stubs.cpp:909
       kFlagIsEnum），不能放 pseudo handle → 改为复用
       get_object_type 的 stable_id→handle 映射表（generated 侧发射
       chaos_reflection_type_handle_from_stable_id），判别器用
       首字段非空 = 托管对象。
     - 结果：passed 253→248，且 si=151–158（MethodInfoTests::
       GetGenericArguments 链）从**干净失败退化为 AV 崩溃**
       （0xc0000005）——coerce 在该路径产生了此前不存在的坏指针。
     - 教训：**判别器/映射表覆盖不足**。Type[] 元素（raw TypeInfoHot*
       经 stelem 存入）在 GetGenericArguments 中被 coerce 成新建托管
       Type 后，后续 generic_argument_type_handle 语义与原先
       "0 或 raw"的假设不符。重试前必须先 cdb 定位 si=151 的精确
       AV 点，把 coerce 的适用面收窄到**显式列出的接收者函数**
       （GetMethod/GetTypeHandle），不要全局替换 11 处 reinterpret。
  ↓ 修好后
层3  FindReflectionQueryMethod 按名字+参数个数匹配 —— IndexOf(char) 与
     IndexOf(string) 个数相同会歧义；需 descriptor 携带参数类型
  ↓（B 组同理需要 MemberInfo 托管模型）
终局  C 组 8 项转绿
```

**每修一层就会暴露下一层** —— 与 B5 修完后 preAssertionRaise 的下降一致
（35→31→33 中 C 组从 abort 转 NRE）。

### 移交状态

| 组 | 依赖 | 建议去向 |
|---|---|---|
| C（6 项） | 层1 newarr lowering + 层2 methods 表 + 层3 参数类型 | 本 roadmap 延伸或新立 |
| B（6 项） | MemberInfo 托管对象模型 | 新立（设计级） |
| A（2 项） | cdb 定位精确 abort 点 | debug-20-real-defects |


## Terminal Notes

- 本项是**调查结论**，非实现
- 关键判别：sentinel ≠ null —— 调用方的 `!` 挡不住，这是本链隐蔽的原因
