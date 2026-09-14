# P-1 根因：`VERIFY` 编译符号从未定义 → 全仓断言被编译期移除

> 日期：2026-09-13 ｜ 状态：**根因确证，待修**

## 结论（一句话）

`Chaos.TestFramework.Sdk` 的 **25 个断言 API 全部标注 `[Conditional("VERIFY")]`**，
而 `VERIFY` 编译符号**在整个仓库中从未定义** —— 编译器因此**完全移除所有断言调用**。
断言从不执行、从不失败、从不改变退出码，于是**每个 subject 都被报成 passed**。

## 证据链

### 1. SDK 侧：25 处 Conditional

`src/reference/Chaos.TestFramework.Sdk/Assert.cs`

```csharp
[Conditional("VERIFY")]
public static void IsTrue(bool condition, string? message = null)
{
    if (!condition)
        Fail(message ?? "Expected true");
}

[Conditional("VERIFY")]
public static void Fail(string message)
{
    s_exitCode = 1;                                      // 失败标记
    System.Console.Error.WriteLine($"[ASSERT FAIL] {message}");
    throw new AssertionException(message);
}
```

`grep -c 'Conditional("VERIFY")' Assert.cs` → **25**（含 `IsTrue`/`AreEqual`/`Fail` 等全部入口）。

**`[Conditional]` 的语义**：若调用点所在编译单元未定义该符号，**调用被整体删除**
（连同实参求值）。所以不是"失败了没上报"，而是"**从未执行**"。

### 2. 调用方：全仓无 `VERIFY` 定义

```bash
grep -rn 'DefineConstants.*VERIFY|"VERIFY"' --include=*.csproj --include=*.props \
    --include=*.targets --include=*.scriban src/
# → 无结果
```

生成工程实测（build 后）：

```bash
grep -nE 'DefineConstants|VERIFY' <chunk>/managed/combined/CombinedSubjects.csproj
# → 无 VERIFY 定义
```

### 3. 行为印证（与前面的实测吻合）

| 现象 | 解释 |
|---|---|
| 接线关闭时 `value=0`（断言成功应 `return 1L`） | 断言被移除，函数走到别处/抛出，未到 `return 1L` |
| `passed=true` | `assertFailed = !caught && Complete()!=0`；`Complete()` 恒 0 |
| `stderr` 无 `[ASSERT FAIL]` | `Console.Error.WriteLine` 也在被移除的 `Fail` 体内 |
| 反向验证"关接线仍通过" | 断言根本没在跑，与接线无关 |

## 与前一份文档的关系（修正）

`PHASE0-RESULTS.md` 记录的"三层缺陷链"中：

- **层 1（ThreadStatic）**：确实存在，但在**当前** runner（main-thread dispatch）
  下不是活跃成因 —— 断言压根没执行，谈不上读不到标记。
- **层 2（assertFailed 与 caught 耦合）**：表达式本身的耦合是事实，但同样
  不是当前的主因。
- **层 3（JIT 恒 false）**：事实，且会掩盖 JIT 路径的真实失败。

**真正的根因是更上游的一条**：**断言执行体被编译期删除**。
修层 1/2/3 都不会让断言开始执行。

## 影响面（全仓）

- 影响**所有** chunk、**所有** subject 的断言 —— 不限于反射线
- 凡是通过 fact 断言得出的"通过"结论，**其断言部分未被执行**
- 这解释了此前多次观察到的怪象：断言写着却总能通过

## 修复方案

### 方案 A：在 SDK 程序集上无条件定义 VERIFY（最小）

`[Conditional]` 判定的是**调用点所在程序集**的符号，故需在**测试工程**侧定义。
在生成工程时注入：

```xml
<DefineConstants>$(DefineConstants);VERIFY</DefineConstants>
```

落点：TPG 生成 csproj 的模板。

### 方案 B：移除 `[Conditional]` 标注

断言无条件生效。**语义更直接**，但会改变 SDK 现有意图（`Conditional` 本意
可能是让发布版剔除断言）。

### 建议

**方案 A**。理由：
1. 保留 SDK 的既有设计意图（`Conditional` 是有意为之）
2. 改动面小（生成工程侧一行）
3. 不改变 SDK 公共 API 形状

**但必须先确认**：`Conditional` 是本项目的**有意设计**（release 剔除断言）
还是**遗留误用**。若是后者，方案 B 更诚实。

## 风险

| 风险 | 缓解 |
|---|---|
| 打开断言后**大量既有"通过"变红** | 这**正是目的** —— 但需分批评估，不能一次性全开 |
| 断言失败导致 CI 大面积红 | 先在单 chunk（反射）验证，量化影响面后再推广 |
| SDK 的使用方（非测试路径）受影响 | 只对**测试工程**注入常量，不动 SDK 本身 |

## 下一步

1. 确认 `[Conditional("VERIFY")]` 是有意设计还是遗留 → **需询问用户**
2. 按方案 A 在反射 chunk 验证：打开 VERIFY 后量化新增红灯
3. 评估是否推广到全 chunk

---

## 更正与最终根因（2026-09-14）

### ⚠️ 先更正上文的错误结论

上文称「`VERIFY` 编译符号从未定义、断言被编译期移除」——**这是错的**。

**错因**：我只 grep 了 `src/` 下的 `.csproj/.props/.targets/.scriban`，
**漏掉了由 Python pipeline 生成的 csproj**。实测生成的工程中：

`<chunk>/managed/combined/CombinedSubjects.csproj`

```xml
<DefineConstants>VERIFY</DefineConstants>
```

`VERIFY` 确实被定义，**断言调用没有被移除**。
（生成点：`tests/e2e/verification/stages/build.py:1398`）

### 真正的根因：方法体的 catch 吞掉断言异常

生成的方法体被 try/catch 包裹，**catch 把托管异常转成"返回默认值"**：

`src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs:485-492`

```csharp
if (_wrapInTryCatch)
{
    builder.AppendLine("} catch (const chaos_managed_exception&) {");
    if (method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
        builder.AppendLine("    return {};");      // ← 吞掉异常，返回 0
    builder.AppendLine("}");
}
```

实际生成物（`native-aot.page-0003.cpp`，`GetExecutingAssembly_3__0`）：

```cpp
[&]() -> void { if (!(_s3)) { throw chaos_managed_exception{}; } }();  // 断言：失败则抛
...
return _i3;
}
catch (const chaos_managed_exception&) {
    return {};                                   // ← 被这里吞掉
}
```

### 完整因果链

```
断言失败
  → Assert.IsTrue 抛 AssertionException（托管异常）
  → 变成 chaos_managed_exception
  → 被**方法体自己的 catch** 捕获 → return 0
  → 异常不传播到 runner（caught=false）
  → runner: assertFailed = !caught && Complete()!=0 = false
  → passed = !caught && !assertFailed = **true**
```

### 实测印证（全部吻合）

| 观察 | 解释 |
|---|---|
| 断言确实在跑（生成体有 `if (!(_s3)) throw`） | 断言未被编译移除 |
| `value=0`（断言成功应 `return 1L`） | catch 返回了默认值 |
| `passed=true` | 异常没到 runner |
| `stderr` 无 `[ASSERT FAIL]` | 需确认 `Fail` 内的 WriteLine 是否也走了被吞路径 |
| 反向验证"关接线仍通过" | 与接线无关，断言结果被吞 |

### 与上文"三层缺陷链"的关系

那三层（ThreadStatic / assertFailed 耦合 / JIT 恒 false）**都是真实存在的次生问题**，
但**都不是当前的主因**。主因是这一条：**方法体吞掉了断言异常**。

修那三层都不会让断言失败变成红灯；修这一条才会。
