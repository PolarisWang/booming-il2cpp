# Step A Spike — AOT 异常类型能否匹配？

> 日期：2026-09-20
> 问题：`TestEmitter.cs:458-472` 的 `catch { }` 不验类型，能否改成类型校验？

## 结论：✅ 技术上可行

### 证据 1：AOT 异常对象带真实 type_info

`exception_helpers.cpp:145-188` 的 `RaiseManagedException`：

```cpp
const auto type_handle = ResolveTypeByName(type_full_name);   // 按名解析类型
abi->class_init(runtime, type_handle);
auto* exc = abi->object_new(runtime, thread, type_handle);    // ← 创建真实托管对象
...
abi->raise_managed_exception(runtime, thread, exc);
```

**异常对象是 `object_new(type_handle)` 创建的**，对象头带正确的 `type_info`。
（`ChaosReflectionSetExceptionMetadata` 只设 message，类型来自对象头。）

### 证据 2：类型匹配机制存在且已在使用

生成体里的 catch 已经是：
```cpp
CHAOS_EH_CATCH_BEGIN
    if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ,
                             chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
    { CHAOS_EH_RETHROW; }
```

**`chaos_eh_match_type` 机制在跑**，只是 ATG 生成的是匹配 `System_Object`（任何异常）。

### 证据 3：ATG 已有类型化断言路径

`TestEmitter.cs:539`：
```csharp
Assert.Throws<{exType}>(() => {callExpr});   // exType 来自 probe 的 ExceptionType
```

**这条路径已存在**，只是 JSON writer **到不了**。

## 根因：走错了分支

`TestEmitter.cs:456-472`：

```csharp
if (result.HasException && result.ExceptionType is not null)
{
    if (isExternalAssembly && ExternalStubRaisesManagedException(method))
    {
        // ← JSON writer 走这里：try { call; throw; } catch { }  不验类型
    }
    else
    {
        Assert.Throws<{exType}>(...);    // ← 类型化断言
    }
}
```

`ExternalStubRaisesManagedException`（`:807`）的列表（`:985-1030`）**包含
`Utf8JsonWriter`** → 所以走弱断言分支。

该分支的注释自承：
> *"The AOT side raises a bare `chaos_managed_exception` without a managed type
> attached, so the exact exception class cannot be matched"*

**这条注释已过时** —— 证据 1 显示异常对象带完整 type_info。

## Step A 的修法

**把 `catch { }` 改为类型校验**。两种实现路径：

| 路径 | 做法 | 风险 |
|:-----|:-----|:-----|
| **A1** | 让该分支也生成 `Assert.Throws<{exType}>` | 需确认 `Assert.Throws<T>` 在 AOT 下能匹配类型 |
| **A2** | 保持 try/catch 结构，但在 catch 里断言类型 | 需 ATG 生成类型检查代码 |

**推荐 A1**（复用已有路径，改动最小），但**必须先验证** AOT 下的
`Assert.Throws<T>` 真能匹配（而非只匹配 `Exception`）。

### ⚠️ 验证方式（下一步）

构造一个 spike：让某个 JSON writer subject 走 `Assert.Throws<InvalidOperationException>`，
跑 pipeline 看：
- 若通过 → A1 可行
- 若失败（异常类型不匹配）→ 降级 A2 或消息匹配

**核心风险**：不能只验证"能通过"，还要验证"错误类型会失败"（防假绿）。

---

## 🔴 关键发现：`Assert.Throws<T>` 路径**从未被使用过**

### 证据

| 检查 | 结果 |
|:-----|:-----|
| `Assert.cs:170-176` 的 `Throws<T>` 实现 | ✅ 存在，做**精确类型匹配**（`catch (T) { }` + 类型不符 `Fail`） |
| `TestEmitter.cs:539` 生成 `Assert.Throws<{exType}>` | ✅ 代码存在 |
| **实际生成的 `CombinedSubjects.cs` 里 `Assert.Throws<` 出现次数** | ❌ **0** |
| **`artifacts/` 里的泛型 Throws 符号** | ❌ **无**（只有非泛型 `Assert__Throws_System_Void_System_Action_`） |

**结论**：`TestEmitter.cs:539` 的类型化断言路径**从未被实际发射过** ——
所有异常断言都走了 `:458-472` 的弱断言（`catch { }`）。

### 对 Step A 的影响

原本的 A1 方案（"让 JSON writer 改走 `Assert.Throws<T>`"）**不再是最小改动** ——
因为那条路径**未经 AOT 验证**，可能：
- 符号未注册（shape 缺失）
- 或 `catch (T)` 在 AOT 下无法做类型匹配

**必须先验证该路径在 AOT 下能否工作**，才能决定 Step A 的实现方式。

### 下一步验证（最小实验）

1. 手工构造一个 subject，让它调用 `Assert.Throws<InvalidOperationException>`
2. 跑 pipeline，看：
   - 编译是否通过（符号是否可解析）
   - 类型匹配是否生效（**且错误类型会失败** —— 防假绿）

---

## 🎯 最终结论：`Assert.Throws<T>` 在 AOT 下**未实现**

### 决定性证据

`RuntimeHelperShapeRegistry.CoreStubs.Part3.S23.cs:1523`：

```csharp
// Assert.Throws<T>(Action) remains unimplemented (requires
// recognizing the callvirt Invoke delegate pattern and emitting a
// try/catch with a type check).
```

**代码自承**：该 shape 未实现，且指明了实现所需的两个条件：
1. 识别 `callvirt Invoke` 委托模式
2. 发射带类型检查的 try/catch

### 这解释了所有观察

| 观察 | 解释 |
|:-----|:-----|
| `CombinedSubjects.cs` 里 `Assert.Throws<` 出现 **0 次** | ATG 生成了，但 AOT 侧无 shape → 会失败/被跳过 |
| `artifacts/` 里无泛型 Throws 符号 | 同上 |
| 所有异常断言都走 `catch { }` | 那是**唯一可用**的路径 |

### Step A 方案修正

| 原方案 | 修正后 |
|:-------|:-------|
| ~~A1：让 JSON writer 改走 `Assert.Throws<T>`~~ | ❌ **路不通** —— 该 shape 未实现 |
| **A1'：实现 `Assert.Throws<T>` 的 AOT shape** | ✅ 这是真正要做的工作 |
| A2：保持 try/catch，在 catch 里断言类型 | ✅ 备选，改动更局部 |

### A1' 的实现要求（来自代码注释）

1. **识别 `callvirt Invoke` 委托模式** —— ATG 生成的
   `Assert.Throws<T>(() => callExpr)` 展开后是 lambda + 委托调用
2. **发射带类型检查的 try/catch** —— 即用 `chaos_eh_match_type` 匹配具体类型

### 对计划的影响

**Step A 的工作量被低估了** —— 它不是"改个分支"，而是**实现一个新的 codegen shape**。

**建议**：
- 若实现 A1' 成本过高 → **改走 A2**（保持 try/catch 结构，在 catch 里做类型断言）
- A2 的 ATG 侧改动：把 `catch { }` 改为
  `catch (InvalidOperationException) { } catch { throw new Exception("wrong type"); }`
  —— **不需要新的 codegen shape**，因为 C++ 的 catch 类型匹配由 `chaos_eh_match_type`
  承载，而该机制**已在用**（见 §证据 2）
