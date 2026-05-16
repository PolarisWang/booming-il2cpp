# C++ 编码规范

## 命名空间

统一使用 `chaos::il2cpp::<module>`：

| 模块 | 命名空间 |
|------|----------|
| `common/chaos/` | `chaos::il2cpp::common` |
| `runtime-core/` | `chaos::il2cpp::runtime_core` |
| `bootstrap/` | `chaos::il2cpp::bootstrap` |
| `interpreter/` | `chaos::il2cpp::interpreter` |
| `support/` | `chaos::il2cpp::support` |
| `hot-update/` | `chaos::il2cpp::hot_update` / `chaos::il2cpp::method_replacement` |

`ChaosIl2cpp::Common` 是旧别名（通过 `namespace ChaosIl2cpp { namespace Common = chaos::il2cpp::common; }`），新代码**不得**使用。

禁止：
- 头文件中使用 `using namespace`（文件作用域）
- 头文件中在 namespace 块外部放置类型声明

## Include 规范

clang-format 自动管理（IncludeBlocks: Regroup），四档优先级：

1. **对应 .h 文件**（.cpp 中第一个）— `"foo.h"`（引号）
2. **项目内部头文件** — `<chaos/header.h>`（尖括号，chaos 前缀）
3. **合约头文件** — `<contracts/header.h>`（尖括号）
4. **标准库** — `<cstdint>`、`<vector>`（尖括号）
5. **外部依赖** — `<gc.h>`、`<fmt/format.h>`（尖括号）

禁止：
- `#include "../module/header.h"` 相对路径
- `#include "chaos/header.h"` 引号风格（统一用 `<chaos/header.h>`）

格式化命令：
```bash
clang-format -i src/native/**/*.cpp src/native/**/*.h
```

## extern "C" 使用规范

用于 ABI 导出，必须搭配注释说明原因：

```cpp
// ABI export: required for C-language linkage from managed/NativeAot code
extern "C" void ChaosFunction() noexcept;
```

- **集中化声明**：放在对应 .h 头文件的 `extern "C" { ... }` 块内
- **避免散落定义**：函数定义避免用 `extern "C"` 修饰（用头文件声明 + 普通 C++ 定义）

## 头文件原则

1. **自包含**：每个 .h 文件包含所有依赖，不依赖前置 include
2. **Include Guard**：`#ifndef CHAOS_IL2CPP_*_H_` / `#define` / `#endif` 风格
3. **最小依赖**：只 include 直接使用的类型
4. **Umbrella header 分层**：
   - `chaos/common.h` — 所有 common 头文件聚合
   - `runtime_core.h` — 所有 runtime-core 头文件聚合
   - 子模块头文件不要相互 include 形成循环依赖
