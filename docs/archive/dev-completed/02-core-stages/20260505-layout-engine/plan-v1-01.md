# LayoutEngine 子系统实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 `parent_task_id: 20260504-generic-runtime-instantiation-phases-3-5` 且 `source_relation: roadmap-child`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 实现独立值类型布局计算引擎 LayoutEngine，替代 `ComputeValueTypeLayout` 的简化占位符。

**架构：** LayoutEngine 是单一职责的 C++17 类，通过 ModuleRegistry 获取类型元数据，递归解析字段类型大小，应用 ECMA-335 对齐规则，计算结果按 module_id 分组缓存。

**技术栈：** C++17, ModuleRegistry, ReflectionQueryTypeDescriptor, unordered_map 缓存

**架构审核模式：** critical（涉及 IL2CPP runtime-core + contracts 主线）

**结构告警重点：**
- LayoutEngine 必须与 runtime_instantiation.cpp 职责分离——LayoutEngine 只计算布局，不负责类型注册
- 新增文件不得在 layout_engine.h 中混入 registry/instantiation 逻辑
- 递归解析必须有明确的深度/循环保护，不能写死在某一大函数中

**权责图审核主题：**
- LayoutEngine (计算布局) → RuntimeInstantiatedType (存储布局) → RuntimeInstantiationBridge (编排流程)
- LayoutEngine 不直接访问 GenericContextRegistry，通过 TypeInfoHandle 接口交互

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a（基础设施）
- capabilityItem: reflection-type layout computation
- ownerSubjectId: n/a（基础设施组件，非 directly-testable subject）
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: Smoke（LayoutEngine 缓存按 module 分组，hotunload 时需失效）
- formalVerificationObjects: [LayoutEngine::GetOrComputeLayout, ComputeValueTypeLayout, TypeSizeResolver chain]
- requiredGates: collector -> registry -> workspace

**设计文档：** 父任务 `brainstorm-v1-01.md` 和 `roadmap-v1-01.md`

**问题清零来源：** parent-roadmap（STATUS.md blocking_questions: [], clearance_confirmed_by_user: true）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a（实现细节，非长期 wiki 知识）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 文件清单

### 新增文件

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/layout_engine.h` | LayoutEngine 类完整声明、FieldLayout/TypeLayout/SizeAndAlignment 结构体 |
| `src/native/runtime-core/layout_engine.cpp` | 完整实现：递归大小解析、对齐规则、缓存、循环检测 |

### 修改文件

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/runtime_instantiation.cpp` | `ComputeValueTypeLayout` 从简化占位符改为调用 LayoutEngine |
| `src/native/runtime-core/runtime_instantiation.h` | `RuntimeInstantiatedType` 增加 `resolved_field_types` 可选缓存字段 |
| `src/native/runtime-core/CMakeLists.txt` | 添加 `layout_engine.cpp` |
| `src/native/common/chaos/native_types.h` | 可能新增 alignment 相关常量（如 `CHAOS_IL2CPP_DEFAULT_ALIGNMENT`） |
| `src/native/runtime-core/module_registry.h` | 新增 `TokenToTypeHandle` 辅助函数（从 module_id + token → TypeInfoHandle） |
| `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` | 新增 LayoutExpectation 数据发射 |
| `src/native/runtime-core/runtime_core.cpp` | 新增 `VerifyModuleLayouts()` 在模块注册时调用 |

---

## 实现步骤

### Step 1: LayoutEngine 头文件定义

**文件：** `src/native/runtime-core/layout_engine.h`

核心数据结构：

```cpp
namespace chaos::il2cpp::layout {

// ── 循环检测守卫 ──
struct RecursionGuard {
    static constexpr uint32_t kMaxDepth = 64;
    static constexpr uint32_t kMaxVisitedSize = 256;
    
    uint32_t depth = 0;
    TypeInfoHandle visiting[32];  // 当前正在解析的 type 栈
    uint32_t visit_count = 0;
    
    bool Enter(TypeInfoHandle type);  // false = 循环检测到
    void Leave();
};

// ── 字段布局 ──
struct FieldLayout {
    TypeInfoHandle resolved_type;  // 解析后的字段类型
    uint32_t offset;
    uint32_t size;
    uint32_t alignment;
    bool is_gc_reference;
};

// ── 类型完整布局 ──
struct TypeLayout {
    TypeInfoHandle closed_type;
    uint32_t value_size;
    uint32_t alignment;
    uint32_t field_count;
    FieldLayout* fields;    // 堆分配，LayoutEngine 管理生命周期
    uint32_t gc_ref_count;
};

// ── 布局引擎 ──
class LayoutEngine {
public:
    const TypeLayout* GetOrComputeLayout(TypeInfoHandle closed_type);
    void InvalidateModuleCache(uint32_t module_id);
    
    // 诊断
    uint32_t GetCacheHitCount() const;
    uint32_t GetComputeCount() const;

private:
    struct SizeAndAlignment {
        uint32_t size;
        uint32_t alignment;
    };

    TypeLayout* ComputeLayout(TypeInfoHandle type, RecursionGuard& guard);
    SizeAndAlignment ResolveSizeAndAlignment(TypeInfoHandle type, RecursionGuard& guard);
    
    TypeInfoHandle ResolveFieldType(
        const ReflectionQueryFieldDescriptor& field,
        TypeInfoHandle* type_args,
        uint32_t arg_count);
    
    // 对齐工具
    static uint32_t AlignUp(uint32_t offset, uint32_t alignment);
    static uint32_t NaturalAlignment(uint32_t size);
    
    // 缓存
    CHAOS_IL2CPP_UNORDERED_MAP(TypeInfoHandle, TypeLayout*) cache_;
    CHAOS_IL2CPP_UNORDERED_MAP(uint32_t, CHAOS_IL2CPP_VECTOR(TypeInfoHandle)) module_index_;
    CHAOS_IL2CPP_MUTEX mutex_;
    
    uint32_t cache_hits_ = 0;
    uint32_t compute_count_ = 0;
};

// ── 布局验证 ──
struct LayoutExpectation {
    uint32_t type_token;
    uint32_t expected_size;
    uint32_t field_count;
    struct { uint32_t offset; uint32_t size; uint32_t alignment; } fields[16];
};

void VerifyModuleLayouts(const ModuleDescriptor* module);

}  // namespace
```

### Step 2: LayoutEngine 实现

**文件：** `src/native/runtime-core/layout_engine.cpp`

#### 2a: ResolveSizeAndAlignment — 核心类型大小解析

```
输入: TypeInfoHandle type, RecursionGuard& guard
输出: SizeAndAlignment {size, alignment}

解析链路:
1. 检查缓存 (SizeAndAlignment 级别的缓存，非 TypeLayout 缓存)
2. 如果 type == 0 → return {1, 1} (空值类型约定)

3. 从 Module Registry 查找 type 的 descriptor:
   module_id = GetModuleId(type)
   token = GetTypeToken(type)
   module = LookupModule(module_id)
   desc = FindReflectionQueryTypeByToken(module->image, token)

4. 根据 desc 判断类型:
   a. primitive types (通过 subject_id_utf8 识别):
      "System.Int32" → {4, 4}
      "System.Int64" → {8, 8}
      "System.Single" → {4, 4}
      "System.Double" → {8, 8}
      "System.Byte" → {1, 1}
      "System.SByte" → {1, 1}
      "System.Int16" → {2, 2}
      "System.UInt16" → {2, 2}
      "System.UInt32" → {4, 4}
      "System.UInt64" → {8, 8}
      "System.Char" → {2, 2}
      "System.Boolean" → {1, 1}
      "System.IntPtr"/"System.UIntPtr" → {sizeof(void*), sizeof(void*)}

   b. reference types (非值类型) → {sizeof(void*), sizeof(void*)}

   c. value types (struct):
      - 如果已在 visiting set 中 → 循环检测 → return {sizeof(void*), sizeof(void*)}
      - 递归调用 ComputeLayout(type, guard) 获取完整布局
      - return {type_layout->value_size, type_layout->alignment}

5. 存入 size/alignment 缓存
```

#### 2b: ComputeLayout — 完整布局计算

```
输入: TypeInfoHandle type, RecursionGuard& guard
输出: TypeLayout*

1. 查缓存 (TypeLayout 缓存)
2. Enter guard (循环检测)
3. 获取 open_type_desc = type->generic_type_definition
4. 如果 open_type_desc == nullptr → 非泛型类型，直接:
   field_count = open_type_desc->field_count
   fields = open_type_desc->fields (直接引用)

5. 解析 type_args:
   如果 type 是封闭泛型值类型:
     generic_type_definition = TryDecodeReflectionQueryHandle(type)
     // 但 TypeInfoHandle 是 (module_id, token) 编码，不是指针
     // 所以需要从 ModuleRegistry 获取 desc:
     desc = LookupModule(GetModuleId(type))->FindTypeByToken(GetTypeToken(type))
     generic_type_def = desc->generic_type_definition
     type_args = RuntimeInstantiatedType 的 type_args (从外部传入)

   实际实现：ComputeLayout 需要知道 type_args
   所以接口应为:
   ComputeLayout(TypeInfoHandle closed_type, const TypeInfoHandle* type_args, uint32_t arg_count)

6. 对每个字段:
   a. 确定字段类型:
      - 从 field.member_type_utf8 判断是否为泛型参数
      - "!0", "!1"... → 从 type_args[索引] 取 TypeInfoHandle
      - "!!0", "!!1"... → 从 method_type_args[索引] 取
      - 其他 → 通过 Module Registry 解析 member_type → TypeInfoHandle
   
   b. field_type = ResolveFieldType(field, type_args, arg_count)
   
   c. SizeAndAlignment = ResolveSizeAndAlignment(field_type, guard)
   
   d. field_offset = AlignUp(current_offset, alignment)
   e. 记录 FieldLayout
   f. current_offset += size

7. struct_alignment = ComputeStructAlignment(fields)
8. value_size = AlignUp(current_offset, struct_alignment)
9. 创建 TypeLayout, 存入 cache, module_index[module_id].push_back(type)
10. Leave guard
```

#### 2c: ResolveFieldType — 字段类型解析

```
输入: field descriptor, type_args array
输出: TypeInfoHandle

解析 member_type_utf8:
1. 如果以 "!" 开头:
   "!0" → type_args[0]
   "!1" → type_args[1]
   ...
   "!!0" → method_type_args[0] (当前不支持，Phase 5)
   
2. 如果匹配已知类型名:
   "System.Int32" → 查找 Module Registry 中所有模块匹配的类型
   → 返回对应的 TypeInfoHandle
   
3. 如果无法解析 → return 0 (调用方 fallback 到 sizeof(void*))
```

#### 2d: 对齐规则

```
AlignUp(offset, alignment):
    return (offset + alignment - 1) & ~(alignment - 1)

NaturalAlignment(size):
    size == 1 → 1
    size == 2 → 2
    size == 4 → 4
    size == 8 → 8
    else → sizeof(void*) (保守)

ComputeStructAlignment(FieldLayout[]):
    max_alignment = 1
    for each field:
        max_alignment = max(max_alignment, field.alignment)
    // 上限为平台指针大小 (ECMA-335 II.12.2.4)
    return min(max_alignment, sizeof(void*))
```

#### 2e: 缓存

```
cache_: TypeInfoHandle → TypeLayout*
  - 插入: ComputeLayout 完成后
  - 查询: GetOrComputeLayout 入口
  - 失效: InvalidateModuleCache(module_id)

module_index_: module_id → vector<TypeInfoHandle>
  - 插入: ComputeLayout 完成时记录
  - 使用: InvalidateModuleCache 遍历清理
  - 移除: module 卸载时

SizeAndAlignment 二级缓存:
  避免递归中重复解析同一类型的 size
  (可选优化，首次实现可以不做，先验证正确性)
```

### Step 3: 集成到 runtime_instantiation.cpp

在 `ResolveOrInstantiateType` 慢路径中，将 `ComputeValueTypeLayout` 替换为 `LayoutEngine::GetOrComputeLayout`：

```cpp
// 原调用:
ComputeValueTypeLayout(rt_type);

// 改为:
auto* engine = GetLayoutEngine();  // 全局单例
const auto* layout = engine->GetOrComputeLayout(closed_handle);
if (layout != nullptr) {
    rt_type->value_size = layout->value_size;
    rt_type->field_offsets = new uint32_t[layout->field_count]; // 或共享 layout->fields
    for (uint32_t i = 0; i < layout->field_count; i++) {
        rt_type->field_offsets[i] = layout->fields[i].offset;
    }
    rt_type->field_offset_count = layout->field_count;
}
```

注意：`RuntimeInstantiatedType` 当前 `field_offsets` 是 `malloc` 分配的，所以 LayoutEngine 返回的 `FieldLayout*` 不能直接引用（生命周期不同）。两种方案：
- **方案 A**：LayoutEngine 返回 layout 数据，调用方拷贝到自己的内存中（当前方案）
- **方案 B**：RuntimeInstantiatedType 直接引用 LayoutEngine 的缓存（需要保证 LayoutEngine 缓存不被清理）

选择方案 A（更安全，因为 Hotunload 可能导致 LayoutEngine 缓存失效）。

### Step 4: RuntimeInstantiatedType 调整

`runtime_instantiation.h` 中增加字段类型缓存字段（可选，用于避免重复计算）：

```cpp
struct RuntimeInstantiatedType {
    // ... 现有字段 ...
    
    /// 解析后的字段类型 handle 数组 (heap-allocated)
    /// Phase 3 新增: 每个字段解析后的类型 handle
    TypeInfoHandle* resolved_field_types = nullptr;
    CHAOS_IL2CPP_UINT32 resolved_field_count = 0u;
};
```

### Step 5: 模块注册时布局验证

**`native-aot.module.h` (codegen 端)**：

在每个模块 header 中发射 LayoutExpectation 数组：

```cpp
// Codegen 在 ObjectModelEmission.cs 中新增:
// 遍历模块的所有值类型，发射期望布局

constexpr LayoutExpectation kLayoutExpectations[] = {
    { 0x02000050, 24, 3,
      {{0, 4, 4}, {8, 4, 4}, {16, 8, 8}}
    },
};
```

注意：codegen 需要能够确定值类型的布局。在 .NET 中，codegen 可以通过 `System.Runtime.InteropServices.Marshal.SizeOf()` 或 `sizeof` 来获取。但 codegen 运行在托管侧，可以用 `System.Runtime.CompilerServices.RuntimeHelpers` 或直接反射字段偏移。

实际上，codegen 端通过 typedb（metadata 数据库）了解所有类型。在 `.NET` 中，LayoutKind 和 FieldOffset 等信息在 metadata 中可用。codegen 可以计算预期的布局并与运行时的 LayoutEngine 结果比对。

**`runtime_core.cpp` (bootstrap 端)**：

```cpp
extern "C" void VerifyModuleLayouts(const ModuleDescriptor* module) {
    if (module->layout_expectations == nullptr || module->layout_expectation_count == 0)
        return;
    
    auto* engine = layout::GetLayoutEngine();
    for (uint32_t i = 0; i < module->layout_expectation_count; i++) {
        auto& exp = module->layout_expectations[i];
        TypeInfoHandle handle = MakeTypeHandle(module->module_id, exp.type_token);
        const auto* actual = engine->GetOrComputeLayout(handle);
        
        CHAOS_IL2CPP_ASSERT_MSG(
            actual != nullptr && actual->value_size == exp.expected_size,
            "LayoutEngine: type 0x%08x size mismatch: expected %u, got %u",
            exp.type_token, exp.expected_size, actual ? actual->value_size : 0);
    }
}
```

### Step 6: CMakeLists.txt

在 `src/native/runtime-core/CMakeLists.txt` 中添加：

```cmake
layout_engine.cpp
layout_engine.h
```

### Step 7: 测试

LayoutEngine 是纯计算组件，不依赖 interpreter/bridge/GC，可以独立测试。

**测试内容：**
1. 基本类型大小解析（int=4, long=8, float=4, void*=8）
2. 简单结构体布局计算（两个 int 字段 → size=8, offsets={0,4}）
3. 带 padding 结构体（byte+int → size=8, offsets={0,4}）
4. 复杂结构体（byte+long → size=16, offsets={0,8}）
5. 嵌套值类型
6. 泛型参数替换 (`Pair<int>` → int 字段正确解析)
7. 缓存命中/未命中
8. 空值类型 (size=1)
9. 引用类型字段 (size=sizeof(void*))

测试框架使用现有的 Python 单元测试模式（`tests/unit/compatibility/test_*.py`），通过编译独立的 C++ 测试代码并验证输出。

---

## 验证命令

```bash
# Step 1-2: 编译验证
cmake --build . --target chaos_runtime_core  # 0 errors

# Step 7: 测试
python -m pytest tests/unit/compatibility/test_layout_engine.py -v

# 全量
python -m pytest tests/unit/compatibility/test_native_runtime_core.py -v
```

---

## 风险

| 步骤 | 风险 | 缓解 |
|------|------|------|
| Step 2a | `member_type_utf8` 字符串解析稳定性 | 先支持已知类型名 + 泛型参数模式，未知类型 fallback 到 sizeof(void*) |
| Step 2b | 递归深度超过系统栈 | RecursionGuard 深度限制 64 + visiting set 循环检测 |
| Step 2c | 跨模块类型名解析（如 DLL_A 中的类型在 DLL_B 注册前被引用） | 只查当前已注册的模块；未找到 → fallback |
| Step 5 | Codegen 侧无法确定 LayoutExpectation | 先跳过 codegen 侧，仅实现 native 侧 LayoutEngine，后续补 LayoutExpectation |
| Step 7 | 测试需要构造 TypeInfoHandle | 使用 ModuleRegistry 注册 mock 类型数据 → 构造 handle |

---

## 执行顺序

```
Step 1 (layout_engine.h) → Step 2 (layout_engine.cpp) → Step 3 (集成 runtime_instantiation.cpp)
                                                                   ↓
Step 4 (RuntimeInstantiatedType 调整) ←──────────────────────────────┘
                                                                   ↓
Step 6 (CMakeLists.txt) → Build 验证 → Step 7 (测试) → Step 5 (Codegen LayoutExpectation)
```

Step 5 (Codegen LayoutExpectation) 可放在最后，不是 Phase 3 的关键路径。
