# PInvokeLite

## 负责的 contract 面

- `DllImport`
- native export
- 动态库符号解析
- 基础 marshalling

## 不负责的内容

- 复杂结构体 marshalling
- host lifecycle
- 反射主体与 metadata cache
- 多库装载策略与平台差异抽象层

## 子场景清单

- 平台库的最小 `DllImport`
- `UnmanagedCallersOnly` 导出声明
- `NativeLibrary.TryLoad` / `TryGetExport`
- UTF-8 字符串的基础往返 marshalling
