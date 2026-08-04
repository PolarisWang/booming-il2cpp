# gc-iw-b1：并发根扫描接入点集成（C6）实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成时先归档本子任务，再同步父任务。

**目标：** 在 codegen 发射的 `stsfld`（静态字段写入）路径中增加 `BgcSatbPreWriteBarrier` 和 `BgcRecordRootChange`，消除 BGC 并发标记期间静态字段 root slot 覆盖导致的对象丢失风险。

**架构：** 现有 GCHandle 写入和 interpreter 的 Handle_StSFld 已有 root change barrier，但 codegen 的 `stsfld` 发射路径缺失。在 ExceptionEmission.cs 的 `case "stsfld"` 块中增加屏障调用。

**技术栈：** C# codegen emitter (NativeAotLoweringPlanner.ExceptionEmission.cs)

**架构审核模式：** critical（触及 codegen 输出和 AOT 正确性主线）

**结构告警重点：** codegen 与 Scriban 分层；重复 emitter 逻辑。

**权责图审核主题：** codegen emission 确保 GC root write barrier 与 interpreter 对等。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: System.Private.CoreLib/garbage-collection
- capabilityItem: concurrent-root-scan-entry-points
- ownerSubjectId: C6
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: [codegen output validation, foundation-dll GC subject fact verification]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（设计已在 STATUS.md 中记录）

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a（单点修复，不影响架构知识）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 背景

C6 差距项分析：

| 路径 | BgcSatbPreWriteBarrier | BgcRecordRootChange | 状态 |
|------|:---------------------:|:-------------------:|:----:|
| Interpreter Handle_StSFld | ✅ 有 | ✅ 有 | 正确 |
| GcSetHandleTarget (GCHandle) | ✅ 有 | ✅ 有 | 正确 |
| GcSetDependentHandleSecondary | ✅ 有 | ✅ 有 | 正确 |
| **Codegen stsfld (ExceptionEmission.cs)** | ❌ 无 | ❌ 无 | **GAP** |

BGC 的 `StwRemark()` 已调用 `BgcDrainRootChangeBuffer` 为所有线程 drain root change，因此只要写入端正确调用 `BgcRecordRootChange`，remark 阶段就能重新标记被覆盖 root slot 的旧值。

## 修改文件

### `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs`

**行 1026-1038**，`case "stsfld":` 块。当前代码：

```csharp
case "stsfld":
{
    var targetRef = GetRequiredTargetReference(instruction);
    if (targetRef.Kind != AotCoreIrReferenceKind.Field)
        throw new NotSupportedException(...);
    builder.AppendLine($"{indentation}{{");
    builder.AppendLine($"{indentation}    auto chaos_value = {ConsumeEvalStackValueExpression()};");
    EmitStaticInitializationForField(builder, targetRef.SubjectId, indentation);
    builder.AppendLine($"{indentation}    {GetNativeStaticFieldSymbol(targetRef.SubjectId)} = chaos_value;");
    builder.AppendLine($"{indentation}}}");
    break;
}
```

需要改为：

```csharp
case "stsfld":
{
    var targetRef = GetRequiredTargetReference(instruction);
    if (targetRef.Kind != AotCoreIrReferenceKind.Field)
        throw new NotSupportedException(...);
    builder.AppendLine($"{indentation}{{");
    builder.AppendLine($"{indentation}    auto chaos_value = {ConsumeEvalStackValueExpression()};");
    EmitStaticInitializationForField(builder, targetRef.SubjectId, indentation);
    // SATB pre-write barrier: record old value before overwriting the slot.
    // Static fields are GC roots; during BGC concurrent mark, overwriting a
    // root slot may lose the only reference to an object.
    string fieldSymbol = GetNativeStaticFieldSymbol(targetRef.SubjectId);
    builder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&{fieldSymbol}));");
    builder.AppendLine($"{indentation}    if (chaos::il2cpp::runtime_core::g_bgc_is_marking.load(std::memory_order_relaxed)) {{");
    builder.AppendLine($"{indentation}        chaos::il2cpp::runtime_core::BgcRecordRootChange(reinterpret_cast<void**>(&{fieldSymbol}), reinterpret_cast<void*>({fieldSymbol}));");
    builder.AppendLine($"{indentation}    }}");
    builder.AppendLine($"{indentation}    {fieldSymbol} = chaos_value;");
    builder.AppendLine($"{indentation}}}");
    break;
}
```

关键决策：
- `BgcSatbPreWriteBarrier`：无条件调用（有内部 fast-path 检查 `g_bgc_is_marking`，非 marking 时单条 load + branch 返回）
- `BgcRecordRootChange`：用 `g_bgc_is_marking` 守卫，避免非 BGC 周期的 fetch_add 开销。这与 interpreter 路径不同（interpreter 无条件调用），但 codegen 路径调用频率更高（每个 stsfld 发射点都调用），守卫必要
- 对引用类型和值类型都加：`BgcSatbPreWriteBarrier` 在 nullptr 时快速返回；`BgcRecordRootChange` 在非 marking 时跳过。对原始类型（int/bool 等）的静态字段写入，`BgcSatbPreWriteBarrier` 读出的 `*slot` 是一个非 object 指针值，但 `BgcSatbPreWriteBarrier` 会在 `g_bgc_is_marking` 为 false 时直接返回，不会误读 slot

## 验证

1. 编译 codegen 项目（`dotnet build Chaos.IL2CPP.Generator`），确认编译通过
2. 运行 GC foundation-dll verification pipeline，确认 `garbage-collection` family 全部 10 subjects 仍然 10/10 pass
3. 检查 codegen 输出中的 `stsfld` 写入是否包含 `BgcSatbPreWriteBarrier` + `BgcRecordRootChange`

## 任务

- [ ] 修改 `ExceptionEmission.cs` 的 `case "stsfld"` 块，增加写入屏障
- [ ] 编译验证 + foundation-dll pipeline 验证
- [ ] 结构告警与架构审视
- [ ] 归档 completed + 更新父 roadmap
