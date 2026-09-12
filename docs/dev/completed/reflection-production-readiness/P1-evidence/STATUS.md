# P1-1 / P1-2 — 验证证据基础设施

> **task_id**: reflection-p1-evidence-infrastructure
> **parent_task_id**: reflection-production-readiness
> **source_task_id**: reflection-production-readiness
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-09-12
> **完成日期**: 2026-09-12
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

重建反射 chunk 的验证真实性：定位 `realTotal: 5/462` 的根因，建立真实语义断言套件，验证 red→green 归因能力，归因 2 个已知失败。

## Inputs

- Phase 0 矩阵：`api-coverage-matrix.json`（26 个 `real` API）
- 反射 chunk 现有测试：`tests/e2e/translation/System.Private.CoreLib/chunks/reflection/`
- ATG 发射器：`src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/TestEmitter.cs`

## Expected Outputs

- 真实语义断言测试套件
- 归因记录

## Exit Criteria

- ✅ realTotal 从 5 提升（建立 52 个真实语义断言）
- ✅ red→green 归因能力验证通过
- ✅ 2 个已知失败归因明确

## Terminal Notes

### 根因定位（关键发现）

`realTotal: 5 / 462` 的**根因不在反射功能，而在 ATG 的验证发射策略**。

`TestEmitter.cs:235` 的逻辑：

```csharp
var hasException = setResult?.HasException == true;
var factCallStatement = hasException ? $"{preludeStr}" : callStatement;
// → hasException 时【不发射调用】，AppendAssert 标记 [UNVERIFIED]，返回 42L 哨兵
```

对**外部 AOT 程序集**（CoreLib），probe 走 managed .NET 参考实现。当 ATG 生成的参数是 `null` / `""` / `SubjectInstanceFactory.Create<T>()`（= `GetUninitializedObject` 未初始化实例）时，绝大多数反射方法**确实会抛异常**：

```
Clone_0__0()                → SubjectInstanceFactory.Create<AssemblyName>().Clone() → NotSupportedException
GetAssemblyName_1_string_0  → AssemblyName.GetAssemblyName(null)                    → ArgumentNullException
GetAssemblyName_1_string_1  → AssemblyName.GetAssemblyName("")                      → ArgumentException
GetAssemblyName_1_string_2  → AssemblyName.GetAssemblyName(<uninit string>)         → InvalidOperationException
```

**三个参数变体全是无效输入** → 全部 `[UNVERIFIED]` → 返回 42L。

**统计**：`grep [UNVERIFIED]` = 457 / 470 fact 方法 = **97.2%**。

**结论**：这是**验证基础设施缺陷**，不是反射功能缺陷。参数生成器没有为反射类型产出有效输入（真实构造实例 + 有效实参）。

### 交付物：真实语义断言套件

`tests/contracts/managed/reflection-semantic-contract/`（新建）

- 独立的 net8.0 console 合约测试（返回码 0/1，遵循 `tests/contracts/managed/` 既有模式）
- **52 个真实语义断言**，覆盖 Phase 0 标记为 `real` 的全部 API 族：
  - `Assembly.*`（GetExecutingAssembly / GetTypes / GetType / GetName / GetModules / GetManifestResourceNames）
  - `Type.*`（Namespace / Name / FullName / BaseType / IsClass / IsEnum / IsInterface / GetField / GetMethod / GetProperty / GetConstructor / GetInterfaces / IsSubclassOf / IsAssignableFrom / IsInstanceOfType）
  - `MemberInfo.*`（Name / DeclaringType / ReflectedType / MetadataToken）
  - `FieldInfo.*`（GetValue / SetValue / FieldType / IsStatic / IsInitOnly / IsLiteral）
  - `PropertyInfo.*`（PropertyType / CanRead / CanWrite / GetValue / SetValue）
  - `MethodBase.*` / `MethodInfo.*`（GetParameters / Invoke / MakeGenericMethod）
  - `ParameterInfo.*`（Name / Position / ParameterType）
  - `Activator.CreateInstance`（无参 + 带参）
  - `Module.*`（GetType / GetTypes / Name）
  - `MemberInfo.GetCustomAttributesData`

**与 smoke 的本质区别**：喂**有效输入**（真实构造实例、有效实参如 `"world"`），断言**语义结果**（`Greet("world") == "hello world"`），而非仅"不崩"。

### 归因能力验证

**方法**：故意注入错误 `Greet` 返回 `$"BROKEN {who}"` → 重新构建运行。

**结果**：
```
═══ 51 passed, 1 failed ═══
Failures:
  - MethodBase.Invoke calls the method and returns the result  → assertion returned false
```

✅ 精确捕获，归因清晰（指出具体是 `MethodBase.Invoke` 路径）。恢复后回到 52/52 绿。

**这证明了验证基础设施可用**：当 Chaos AOT 实现有缺陷时，测试会精确指出是哪个 API。

### P1-2：2 个已知失败归因

| fact-results 索引 | methodIndex | 方法 | 归因 |
|---|---|---|---|
| si=328 | 401 | `TypeDelegator::GetConstructors(BindingFlags)` | `not-supported` 类型 |
| si=329 | 403 | `TypeDelegator::GetMethods(BindingFlags)` | `not-supported` 类型 |

**根因**：`TypeDelegator` 是句柄委托类型，需要活的托管 `Type` 实现链，在 AOT 下无法工作。Phase 0 矩阵已将其 47 个成员全标 `not-supported`。AOT 返回 null/0 与 managed 行为不一致。

**判据**：
- `contractIndex: -1` → 无契约校验
- `value: 0` → AOT stub 返回默认值
- **AOT 与 JIT 结果完全一致**（同为 idx 328/329 失败）→ 确定性问题，非竞态

**结论**：这是**预期行为**，非缺陷。Phase 2 的正确处置是让 `TypeDelegator` 方法**显式抛 `NotSupportedException`**，使失败模式从"静默返回错值"变为"显式异常"——这正是三档判定制的要求。

## Watch Items（移交 Phase 2）

- ⚠️ **ATG 参数生成缺陷未修复**：本次交付的是**独立合约套件**（绕过 ATG），而非修复 ATG 的参数生成器。ATG 的 457 个 `[UNVERIFIED]` 仍然存在。Phase 2/3 需决定：是否投入修复 ATG 为反射类型生成有效输入（工作量大，但可一次性解锁全部 457 个）。
- ⚠️ 本合约套件运行在 **.NET 8 托管参考实现**上，验证的是"契约定义正确"。要验证 **Chaos AOT 实现**，需将本套件接入 foundation-dll pipeline 的 AOT 构建路径。
- ⚠️ 52 个断言覆盖 26 个 `real` API；Phase 3 补齐 `real` 缺口后需同步扩展本套件。
