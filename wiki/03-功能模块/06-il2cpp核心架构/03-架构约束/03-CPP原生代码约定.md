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
| 命名空间 | `chaos` | `chaos::il2cpp::runtime` |

## 包含规则

- 头文件使用 `#pragma once`
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
