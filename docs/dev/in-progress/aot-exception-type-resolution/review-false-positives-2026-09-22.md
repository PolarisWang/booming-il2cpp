# 一轮 7 条 review 的误报复盘（3 条高/中 severity 全为误报）

> 日期：2026-09-22
> 被审对象：`src/native/runtime-core/core/object_creation.cpp` 的 `ResolveObjectTypeInfo` Path 1
> 引入该改动的提交：`b3d793b17`（"ResolveObjectTypeInfo Path 1 补 type_info 赋值 — tag 句柄不再返回 nullptr"）
> 关联任务：`aot-exception-type-resolution/`（异常类型解析链）

---

## 0. 一句话结论

外部 review 提出 7 条 finding，其中 3 条 🔴/🟠（#1 UAF、#3 死代码、#4 缺少编译期守卫）**全部为误报**，
且全部满足同一模式：**引用了源码中不存在的字面量或行为，并据此下结论**。

**若照这 3 条动手，会推翻 `b3d793b17` 刚修复的异常路径**——该提交的 commit message 已记录
修复前的崩溃链：`ResolveObjectTypeInfo` 不赋 `out_type_info` → `ObjectNew` 返回 nullptr
→ `RaiseManagedException` 的 `std::abort()` → SIGABRT → longjmp → `caught=true/value=0`。

**方法论产出**：已给 review 流程加「一手引用门槛」（见文末 §4）。

---

## 1. 三条误报的逐条拆解

### #1 🔴 "Path 1 无条件信任 desc->type_info_ptr ... 属 Use-After-Free"

**声称**：该指针指向 `RuntimeInstantiatedType` 描述符，而该描述符在
`UnregisterModuleGenerics`（`resolve_instantiate.cpp:137`）中被 `DOMAIN_CURRENT_FREE` 释放，
且来自 Domain 分配域而对象头属 GC 域 → 域卸载后对象头持有悬垂指针 → UAF。

**实际**：**把两个同名但不同宿主的 `type_info_ptr` 当成了同一个东西。**

| | 被读的是 | 被释放的是 |
|---|---|---|
| 宿主 | `ReflectionQueryTypeDescriptor`（`reflection_query_model.h:128`） | `RuntimeInstantiatedType`（堆记录，`instantiation_engine.h:154`） |
| 关系 | `RuntimeInstantiatedType` 内嵌一个 `descriptor`，但**没有代码写过它的 `type_info_ptr` 字段** | `resolve_instantiate.cpp:137` 释放的是整个 `rt` 记录 |

实际取值来自 `BuildExceptionTypeTable`（`ModuleRegistration.Dispatch.cs:913-916`）：

```csharp
sb.Append("    nullptr, 0u, 0u, &")
  .Append(GetNativeMethodTableSymbol(bare))   // = "chaos_mt_" + subjectId
  .AppendLine(" };");
```

按 `reflection_query_model.h:102-128` 的字段顺序，第 4 个字段正是 `type_info_ptr`。
**该指针指向静态 `constexpr MethodTable`（`chaos_mt_*`，定义在同一 TU），是静态存储，从不被 `DOMAIN_CURRENT_FREE` 触及。**

**判定**：误报。无 UAF；域卸载场景不成立。

> 附：review 还称 `ModuleRegistration.cs:1080` 是描述符字段赋值——实为往 `_extraMethodTableSymbols` 塞符号名，非字段赋值。

---

### #3 🟠 "异常表描述符的 type_info_ptr 恒为 nullptr，整段新分支是死代码"

**声称**：模板块是 `` `nullptr, 0u, 0u, &>` ``，"异常表版本只写了 0u 常量"，因此 `desc->type_info_ptr` 恒为空，
新分支对注释点名的唯一场景是死代码；真正生效的是模块反射描述符，与注释所述不符。

**实际**：**review 引用的模板串在源码中不存在。**

`ModuleRegistration.Dispatch.cs:913-916` 是 `"    nullptr, 0u, 0u, &" + GetNativeMethodTableSymbol(bare)`，
即展开为 `nullptr, 0u, 0u, &chaos_mt_XXX };`。代码里没有任何地方存在 `&>` 这样的串。

**方向恰好相反**：异常表描述符的 `type_info_ptr` **恒非 nullptr**——这正是 `b3d793b17` 得以生效的原因，
也与 `exception_helpers.cpp:113-115` 的注释一致（`ResolveTypeByName` 返回的正是 `kExcDesc` 的 tag 句柄）。

**判定**：误报，且结论方向被颠倒。据此"修复"会破坏当前唯一有效的异常类型解析路径。

---

### #4 ⚪ "未加任何编译期守卫，MethodTable 重排会静默读错"

**声称**：把 MethodTable 当 TypeInfoHot 用，兼容性由 `type_info.h` 的断言保证，但**新增代码未加守卫**；
一旦 warm 段重排，异常对象的 GC 扫描会静默读错字段。

**实际**：双重不成立。

1. **守卫已经存在**且就在同项目头文件：`type_info.h:238-251` 有 7 条 `offsetof` 断言（`parent_mt`/`vtable_array`/
   `stable_id`/`vtable_length`/`warm_delta`/`type_shape`/`flags` 逐一比对 TypeInfoHot），另有 `:234` 的 `sizeof(MethodTable)==64`。
   这些断言在**全局**生效，不需要在新代码处重复。
2. **这里根本没有发生 reinterpret 带来的布局依赖**。`chaos_mt_X` 本身就是 `MethodTable` 对象，
   `static_cast<const TypeInfoHot*>` 只是取同一地址（`AsTypeInfoHot()` 是同一操作的具名封装）。

`type_info.h:217` 的 Phase 1+ 注释被引用来支持该假设，但 **Phase 1+ 改的是 warm 段**（加 eeclass 指针），
hot 段偏移被上述 7 条断言钉死——想动就编译不过，不存在"静默读错"。

**判定**：误报。

---

## 2. 成立的条目（已处置）

| # | 声称 | 判定 | 处置 |
|---|------|------|------|
| #2 | `stable_id == 0` 静默降级、无 trace | 部分有效（"调用方未检查返回值"不实——`ObjectNew:72-73` 通过 out 参数检查了） | 两个降级路径各补 `CHAOS_IL2CPP_LOG_WARN_M` |
| #5 | Path 1 不强制 vtable 校验而 Path 2 强制，无注释 | 有效（低） | 补注释说明是刻意放宽 |
| #6 | 与 `type_registry.cpp:191-202` 语义假设分叉 | 有效 | 更新过期注释 |
| #7 | `return (out_type_info != nullptr)` 恒真 | 有效 | 改为 `return true` |

---

## 3. 误报模式（可复用的识别特征）

三条误报共享一个特征：**引用的原文/行为，在源码中无法逐字找到**。

| # | 引用的东西 | 实际 |
|---|-----------|------|
| #1 | 把 `A::type_info_ptr` 当成 `B::field` | 两个不同宿主的同名字段 |
| #3 | 模板串 `` `&>` `` | 源码中不存在此串 |
| #4 | "缺少守卫" | 守卫就在同项目头文件里 |

**推论**：引用错误 ⇒ 审查者**没有真的读那段代码** ⇒ 其严重性判定不携带信息量，方向可能是反的。

---

## 4. 已加的流程护栏

针对**产出端**与**接收端**各加一道闸：

| 端 | 文件 | 内容 |
|----|------|------|
| 产出端（reviewer 子 Agent 提示模板） | `.ai/skills/library/skills/dev-requesting-code-review/code-reviewer.md` | 新增「🔴 一手引用门槛」：每条 finding 必须附 `文件:行号` + 原文摘录；列出 5 类必须逐字验证的断言（"某处写的是 X" / "死代码" / "缺少守卫" / "UAF" / "未检查返回值"）；同名符号必须验明宿主；每条需用固定格式（位置/原文/问题/修复） |
| 接收端 | `.ai/skills/library/skills/dev-receiving-code-review/SKILL.md` | 新增「第 6 项：先核对引用，再讨论对错」：按「严重性 × 引用质量」分档；引用不成立 → 整条作废；含"误报代价不对称"论证与 4 问自检清单；速查表补 2 行 |

**核心原则**：**对已修复代码发起的反向修改，比漏掉一个真问题更贵。**
真问题会通过 fact/测试/崩溃再次浮现；误报驱动的改动发生在没有失败信号时，
且因编译照样通过，直到很久以后才暴露。

---

## 5. 未做

- **未为 Path 1 补回归 fact**（原计划的 B 项）。`b3d793b17` 的 `regression_check` 只写了
  「text-json build 阶段通过」——**仅编译通过，未验证异常路径运行时行为**（属"编译期成立 ≠ 运行时等价"）。
  这是本任务链上仍存在的验证缺口，见 `root-cause-complete.md` 的层 2-4 未修状态。
