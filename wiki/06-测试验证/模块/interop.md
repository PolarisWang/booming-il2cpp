objectId: module/interop/basic
objectType: module
primaryModuleId: interop

# interop

## 对象

- `module/interop/basic`

## 目的

- 为 P/Invoke、marshal 与 COM 互操作相关改动提供默认完成前模块验证。
- 覆盖 native 测试（GoogleTest，119 tests，5 skipped）和 managed 测试（foundation-dll pipeline，185 fact tests across 14 families）。

## 覆盖

- `contract/native-abi`
- `contract/native-bridge`
- `contract/marshal-alloc`
- `contract/marshal-string`
- `contract/marshal-struct`
- `contract/marshal-custom`
- `contract/com-rcw`
- `contract/com-ccw`
- `contract/com-connection-point`
- `smoke/PInvokeLite`

## 适用场景

- 托管到 native 的调用边界发生改动。
- native ABI 头文件、布局约束或 compile-only smoke 发生改动。
- 编组、符号绑定或基础导出调用链路发生改动。
- COM RCW/CCW/ConnectionPoint 生命周期管理发生改动。
- ICustomMarshaler 缓存或并发策略发生改动。

## 正式入口

- `run test module --id module/interop/basic`
- `run test module --module interop --profile basic`

## 测试详情

### Native 测试

位于 `testing/src/native/runtime-core/`，通过 ctest -L marshal 运行：

| 测试目标 | 测试数 | 标签 |
|---------|--------|------|
| marshal_smoke | 7 | unit;marshal |
| marshal_alloc | 8 | unit;marshal |
| marshal_string | 11 | unit;marshal |
| marshal_api_basic | 12 | unit;marshal |
| struct_marshal | 14 | unit;marshal |
| custom_marshaler | 18 | unit;marshal |
| com_rcw | 7+5skip | unit;marshal |
| com_ccw | 20 | unit;marshal |
| com_connection_point | 10 | unit;marshal |

### Managed 测试

通过 foundation-dll pipeline 验证：

| 程序集 | 测试族 | AOT Fact | 覆盖内容 |
|--------|--------|----------|---------|
| System.Private.CoreLib | pinvoke-dllimport | 12/12 | 基础 P/Invoke |
| System.Private.CoreLib | rcw-basic | 29/29 | COM RCW |
| System.Private.CoreLib | ccw-basic | 19/19 | COM CCW |
| System.Private.CoreLib | error-info-basic | 10/10 | COM Error Info |
| System.Private.CoreLib | dispatch-basic | 18/18 | COM Dispatch |
| System.Runtime.InteropServices | pinvoke-dllimport | 12/12 | S.R.IS P/Invoke |
| System.Runtime.InteropServices | com-types | 22/22 | COM Types |
| System.Runtime.InteropServices | com-wrappers | 10/10 | COM Wrappers |
| System.Runtime.InteropServices | exception-errors | 18/18 | Exception/Error |
| System.Runtime.InteropServices | marshalling-attributes | 10/10 | Marshal 属性 |
| System.Runtime.InteropServices | runtime-interop-services | 17/17 | Runtime Interop |
| System.Runtime.InteropServices | secure-string-marshal | 4/4 | SecureString |
| System.Runtime.InteropServices | custom-marshaller-contracts | 0/0 (contract-only) | ICustomMarshaler 接口契约 |
| System.Runtime.InteropServices | objective-c-interop | 4/4 | Objective-C 互操作 |
| **合计** | **14 家族** | **185/185** | **全部通过** |

## 验证命令

```bash
# Native 测试
ctest --test-dir build/testing -L marshal --output-on-failure

# Managed 测试（foundation-dll pipeline）
# --all 标志运行所有 interop 家族
run foundation-dll --assembly System.Private.CoreLib --all
run foundation-dll --assembly System.Runtime.InteropServices --all

# 单家族运行
run foundation-dll verify-family --family pinvoke-dllimport --assembly System.Private.CoreLib
run foundation-dll verify-family --family rcw-basic --assembly System.Private.CoreLib
```
