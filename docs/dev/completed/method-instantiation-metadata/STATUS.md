---
task_id: method-instantiation-metadata
title: 方法泛型实例化 Metadata — RuntimeInstantiatedMethod + 注册/查询 API
task_type: plan
lifecycle_status: completed
phase: B
created_at: 2026-05-04 20:00:00 +08:00
updated_at: 2026-05-04 20:00:00 +08:00
parent_task_id: 20260504-generic-runtime-instantiation-phases-3-5
source_task_id: roadmap-v1-01.md
source_relation: roadmap-child
clearance_source: parent-roadmap
---

## Scope

实现 `resolve_or_instantiate_method` 返回可查询的 MethodInfoHandle。

**边界**：
- 只做 metadata 注册/查询，不做 IL 执行（留给 Phase 5b）
- MethodInfoHandle 使用与 TypeInfoHandle 相同的 tag-encoding 模式
- 调用已实例化的方法时返回错误 "runtime method body execution not yet available"

## 交付物

- `src/native/runtime-core/generic_method_instantiation.h` (new)
- `src/native/runtime-core/generic_method_instantiation.cpp` (new)
- 修改 `generic_context.h` — `RegisterGenericMethodInstantiation`, `TryResolveClosedMethod`
- 修改 `generic_context.cpp` — 注册/查询/模块卸载清理
- 修改 `runtime_instantiation.cpp` — `ResolveOrInstantiateMethod` 真实实现
- 修改 `CMakeLists.txt` — 添加新文件
- `tests/unit/compatibility/test_method_instantiation.py` (new, 38 tests)

## Exit Criteria

- ✅ 38/38 兼容性测试通过
- ✅ `resolve_or_instantiate_method` 返回 valid MethodInfoHandle（不再返回 nullptr）
- ✅ 方法泛型的 Registry 注册和查询（TryResolveClosedMethod）正确
- ✅ 模块卸载时方法泛型条目正确清理
- ✅ CMakeLists.txt 包含新文件

## 设计要点

1. **RuntimeInstantiatedMethod**: 堆分配的闭包方法描述符，嵌入 ReflectionQueryMethodDescriptor，支持 EncodeReflectionQueryMethodHandle tag-encoding
2. **CreateClosedMethodDescriptor**: 读取开放方法定义 → 构建闭包 subject_id → 分配 runtime token → 复制 type_args
3. **RegisterGenericMethodInstantiation**: Thread-safe, idempotent, 按 open_method handle 索引
4. **TryResolveClosedMethod**: O(n) 遍历每个 open_method 的实例化列表
5. **ResolveOrInstantiateMethod**: Fast path (registry) + Slow path (descriptor build + register)
6. **Phase 5b 延迟**: 方法体执行和 RuntimeInstantiatedMethod 清理推迟到 Phase 5b

## Terminal Notes

B2 作为 roadmap child 自动推进，38 个兼容性测试全部通过。
