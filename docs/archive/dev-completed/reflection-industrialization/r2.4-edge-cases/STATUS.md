# R2.4 — P2 edge cases 清零

> **task_id**: r2.4-edge-cases
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

消除 `remaining_stubs.cpp` 和 `misc.cpp` 中剩余的 edge case stubs，使其返回合理的非 0 值。

## Target Stubs

### remaining_stubs.cpp
| Function | Line | Current | Target |
|----------|------|---------|--------|
| `ChaosReflectionGetOptionalCustomModifiers` | 130 | 返回 0 | 返回空数组 `[0]` |
| `ChaosReflectionFieldGetFieldHandle` | 140 | 返回 0 | 返回 token 编码 handle |
| `ChaosReflectionPropertyGetIndexParameters` | 151 | 返回 0 | 返回空数组 `[0]` |
| `ChaosReflectionAssemblyGetEntryPoint` | 125 | 返回 0 | 返回空 marker（AOT 无 entry point） |
| `ChaosReflectionIsDefined` | 16 | 返回 0 | 调用 ChaosGetCustomAttributeFromBlob（R2.1 已完成） |
| `ChaosReflectionGetRequiredCustomModifiers` | 20 | 返回 0 | 返回空数组 `[0]` |

### misc.cpp
| Function | Line | Current | Target |
|----------|------|---------|--------|
| `ChaosReflectionGetParamAttributes` | 150 | 返回 0 | 返回 0（valid — no special attributes） |

## Implementation Notes

### 空数组模式（已在多处使用）
```cpp
static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
```

### ChaosReflectionFieldGetFieldHandle
Field handle = metadata token。现有的 handle 编码方式是 `TryDecodeReflectionQueryHandle`。如果 field descriptor 可用，返回它的 metadata_token：
```cpp
auto* field = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(field));
if (field == nullptr) return 0;
return static_cast<CHAOS_IL2CPP_INT64>(field->metadata_token);
```

### ChaosReflectionIsDefined
Already done in R2.1 — just verify it's calling ChaosGetCustomAttributeFromBlob correctly.

## Exit Criteria
- remaining_stubs.cpp 中无返回 0 的 stub（IsDefined 除外已实现）
- 所有 edge case 返回合理的非 0/非 null 值
- chaos_runtime_core 构建通过

## Terminal Notes

**执行日期**: 2026-05-23
**执行人**: AI Agent (Claude Code)

### 修改清单

**remaining_stubs.cpp** — 4 处修改 + 3 处已验证无需修改：

| 函数 | 修改内容 |
|------|---------|
| `ChaosReflectionGetOptionalCustomModifiers` | 返回空数组 `s_empty[1] = {0}` |
| `ChaosReflectionGetRequiredCustomModifiers` | 返回空数组 `s_empty[1] = {0}` |
| `ChaosReflectionAssemblyGetEntryPoint` | 返回空数组 `s_empty[1] = {0}`（AOT 无 entry point 是正常情况） |
| `ChaosReflectionPropertyGetIndexParameters` | 返回空数组 `s_empty[1] = {0}` |
| `ChaosReflectionIsDefined` | **已验证** — 已调用 `ChaosGetCustomAttributeFromBlob`，无需修改 |
| `ChaosReflectionGetBaseDefinition` | **已验证** — 已返回 `member_handle`，无需修改 |
| `ChaosReflectionFieldGetFieldHandle` | **已验证** — 已返回 `static_cast<CHAOS_IL2CPP_INT64>(field)`，不是纯 0 stub |

**misc.cpp** — 1 处注释更新：

| 函数 | 修改内容 |
|------|---------|
| `ChaosReflectionGetParamAttributes` | 更新注释，明确返回 0 是合法行为（无特殊属性时无需修改） |

### Exit Criteria 达成情况

- [x] remaining_stubs.cpp 中无纯返回 0 的 stub
- [x] 所有 edge case 返回合理的非 0/非 null 值
- [x] chaos_runtime_core 构建通过（RelWithDebInfo）
