# Shared ABI Calling Convention v0

## 目标

冻结 AOT 主线、生成代码、热更 runtime 将共同依赖的 calling convention、状态码与 `struct_size` 兼容约定。

## 导出与调用宏

- Windows:
  - `CHAOS_RUNTIME_ABI_EXPORT` 根据 `STATIC / EXPORTS / dllimport` 展开
  - `CHAOS_RUNTIME_ABI_CALL` 固定为 `__cdecl`
- 非 Windows:
  - `CHAOS_RUNTIME_ABI_EXPORT` 使用 default visibility
  - `CHAOS_RUNTIME_ABI_CALL` 为空

## 低层 ABI 表

- `RuntimeAbiV0`
- `CodegenBridgeV0`

两者都遵循：

- `abi_version`
- `struct_size`
- 过程表中的函数指针统一使用 `CHAOS_RUNTIME_ABI_CALL`

## 参数顺序

对需要运行时上下文的方法，v0 统一优先传：

1. `RuntimeState*`
2. `ThreadState*`（如果适用）
3. 主题 handle / object instance / argv
4. `out_return_value`
5. `out_exception`（如果适用）

## 状态码

`RuntimeStatus`:

- `OK`
- `INVALID_ARGUMENT`
- `INVALID_STATE`
- `NOT_FOUND`
- `NOT_SUPPORTED`
- `MANAGED_EXCEPTION`
- `INTERNAL_ERROR`

`BridgeStatus`:

- `OK`
- `INVALID_ARGUMENT`
- `NOT_READY`
- `NOT_FOUND`
- `NOT_SUPPORTED`
- `MANAGED_EXCEPTION`
- `INTERNAL_ERROR`

## 返回值约定

- 纯 query helper 返回 handle / pointer，找不到时返回 `NULL`。
- `method_invoke` / `invoke_virtual` / `delegate_invoke` 在 managed fault 时返回 `*_MANAGED_EXCEPTION`，并通过 `out_exception` 返回异常句柄。

## struct_size 兼容规则

- 所有跨 ABI 的 config / registration struct 都必须以 `uint32_t struct_size` 开头。
- 调用方可以传入“自己认识的最小版本”大小。
- 新版本只能在尾部追加字段，不能重排或删除旧字段。
- 被调用方必须在使用前检查 `struct_size >= sizeof(required_prefix)`。

## 事实来源

- `contracts/native/v0/runtime_abi.h`
- `contracts/native/v0/codegen_bridge.h`
