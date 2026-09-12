# P0-1 / P0-2 — Phase 0 覆盖矩阵建立

> **task_id**: reflection-p0-coverage-matrix
> **parent_task_id**: reflection-production-readiness
> **source_task_id**: reflection-production-readiness
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-09-12
> **完成日期**: 2026-09-12
> **entry_skill**: dev-writing-plans（轻量，父 roadmap 已提供充分上下文）

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

建立 `System.Reflection` 公开 API 的权威全集与三维判定矩阵，对齐现有 native 声明与 codegen 黑名单，产出 Phase 0 关键交付物：**总量数字**。

## Inputs

- 上游设计：`docs/dev/in-progress/reflection-production-readiness/design-v1-01.md`
- 上游 roadmap：`roadmap-v1-01.md`
- .NET 参考程序集：`Microsoft.NETCore.App.Ref/8.0.11/ref/net8.0`
- 现有 native 声明：`src/native/runtime-core/reflection_api.h`（82 个 Chaos* 符号）
- codegen 黑名单：`NativeReferenceProofEmitter.ReflectionPlatformCapabilityFamily.cs`（30 项）

## Expected Outputs

- `src/tools/Chaos.IL2CPP.Tools.ApiSurfaceScanner/`（新建工具，遵守 ATG 层边界：只产 `.cs`/`.json`）
- `api-coverage-matrix.json`（829 条目，每行有初判 tier）
- `api-surface-report.md`（总量与分类统计）

## Exit Criteria

- ✅ `Chaos.IL2CPP.Tools.ApiSurfaceScanner` 可构建、可运行
- ✅ `api-coverage-matrix.json` 生成，每行有初判 tier
- ✅ 总量数字明确公布
- ✅ 82 native 声明 + 30 黑名单已对齐
- ✅ `Reflection.Emit` 归 `not-applicable`（代码内 `NotApplicableTypePrefixes`）

## Terminal Notes

### 总量数字（Phase 0 关键产出）

| tier | 数量 | 说明 |
|---|---|---|
| `real` | 26 | 已有 native 实现（fact 档，性能未达标） |
| `not-supported` | 107 | 结构不可行，必须显式抛异常 |
| `unclassified` | 503 | **Phase 2 的真实工作面** |
| `enum-surface` | 193 | 纯枚举常量，排除出分母 |
| **分母** | **636** | 排除 enum-surface |

**总量：829 条目 / 636 操作的 API 分母。**

### 枚举目标

`System.Runtime.dll`、`mscorlib.dll`、`System.Reflection.dll`、`System.Runtime.Extensions.dll`（.NET 8 参考程序集）。命名空间过滤 `System.Reflection`（含子命名空间）。

### 口径决策（执行中发现并固化）

1. **枚举常量分离**：`BindingFlags`/`TypeAttributes`/`MethodAttributes` 等 22 个枚举类型共 193 个常量，是**元数据值定义**而非反射操作，tier 标 `enum-surface` 排除出分母。否则分母虚高 30%。
2. **`Reflection.Emit` 排除**：`NotApplicableTypePrefixes` 硬编码 `System.Reflection.Emit.` 前缀。但因参考程序集中 Emit 位于独立 DLL，实际命中 0 条——需注意 `System.Reflection.Emit` 的真实程序集未纳入本次枚举范围（见 Watch Items）。
3. **`System.Reflection.Metadata` 排除**：独立 DLL，有自身的 pipeline 追踪（见设计 §2.2 不覆盖项）。

### not-supported 判定（107 项）

| 类型 | 数量 | 理由 |
|---|---|---|
| `TypeDelegator` | 47 | 需要活的托管 Type 实现链 |
| `Assembly`（Load 系列） | 12 | 运行时程序集加载，AOT 无 |
| `IReflect` | 12 | COM 时代后期绑定接口 |
| `ConstructorInvoker`/`MethodInvoker` | 14 | 需 Delegate.CreateDelegate |
| `Binder` | 6 | 自定义模型绑定器 |
| `AssemblyName`（公钥系列） | 5 | 强名密钥材料不在 AOT 元数据中 |
| `Pointer`/`StrongNameKeyPair` | 8 | 需运行时装箱指针/密钥对 |
| 其他（MemberInfo/MethodBody） | 3 | GetCustomAttributes 任意属性物化 / IL 字节数组 |

### 工具层边界合规

`ApiSurfaceScanner` 位于 `src/tools/`（ATG 层），只产出 `.cs`/`.json`，不生成 C++。符合 CLAUDE.md 四层架构红线。

### 验收

- `dotnet build` 0 error 0 warning
- 矩阵生成且结构完整
- 分类分布经人工核查合理

## Watch Items（移交 Phase 2）

- ⚠️ `System.Reflection.Emit` 的真实程序集（`System.Reflection.Emit.dll` 等）**未纳入本次枚举**——它们不在 `Microsoft.NETCore.App.Ref` 的 ref/net8.0 目录中。Phase 2 需确认这些 API 是否需显式登记为 `not-applicable`。
- ⚠️ 503 个 `unclassified` 中可能有相当部分实际不可实现（如 `MethodBase.GetMethodBody` 的 IL 操作），Phase 2 判定时需重新评估 `not-supported` 边界。
- ⚠️ 26 个 `real` 是 fact 档，性能未达标（REF-RISK-1/2 未修），Phase 4 需重新审视。
