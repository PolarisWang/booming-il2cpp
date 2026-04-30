# VTable 注册表与虚方法分派

## 核心职责

虚方法分发的中央注册中心。bootstrap 阶段注册所有具体类型的 `TypeVTable`，运行时通过继承链遍历解析虚调用。

## 关键类型

| 类型 | 说明 |
|------|------|
| `VTableSlot` | method_token → method_pointer 映射 |
| `TypeVTable` | type handle、type_token、base_type、slot 数组 |

## 关键函数

| 函数 | 说明 |
|------|------|
| `RegisterTypeVTable(vtable)` | 引导阶段为每类型注册一次 |
| `ResolveVirtualMethodPointer(type_token, method_token)` | 沿 type→base_type 链查找 method_token |
| `GetRegisteredVTableCount()` | 诊断辅助 |

## 与 codegen 的关系

- codegen 在 callvirt 调用点发射 `ResolveVirtualMethodPointer(instance_type_token, declared_method_token)` 而非硬编码函数指针
- **规划中**：codegen 为每个含虚方法的类型发射 `const VTableSlot[]` 和 `const TypeVTable`，在模块的 bootstrap 函数中调用 `RegisterTypeVTable`（当前通过桥接函数运行时解析）
- 这确保了通过继承链的正确分发

## 位置

- 模块: `chaos_runtime_core`
- 文件: `src/native/runtime-core/vtable_registry.*`
