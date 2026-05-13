# CPP原生代码约定

## 命名约定

| 分类 | 约定 | 示例 |
|------|------|------|
| 类型 | PascalCase | `RuntimeManager`, `StringHelper` |
| 函数 | snake_case | `resolve_virtual_method`, `object_new` |
| 全局函数 | `chaos_` 前缀 | `chaos_is_string_id`, `chaos_extract_string_id` |
| 宏 | `CHAOS_` 前缀 | `CHAOS_STRING_ID_TAG`, `CHAOS_IL2CPP_INT32` |
| 常量 | UPPER_SNAKE_CASE | `CHAOS_BRIDGE_STATUS_OK` |
| 枚举 | PascalCase + k 前缀 | `kCarrierKind_Int32` |
| 命名空间 | `chaos::il2cpp::<module>` | `chaos::il2cpp::runtime_core`, `chaos::il2cpp::common` |

## 命名空间规范

所有 native C++ 代码使用小写命名空间形式 `chaos::il2cpp::<module>`：

| 模块 | 命名空间 |
|------|----------|
| `common/chaos/` | `chaos::il2cpp::common` |
| `runtime-core/` | `chaos::il2cpp::runtime_core` |
| `bootstrap/` | `chaos::il2cpp::bootstrap` |
| `interpreter/` | `chaos::il2cpp::interpreter` |
| `support/` | `chaos::il2cpp::support` |
| `hot-update/` | `chaos::il2cpp::hot_update` / `chaos::il2cpp::method_replacement` |

禁止事项：

- **禁止使用 `using namespace`（文件作用域）** — 头文件中不得使用文件作用域的 `using namespace` 指令。cpp 文件中有限使用可接受。
- **禁止 namespace 外部放置类型声明** — 头文件中的类型声明必须放在命名空间块内部。
- **旧命名空间别名** — `ChaosIl2cpp::Common` 是旧命名空间别名（通过 `namespace ChaosIl2cpp { namespace Common = chaos::il2cpp::common; }` 提供向后兼容），新代码不得直接使用 `ChaosIl2cpp::Common`。

## Include 规范

四档优先级顺序（clang-format 自动管理）：

1. **对应的 `.h` 文件**（.cpp 中第一个 include）— `"foo.h"`（引号）
2. **项目内部头文件** — `<chaos/header.h>`（尖括号，`chaos/` 前缀）
3. **合约头文件** — `<contracts/header.h>`（尖括号，`contracts/` 前缀）
4. **标准库头文件** — `<cstdint>`、`<vector>`、`<string>` 等（尖括号）
5. **外部依赖** — `<gc.h>`、`<fmt/format.h>` 等（尖括号）

禁止事项：

- **禁止使用 `#include "../module/header.h"` 相对路径** — 改用 CMake `target_include_directories` 后直接用 `"module/header.h"` 或 `<module/header.h>`。
- **禁止使用 `#include "chaos/header.h"` 引号风格** — 统一使用 `<chaos/header.h>`。

### clang-format 配置

项目 `.clang-format` 配置了 `IncludeBlocks: Regroup` 和 `IncludeCategories`。运行以下命令统一格式：

```bash
clang-format -i src/native/**/*.cpp src/native/**/*.h
```

## extern "C" 使用规范

`extern "C"` 用于 ABI 导出，必须搭配注释说明原因：

```cpp
// ABI export: required for C-language linkage from managed/NativeAot code
extern "C" void ChaosFunction() noexcept;
```

规范：

- **集中化声明**：尽量放在对应的 `.h` 头文件中的 `extern "C" { ... }` 块内。
- **避免散落定义**：函数定义应避免在 `.cpp` 文件中以 `extern "C"` 前缀修饰（改用头文件声明 + 普通 C++ 定义）。
- **.cpp 文件顶部注释**：文件顶部必须有注释块说明为何需要 extern "C"（如 `// ABI exports: extern "C" linkage for managed/NativeAOT callability.`）。

## 头文件原则

1. **自包含**：每个 `.h` 文件必须包含它自己所有的依赖，不依赖前置 include。
2. **Include Guard**：统一使用 `#ifndef CHAOS_IL2CPP_*_H_` / `#define` / `#endif` 风格。
3. **最小依赖**：头文件只 include 它直接使用的类型，不包含"可能用到的"。
4. **Umbrella header 分层**：
   - `chaos/common.h` — 所有 common 头文件的聚合
   - `runtime_core.h` — 所有 runtime-core 头文件的聚合（有 namespace 块，保证声明顺序）
   - 子模块头文件不要相互 include 形成循环依赖

## 包含规则

- 头文件使用 `#pragma once`（或传统 `#ifndef` guard）
- C ABI 合约头文件用 `extern "C"`（`codegen_bridge.h`, `runtime_abi.h`）
- 模块内部头文件优先引用本地头文件
- 跨模块引用必须通过合约头文件
- 不得在头文件中包含运行时实现细节

## 依赖方向

由 `build/architecture.yaml` + `check_arch.py` 强制执行：

| 规则 | 说明 |
|------|------|
| 下层不引用上层 | 违反导致构建失败 |
| 同层无循环依赖 | 循环引用被静态检测 |
| 合约头文件免检 | C ABI 合约头文件不在检查范围内 |

## 平台抽象

- 所有原生类型通过 `CHAOS_IL2CPP_*` 宏定义（`native_types.h`）
- 无裸 `int32_t`、`uint64_t` 等 `<cstdint>` 类型
- 无裸 `std::string`、`std::vector` 等 STL 类型
- C ABI 合约头文件豁免（保持纯 C 兼容）

## 代码风格

- 花括号：Allman 风格（换行左花括号）
- 缩进：4 空格
- 指针/引用：`*`/`&` 靠类型（`int32_t* p`）
- 行尾注释：`//` 风格
- 文件头注释：文件名 + 简短描述
