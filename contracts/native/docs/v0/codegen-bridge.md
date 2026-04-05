# Codegen Bridge v0

> first-proof boundary supplement: `contracts/docs/v0/codegen-bridge-proof-boundary.md`

## 定位

- `boom_codegen_get_bridge_v0()` 返回进程级 bridge 函数表，供 `generated C++` 直接依赖。
- `bridge` 位于 ABI 之上：ABI 提供低层基础能力，bridge 把这些能力整理成生成代码可以稳定消费的高层 helper 面。
- `virtual invoke`、`delegate`、`icall resolve` 已经进入 `bridge v0`，不是后续阶段才会补的占位项。

## 调用约定

- bridge 与 ABI 共享同一套调用约定宏：`BOOM_RUNTIME_ABI_EXPORT` 和 `BOOM_RUNTIME_ABI_CALL`。
- 调用方应先缓存 `CodegenBridgeV0*`，再按 helper 分类访问函数指针。
- bridge 只负责生成代码优先依赖的帮助函数；底层生命周期、对象分配和显式异常触发仍通过 `Runtime ABI` 完成。
- `BridgeStatus` 在 `v0` 中冻结为 32-bit 有符号整数，避免导出 ABI 受 C `enum` 底层表示影响。

## 错误模型

- `bridge` 采用混合错误模型：
  - 查询/解析类 helper 返回状态码或空指针
  - 托管语义失败通过受控异常出口返回，例如 `out_exception`
- `register_codegen` 与 `bootstrap_runtime` 返回 `BridgeStatus`，让 bootstrap 流程可以在宿主边界显式失败。
- `resolve_type_by_token`、`resolve_method_by_token`、`resolve_field_by_token`、`resolve_virtual_method`、`resolve_icall` 在无法解析时返回空句柄/空指针。
- `invoke_virtual` 与 `delegate_invoke` 的托管语义失败不会被静默吞掉，必须通过 `out_exception` 交还调用方。
- `box_value` 失败时返回空对象指针；`unbox_value` 使用状态码报告布局或目标缓冲区不匹配。

## 与 ABI 的职责分层

| 层级 | 负责内容 | 不负责内容 |
| --- | --- | --- |
| `Runtime ABI` | runtime/thread 生命周期、对象分配、类初始化、GC handle、显式异常入口、基础反射句柄查询 | 生成代码友好的虚调用、delegate 语义、icall 解析 |
| `Codegen Bridge` | registration、runtime metadata helper、boxing/unboxing、virtual invoke、delegate、icall resolve | 替代 ABI 的底层状态管理 |

## Helper 分类

### Registration helper

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `register_codegen` | 同时接收 `CodeRegistrationV0`、`MetadataRegistrationV0` 和可选 `CodegenRegistrationOptionsV0` | 返回显式状态码，要求结构体 `struct_size` 已正确填写 |
| `bootstrap_runtime` | 触发生成代码依赖的最小运行时装配 | 不接收参数；失败时返回状态码 |

### Runtime metadata helper

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `resolve_type_by_token` | 用 `image + type_token` 解析类型 | 查无结果返回空 |
| `resolve_method_by_token` | 用 `image + method_token` 解析方法 | 查无结果返回空 |
| `resolve_field_by_token` | 用 `image + field_token` 解析字段 | 查无结果返回空 |

### Boxing / unboxing helper

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `box_value` | 结合 `runtime_state`、`thread_state`、值类型句柄和原生值缓冲区创建 boxed object | 成功返回对象指针，失败返回空 |
| `unbox_value` | 把 boxed object 的内容复制到宿主缓冲区 | 用状态码报告布局或缓冲区问题 |

### Virtual invoke helper

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `resolve_virtual_method` | 用实例类型与声明方法解析最终落点 | 查无结果返回空 |
| `invoke_virtual` | 用实例、参数数组和可选返回缓冲区执行虚调用 | 托管语义失败通过 `out_exception` 返回 |

### Delegate helper

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `create_delegate` | 从方法句柄和目标实例创建 delegate 对象 | 成功返回 delegate 对象指针 |
| `delegate_invoke` | 用参数数组调用 delegate | 语义失败通过 `out_exception` 返回 |

### ICall resolve helper

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `resolve_icall` | 用稳定的内部调用名解析原生入口 | 查无结果返回空指针 |

## `v0` 冻结原则

- `v0` 已冻结 helper 名称、函数签名、参数方向和错误模型。
- 后续版本可以追加字段或扩展 helper 组，但不能在 `v0` 内悄悄改变既有语义。
- 生成代码如果要依赖新的高层语义，应该通过新增版本或追加 helper，而不是改写现有入口的含义。
