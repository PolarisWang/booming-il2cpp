---
task_id: clib-iw-g1
lifecycle_status: completed
phase: G
parent_task_id: commonlib-industrialization
source_relation: roadmap-child
---

# clib-iw-g1 — Risk Remediation

## 修复内容

1. **trace.h — 时间戳竞态条件修复**
   - 将 `cached_iso8601()` 中的共享全局变量 (`g_last_ts_ms`, `g_cached_timestamp`) 改为 thread_local，消除多线程写入的数据竞争。

2. **profile.h — 退役节点内存泄漏修复**
   - `ProfileDump()` 现在在聚合完退役节点数据后自动释放节点，防止线程频繁创建/销毁场景下的无限内存增长。

3. **async.h — 异常暴露**
   - 新增 `async_task_awaiter_get_exception()`，提供故障任务异常指针的读取接口。

4. **collection.h — const 访问器**
   - 新增 `get_dictionary_runtime_storage()` const 限定只读访问器，支持仅查找/计数的读路径。
   - 返回 nullptr 表示未分配存储，便于调用者安全处理。

## 完成证据

- 所有 33 个 common 测试通过
- 无回归
