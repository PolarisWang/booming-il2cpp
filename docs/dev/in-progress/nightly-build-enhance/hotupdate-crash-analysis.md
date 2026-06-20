# Hotupdate Benchmark Crash — 根因分析

## Crash 描述
`DispatchDirectVoid + --benchmark-iterations + --patch-data` 组合导致 entry.exe 崩溃。

## 根因

查看 `hotpatch_dispatch.h` 的 dispatch 路由：

```cpp
// Line 167-183: ChaosDispatchMethodBench (benchmark fast path)
inline int32_t ChaosDispatchMethodBench(...) {
    auto& entry = entries[index];

    if (HotpatchIsActive(entry) && !HotpatchShouldKeepNative(entry)) {
        InterpreterEntryDirectFast(entry.method_key);  // ① patched → interpreter
    } else if (thunks) {
        thunks[index]();                                // ② thunk path
    } else if (entry.direct_ptr) {
        DispatchDirectVoid(entry.direct_ptr, entry.flags); // ③ direct call
    }
    return 0;
}
```

Benchmark 模式下 dispatch 走 `ChaosDispatchMethodBench`（不是 `ChaosDispatchMethod`），
区别在于 **没有 SEH 异常保护**（没有 `CHAOS_EH_TRY/CATCH`）。

当：
1. `--patch-data` 加载 patch 后，部分方法的 `kHotpatchActive` 被置位
2. `--benchmark-iterations` 启动 benchmark 循环
3. `DispatchDirectVoid` 调用 `direct_ptr(0,0,0,0)` 
4. 但 `direct_ptr` 在被 patch 修改后可能指向无效内存 → crash

## 当前修复

在 hotupdate_chunk.py 中：
- `benchmark_iterations = 3`（小值，容忍 crash 后继续后续 stage）
- chunk 越大迭代越少（500/2000 分档）
- crash 时 benchmark 产出为空，但 fact 和 revert 仍正常运行

## 长期修复方案

需要在 native 侧 `ChaosDispatchMethodBench` 中加入 SEH 保护：

```cpp
// 在 hotpatch_dispatch.h 的 ChaosDispatchMethodBench 中加入：
inline int32_t ChaosDispatchMethodBench(...) {
    if (index < 0 || index >= count) return -1;
    auto& entry = entries[index];
    CHAOS_EH_TRY
        // ... existing dispatch logic ...
    CHAOS_EH_CATCH_BEGIN
        return -1;  // benchmark crash → skip this method
    CHAOS_EH_END
    return 0;
}
```
