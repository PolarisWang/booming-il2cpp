# Shared Delegate ABI v0

## 目标

冻结 proof runtime 当前已经实现的 delegate 形态，保证 AOT 与未来热更 bridge 至少共用同一套“delegate 是什么”的最小模型。

## 当前 proof layout

```cpp
struct DelegateInstance {
    uint32_t method_token;
    void* method_pointer;
    void* target_instance;
};
```

## 字段语义

- `method_token`: 创建 delegate 时记录的 method token。
- `method_pointer`: bootstrap 通过 code registration 解析出的真实入口。
- `target_instance`: 闭包/实例 delegate 的 target；静态 delegate 为 `NULL`。

## 创建语义

- `create_delegate(...)` 先从 `MethodInfoHandle` 解码 token。
- 再用 token 到 `method_pointers` 表里查找真实入口。
- 查找失败则返回 `NULL`。

## 调用语义

- `delegate_invoke(...)` 当前 proof 只支持单参数调用面。
- 如果 `target_instance != NULL`，走 closed-instance delegate 调用约定。
- 否则走 static delegate 调用约定。
- managed fault 仍通过 `out_exception` + `CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION` 返回。

## 热更兼容约束

- Phase 6 之前，cross-mode delegate bridge 还未落地；但 v0 已冻结 delegate 的最小三元组：
  - `method_token`
  - `method_pointer`
  - `target_instance`
- 后续 AOT <-> hot update wrapper 只能在这个共享模型之上扩展，不能改写其基础语义。

## 事实来源

- `src/native/bootstrap/bootstrap.cpp`
- `contracts/native/v0/codegen_bridge.h`
