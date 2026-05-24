# COM 互操作模块

## 概述

COM 互操作模块实现了托管代码与原生 COM 组件之间的双向调用能力：

- **CCW (COM Callable Wrapper)** — 将托管对象暴露为 COM 接口，使原生代码可以通过 COM ABI 调用托管方法
- **RCW (Runtime Callable Wrapper)** — 将原生 COM 对象包装为托管可访问的句柄，支持生命周期管理与接口查询

## 架构状态

| 组件 | 状态 | 版本 |
|------|------|------|
| CCW 基础 (IUnknown) | ✅ 完成 | V4 |
| CCW 多接口 QI | ✅ 完成 | V4 |
| CCW 方法分发 (thunks) | ✅ 完成 | V4 |
| CCW 代码生成 (vtable+fatory) | ✅ 完成 | V4 |
| CCW ITypeInfo/ITypeLib | ✅ 完成 | V2 |
| CCW ResolveCcw 地址距离检查 | ✅ 完成 | V2 |
| RCW 基础 (IUnknown) | ✅ 完成 | V2 |
| RCW QI 缓存 | ✅ 完成 | V2 |
| RCW 线程安全缓存表 | ✅ 完成 | V2 |
| RCW 方法分发 | ✅ 完成 | V2 |
| IDispatch | ✅ 完成 | V2 |
| ICustomMarshaler | ✅ 完成 | V4 |
| Variant 互操作 | ✅ 完成 | V3 |
| IErrorInfo 支持 | ✅ 完成 | V1 |
| STA/MTA 套间支持 | ✅ 完成 | V1 |
| SAFEARRAY Marshaling | ✅ 完成 | V1 |

## 文档目录

- [01-架构总览](01-架构总览.md) — 整体架构设计
- [02-CCW实现](02-CCW实现.md) — COM Callable Wrapper 实现细节
- [03-RCW实现](03-RCW实现.md) — Runtime Callable Wrapper 实现细节
- [04-代码生成](04-代码生成.md) — Codegen 管线与 Scriban 模板
- [05-测试与验证](05-测试与验证.md) — 测试体系和验证方法
- [06-扩展互操作](06-扩展互操作.md) — 套间支持、SAFEARRAY、IErrorInfo
