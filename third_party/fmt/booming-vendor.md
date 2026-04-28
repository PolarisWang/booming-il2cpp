# {fmt} vendor metadata

## 来源

- 项目名称：`{fmt}` (fmtlib)
- 上游仓库：`https://github.com/fmtlib/fmt`
- 当前版本：`11.1.4`
- 许可证：MIT
- 当前 vendored 路径：`third_party/fmt`

## 许可证

MIT License — 详见本目录 `LICENSE.txt`

Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors

## 用途

- 为 `chaos_common` 库提供高性能、类型安全的字符串格式化能力
- 替代 codegen 生成代码中所有 `snprintf` 热路径（Int32.ToString、Single/Double.ToString、AppendFormatted、String.Join）
- 消除手写 char[] buffer 管理的安全风险

## 落点说明

- 以 compiled static library 模式集成（非 header-only），避免编译膨胀
- `chaos_common` 通过 `target_link_libraries(chaos_common PUBLIC chaos_fmt)` 引用
- 生成代码不直接引用 fmt 头文件——通过 `chaos_common` 的 `chaos/format.h` 封装层使用
