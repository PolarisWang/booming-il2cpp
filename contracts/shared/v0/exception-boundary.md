# Shared Exception Boundary v0

## 目标

冻结 managed fault 穿过 runtime / bridge 边界时的唯一出口，不允许把未约束的 C++ 异常直接泄露到 ABI 使用方。

## v0 规则

- ABI 外部可见的 managed failure 统一通过 `out_exception` + `*_MANAGED_EXCEPTION` surface。
- `raise_managed_exception(...)` 是 runtime 侧主动抛出 managed fault 的统一入口。
- `method_invoke(...)`、`invoke_virtual(...)`、`delegate_invoke(...)` 必须捕获内部 carrier，并把异常句柄写入 `out_exception`。

## 当前实现事实

- `runtime_core.cpp` 内部使用 `ManagedExceptionCarrier` 做冷路径异常承载。
- 这是内部实现细节，不构成 ABI 对外承诺。
- 对外稳定面只有：
  - `ExceptionHandle`
  - `out_exception`
  - `MANAGED_EXCEPTION` 状态码

## 边界要求

- managed -> native：不得把原始 CLR exception 对象直接跨边界抛出；必须包装成 `ExceptionHandle`。
- native -> managed：helper 返回 `MANAGED_EXCEPTION` 时，调用方必须先检查 `out_exception`，而不是猜测返回值。
- finally / fault：v0 只冻结“不能因为跨 ABI 边界而吞掉 managed fault”；完整 EH 语义的执行器实现留在后续 phase。

## 非目标

- v0 不定义完整 interpreter EH stack。
- v0 不定义跨线程异常注入。

## 事实来源

- `src/native/runtime-core/runtime_core.cpp`
- `src/native/bootstrap/bootstrap.cpp`
