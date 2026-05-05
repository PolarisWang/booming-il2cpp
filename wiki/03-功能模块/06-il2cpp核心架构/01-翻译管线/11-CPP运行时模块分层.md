# CPP运行时模块分层

> 实际模块结构由 `build/architecture.yaml` 定义，由 `check_arch.py` 强制执行。

## 8 模块架构

| 模块 | 路径 | 命名空间 | 可见性 |
|------|------|---------|--------|
| `chaos_common` | `src/native/common` | `ChaosIl2cpp::Common` | public |
| `chaos_runtime_core` | `src/native/runtime-core` | `chaos::il2cpp::runtime_core` | public |
| `chaos_hot_update` | `src/native/hot-update` | `chaos::il2cpp::hot_update` | internal |
| `chaos_interpreter` | `src/native/interpreter` | `chaos::il2cpp::interpreter` | internal |
| `chaos_support` | `src/native/support` | `chaos::il2cpp::support` | internal |
| `chaos_bootstrap` | `src/native/bootstrap` | `chaos::il2cpp::bootstrap` | internal |
| `chaos_engine_bridge` | `src/native/engine-bridge` | *(anonymous)* | internal |
| `chaos_bdwgc` | `third_party/bdwgc` | *(C library)* | public |

## 依赖 DAG

```
chaos_bdwgc       (无依赖)
chaos_common      (无依赖)
  └→ chaos_runtime_core     (依赖: chaos_bdwgc)
       ├→ chaos_hot_update  (依赖: chaos_runtime_core)
       ├→ chaos_interpreter (依赖: chaos_runtime_core)
       ├→ chaos_support     (依赖: chaos_runtime_core)
       └→ chaos_bootstrap   (依赖: chaos_runtime_core, chaos_hot_update, chaos_support)
            └→ chaos_engine_bridge (依赖: chaos_runtime_core, chaos_bootstrap)
```

## 禁止包含列表

每个模块声明 `forbidden_includes`——严禁包含的路径：

| 模块 | 禁止包含（forbidden_includes） |
|------|-------------------------------|
| chaos_common | `runtime-core`, `bootstrap`, `hot-update`, `interpreter`, `engine-bridge` |
| chaos_runtime_core | `common`, `support`, `interpreter`, `hot-update`, `engine-bridge` |
| chaos_hot_update | `common`, `interpreter`, `engine-bridge`, `bootstrap` |
| chaos_interpreter | `common`, `bootstrap`, `engine-bridge` |
| chaos_support | `common`, `engine-bridge`, `interpreter`, `hot-update` |
| chaos_bootstrap | `common`, `interpreter`, `engine-bridge` |
| chaos_engine_bridge | `common`, `interpreter` |

## 命名约定

| 类别 | 约定 | 示例 |
|------|------|------|
| 类型 (class/struct/enum) | PascalCase | `FinallyGuard`, `SpanRuntimeEntry` |
| 函数 | snake_case | `checked_conv_ovf_i1`, `wrap_add` |
| 常量 (constexpr/enum) | `k_` + snake_case | `k_managed_pointer_local_slot_tag` |
| Include guard | `CHAOS_IL2CPP_{MODULE}_H_` | `CHAOS_IL2CPP_COMMON_H_` |
| 命名空间 | 按模块（见上表） | `ChaosIl2cpp::Common` |

### Include Guard 前缀

| 模块 | Guard 前缀 |
|------|-----------|
| chaos_common | `CHAOS_IL2CPP_COMMON` |
| chaos_runtime_core | `CHAOS_IL2CPP_RUNTIME_CORE` |
| chaos_hot_update | `CHAOS_IL2CPP_HOT_UPDATE` |
| chaos_interpreter | `CHAOS_IL2CPP_INTERPRETER` |
| chaos_support | `CHAOS_IL2CPP_SUPPORT` |
| chaos_bootstrap | `CHAOS_IL2CPP_BOOTSTRAP` |
| chaos_engine_bridge | `CHAOS_IL2CPP_ENGINE_BRIDGE` |

## 全局禁止模式（所有原生代码）

- `using namespace std;` — 禁止
- `using namespace chaos;` — 禁止
- `NULL` — 禁止（使用 `nullptr`）
- 裸 `int`/`long` 类型 — 禁止（使用 `<cstdint>` 类型）
- `#pragma once` — 禁止（使用 `#ifndef`/`#define` include guard）
- 生成代码中禁止裸 `std::` 类型（必须使用 `CHAOS_IL2CPP_*` 宏，如 `CHAOS_IL2CPP_INT32`、`CHAOS_IL2CPP_STRING`）

## 验证层级

| 层级 | 工具 | 时机 |
|------|------|------|
| CLI | `python build/check_arch.py [--module X] [--file Y]` | 按需 |
| CLI (CI) | `python build/check_arch.py --ci` | CI |
| CMake | `cmake --build <dir> --target check_arch_<module>` | 按模块 |
| Pre-commit | `.githooks/pre-commit` | git commit 时 |
| 代码生成 | `NativeCodegenValidator` | C# codegen emission 时 |

## 子系统分布

各子系统在对应模块中的分布：

| 子系统 | 模块 | 路径 |
|--------|------|------|
| 公共库（native_types, 算术, checked_conv, ptr_tag, format, 等） | chaos_common | `src/native/common/chaos/*.h` |
| MemoryDomain 系统 | chaos_runtime_core | `src/native/runtime-core/memory_domain.*` |
| VTable 注册表 | chaos_runtime_core | `src/native/runtime-core/vtable_registry.*` |
| 泛型上下文 | chaos_runtime_core | `src/native/runtime-core/generic_context.*` |
| 字符串表 | chaos_runtime_core | `src/native/runtime-core/string_table.*` |
| 反射查询模型 | chaos_runtime_core | `src/native/runtime-core/reflection_query_model.*` |
| 方法表 (method_table) | chaos_runtime_core | `src/native/runtime-core/method_table.*` |
| ABI Manifest | chaos_runtime_core | `contracts/native/v0/abi_manifest.h` + `src/native/runtime-core/abi_manifest.cpp` |
| 热更新包加载 | chaos_hot_update | `src/native/hot-update/hot_update.*` |
| 方法替换 | chaos_hot_update | `src/native/hot-update/method_replacement.*` |
| 解释器 VM | chaos_interpreter | `src/native/interpreter/interpreter_vm.*` |
| 引导程序 | chaos_bootstrap | `src/native/bootstrap/bootstrap.*` |
| 引擎桥接 | chaos_engine_bridge | `src/native/engine-bridge/engine_bridge.*` |
| 托管线程模型 | chaos_runtime_core | `src/native/runtime-core/thread_state.*` |
| ThreadPool 基础 | chaos_runtime_core | `src/native/runtime-core/thread_pool.*` |
| Monitor 轻量锁 | chaos_runtime_core | `src/native/runtime-core/runtime_core.cpp` |
| Hybrid GC Safepoint | chaos_runtime_core | `src/native/runtime-core/thread_state.*` |
