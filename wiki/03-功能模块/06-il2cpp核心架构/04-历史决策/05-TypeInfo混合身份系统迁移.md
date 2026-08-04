# Hybrid TypeInfo\* 类型身份系统迁移

**来源**: `docs/archive/discuss/il2cpp-mainline/20260504-typeinfo-migration`

## 背景

原有的 type_id 系统使用 `CHAOS_IL2CPP_INTPTR` 整数常量配合 `switch` 函数进行类型兼容性检查，存在三个根本性问题：

1. **跨 TU 不一致**：每个 .cpp 各自编号，`generated_code_compat.h` 用字面量 `2` 硬编码 `System.Object` ID（MSVC C2872 禁止共享 constexpr）
2. **性能非最优**：`chaos_is_type_compatible` 通过 switch 链走 parent 链 O(depth)，中间有函数调用开销
3. **HotUpdate 不支持**：整数 ID 无法在运行时扩展新类型

## 决策

将 `chaos_object_header` 中的整数 `type_id` 替换为 `const TypeInfo*` 指针，使用 Hybrid 方案：

| 层次 | 方案 | 说明 |
|------|------|------|
| 静态类型 | `inline constexpr TypeInfo` | C++17 保证跨 TU 唯一地址，指针比较 O(1) |
| 动态类型 | `chaos_register_type()` heap 分配 | HotUpdate 场景，指针比较同样生效 |

### TypeInfo 布局

```cpp
struct TypeInfo {
    const TypeInfo* parent;          // 基类指针
    CHAOS_IL2CPP_UINT64 stable_id;   // FNV-1a 64-bit hash
    CHAOS_IL2CPP_UINT8  type_shape;  // 1=reference, 2=value, 3=interface
};
```

### 关键优势

- **O(1) 指针比较**：`current == target` 替代 switch 链 + parent walk 函数调用
- **FNV-1a hash stable_id**：跨构建/跨 TU 稳定，无需协调编号
- **parent 指针直接回溯**：`current->parent` 替代 `chaos_get_base_type_id()` switch 函数
- **inline constexpr**：C++17 保证每个 TypeInfo 实例在程序中有唯一定义地址
- **统一 HotUpdate**：动态 TypeInfo 走 heap 分配，指针比较逻辑一致

## 涉及文件

### C++ 运行时（新建/修改）

- `src/native/common/chaos/type_info.h` — **新建**：TypeInfo 结构、FNV-1a hash、shape 常量
- `src/native/runtime-core/type_registry.cpp` — **新建**：动态注册表实现
- `src/native/runtime-core/generated_code_compat.h` — 清理：移除已不用的 fallback 声明
- `src/native/runtime-core/exception_api.cpp` — 同步更新局部 `chaos_object_header`

### C# CodeGen Emitter（修改 8 文件）

- `ObjectModelEmission.cs` — 核心：TypeInfo 实例发射、兼容性函数签名变更
- `ObjectModelUtilities.cs` — 新增 `GetNativeTypeInfoSymbol`/`ComputeStableTypeId`
- `MethodEmission.cs` — ~25 处调用点更新（赋值、比较、虚分发）
- `ExceptionEmission.cs` — 7 处调用点更新
- `ReflectionObjectEmission.cs` — ~30 处调用点更新
- `ObjectEqualityEmission.cs` — switch → stable_id
- `NativeAotLoweringPlanner.cs` — 委托分配
- `InvocationAbi.cs` — 虚分发目标

### Scriban 模板（5 文件）

所有模板的 `type_id_symbol` 字段统一改为 `type_info_symbol`。

## 验证标准

1. `dotnet build` — C# codegen 0 错误
2. `batch_native_aot_runner.py` — 17 个 family emit 通过（迁移前与迁移后一致）
3. `native_compile_runner.py` — 0 TypeInfo 相关 MSVC 错误（所有编译错误均为预存问题）
