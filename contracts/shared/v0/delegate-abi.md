# Delegate ABI v0 (Unified Layout)

## 目标

定义托管 delegate 对象的统一内存布局，保证 AOT codegen、bootstrap bridge、Marshal.GetDelegateForFunctionPointer 三条路径共享同一份结构体布局，消除此前 `DelegateInstance` (bootstrap) 与 `chaos_type_System_Delegate` (codegen) 的双布局不一致。

## 统一布局

```cpp
struct DelegateObject {
    void* type_info;                            // [0]   8  ThinLockableHeader.type_info
    uint64_t sync_state;                        // [8]   8  ThinLockableHeader.sync_state
    CHAOS_IL2CPP_INTPTR chaos_delegate_target;  // [16]  8  target_instance
    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr; // [24] 8  method_pointer
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list; // [32] 8  vector<INTPTR>* or nullptr
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count; // [40] 8  0=single, >0=multicast
    CHAOS_IL2CPP_UINT32 chaos_delegate_method_token; // [48] 4  hot-update token
    CHAOS_IL2CPP_UINT32 _pad;                   // [52] 4
};
static_assert(sizeof(DelegateObject) == 56);
```

## 字段语义

- `type_info`: 对象类型元数据（GC header 的一部分）。bridge 路径（old-gen）设为 `nullptr`；codegen newobj 路径通过 `GcTypeLayout` 注册。
- `sync_state`: GC 同步状态（lock-free 同步用）。
- `chaos_delegate_target`: 闭包/实例 delegate 的 target instance；静态 delegate 为 `0`。
- `chaos_delegate_method_ptr`: 解析后的真实入口函数指针。
- `chaos_delegate_invocation_list`: multicast 场景下的扁平化 invocation 列表。单 entry 时为 `0`，multicast 时指向 `std::vector<CHAOS_IL2CPP_INTPTR>*`（GC 分配）。
- `chaos_delegate_invocation_count`: multicast 列表长度。`0` 表示单 entry，`>0` 表示 multicast wrapper。
- `chaos_delegate_method_token`: 创建 delegate 时记录的 method token，用于 hot-update 时 `method_replacement::Resolve` 重解析。

## 分配路径

| 路径 | 分配器 | GC 扫描 | 对象大小 |
|------|--------|---------|----------|
| bootstrap `CreateDelegate` | `g_old_gen.Allocate(56, true)` | 保守扫描 | 56B (size class 56) |
| `MarshalGetDelegateForFunctionPointerImpl` | `g_old_gen.Allocate(56, true)` | 保守扫描 | 56B (size class 56) |
| AOT codegen `newobj` | `CHAOS_IL2CPP_NEW_GC(Type)` → `GcAllocate` | 精确扫描 (GcTypeLayout) | 56B |
| `DelegateCombine` 创建的 multicast wrapper | `g_old_gen.Allocate(56, true)` | 保守扫描 | 56B (size class 56) |

## Multicast 模型

使用 `vector<INTPTR>*` 替代旧的 linked-list `next` 指针：

- **单 entry**: `invocation_list = 0`, `invocation_count = 0`, `target` + `method_ptr` 设为目标。
- **Multicast wrapper**: `invocation_list = vector<entry>`, `invocation_count = N`, `target = 0`, `method_ptr = 0`。
- 所有 entry 在 combine 时已扁平化（递归展开嵌套 multicast），保证 `invocation_list` 中的每个元素都是单 entry delegate。
- `DelegateInvoke` 遍历 `invocation_list`，对每个 entry 使用其自身的 `target` + `method_ptr`（经 `method_replacement::Resolve` 重解析后）进行分发。

## Combine / Remove

由 `runtime_core::DelegateCombine` / `DelegateRemove` 实现：
1. 递归扁平化左右操作数的 entry 列表。
2. 验证所有 entry 的 `type_info` 一致。
3. 构造新的 multicast wrapper（entry > 1）或直接返回单 entry。
4. 通过 `g_old_gen.Allocate` 分配 wrapper DelegateObject + vector。

## 创建语义

- `CreateDelegate` 从 `MethodInfoHandle` 解码 token → 查找 `method_pointers` 表 → 分配 `DelegateObject`。
- 查找失败返回 `nullptr`。
- `MarshalGetDelegateForFunctionPointer` 分配 `DelegateObject`，`target` = native fn ptr，`method_ptr` = arity dispatch thunk。

## 调用语义

- `DelegateInvoke` 转化为 invocation_list 遍历 + 单 entry dispatch。
- 单 entry dispatch 流程：
  1. `method_replacement::Resolve(method_token)` 检查 hot-update 替换。
  2. arity 0-4: `DfnDispatch<has_target>` 类型化函数指针调用。
  3. arity 5+: `method_invoke` ABI fallback。
- managed fault 通过 `out_exception` + `CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION` 返回。

## 热更兼容约束

- `chaos_delegate_method_token` 字段提供 hot-update 所需的 method token，`DelegateInvoke` 每次调用前通过 `method_replacement::Resolve(token)` 重新解析入口，无需重建 delegate 实例。
- multicast 中的每个 entry 独立保留其自身的 `method_token`，hot-update 可精确替换任意 entry 的目标方法。

## 事实来源

- `src/native/runtime-core/runtime_core.h` — `DelegateObject` 结构体定义
- `src/native/runtime-core/core/delegate_helpers.h` / `.cpp` — combine/remove 实现
- `src/native/runtime-core/core/delegate_thunks.cpp` — Marshal bridge thunks
- `src/native/bootstrap/bootstrap.cpp` — CreateDelegate / DelegateInvoke
- `contracts/native/v0/codegen_bridge.h`
- `src/managed/Chaos.IL2CPP.Generator/Templates/NativeAot.DelegateRuntimeSupport.cpp.scriban`
