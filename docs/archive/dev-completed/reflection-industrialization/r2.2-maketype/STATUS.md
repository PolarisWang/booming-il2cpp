# R2.2 — MakeArrayType/MakeByRefType/MakePointerType

> **task_id**: r2.2-maketype
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

将 `ChaosTypeMakeArrayType`、`ChaosTypeMakeByRefType`、`ChaosTypeMakePointerType` 从伪句柄（硬编码 hash）实现为真实类型查找。通过 ModuleRegistry type_names 查找输入类型名 + 后缀（"[]", "&", "*"）的目标类型。

## Current State

当前 3 个函数均返回伪句柄 `0x8xxxxxxx | <hardcoded_hash>`，不处理输入 type：
```cpp
CHAOS_IL2CPP_INTPTR ChaosTypeMakeArrayType(CHAOS_IL2CPP_INTPTR type) noexcept {
    (void)type;
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(0x8000000000000000ULL | 35342034ULL));
}
```

## Implementation Plan

每个函数需要：
1. 解析输入 type 的 type name（从 ModuleRegistry type_names 或 descriptor subject_id）
2. 构造目标 type name（追加 "[]"/"&"/"*"）
3. 在所有 module 的 type_names 中查找匹配
4. 返回 MakeTypeHandle(module_id, token) 或 EncodeReflectionQueryTypeHandle

### 核心辅助函数

可以复用 R1.4 中的 `findTypeRefByName` lambda 模式（在 type_stubs.cpp 中已有）。

### 实现细节

```cpp
CHAOS_IL2CPP_INTPTR ChaosTypeMakeArrayType(CHAOS_IL2CPP_INTPTR type) noexcept {
    // 1. Try TypeRef path
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        const char* name = tr.module->type_names[tr.type_index];
        if (name != nullptr) {
            // Build "ElementType[]" and search
            // ...
        }
    }
    // 2. Try descriptor path
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        // Build "Namespace.Type[]" and search
        // ...
    }
    return 0;
}
```

### 回退策略

如果目标数组类型不存在（例如 element type 是尚未实例化的泛型参数），返回 0。managed 端需要处理 0 为"不支持"。

## Exit Criteria

- MakeArrayType 对已知数组类型返回真实 handle（如 int → int[]）
- MakeByRefType/MakePointerType 对已知类型返回真实 handle
- 未知类型回退返回 0
- chaos_runtime_core 构建通过

## Terminal Notes

### 执行概况

1. 将 `ChaosTypeGetElementType` 内部的两个 lambda（`findDescBySubjectId`、`findTypeRefByName`）提取为文件级 `static` 函数，位于 type_stubs.cpp 中 ChaosTypeGetUnderlyingSystemType 与 ChaosTypeGetElementType 之间。
2. `ChaosTypeGetElementType` 内部的 lambda 定义替换为直接调用 `FindDescBySubjectId` / `FindTypeRefByName`（驼峰命名）。
3. 三个 Make*Type 函数实现两阶段查找：
   - **Path 1 (ModuleRegistry TypeRef)**: 解析输入 type 的名称 → 追加后缀（"[]"/"&"/"*"）→ 在全部 module 的 type_names 中搜索 → 返回 `MakeTypeHandle(module_id, token)`。
   - **Path 2 (Descriptor/Fallback)**: 通过 `GetTypeDescriptorFromHandle` 获取 subject_id → 追加后缀 → 通过 `FindDescBySubjectId` 扫描 module images + aot_metadata::kAllTypes → 返回 `EncodeReflectionQueryTypeHandle(desc)`。
   - 两路径都失败时返回 0（managed 端处理回退）。
4. 构建验证：`chaos_runtime_core` 编译通过（RelWithDebInfo），仅有预存在的 linker 警告，无新增错误。

### 关键设计决策

- 三个函数各自拥有独立的 `static char s_buf[1024]`（非共享，避免跨函数线程安全风险）。
- 不使用硬编码 hash 回退：找不到时严格返回 0。
- 后缀构造逻辑与 `ChaosTypeGetElementType` 的逆向剥离逻辑完全对称。
