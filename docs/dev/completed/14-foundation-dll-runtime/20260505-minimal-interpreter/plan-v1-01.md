# MinimalInterpreter 扩展实现计划

**目标：** MinimalInterpreter 支持值类型栈操作和外部调用派发。

**架构：** ValueTag::Struct 新增枚举 + InterpreterValue 内存管理 + ExecutionResult 扩展回调。

**技术栈：** C++17, interpreter_vm.h/cpp

**架构审核模式：** critical（涉及 IL2CPP interpreter + runtime-core 主线）

**设计文档：** 父任务 roadmap-v1-01.md

**问题清零来源：** parent-roadmap

---

## 修改清单

### `interpreter_vm.h`

1. `ValueTag::Struct = 7` 新增枚举
2. `InterpreterValue` 新增 `struct_data` (reuses void* obj slot) + `struct_size`
3. `InterpreterValue::from_struct(const void* data, uint32_t size)` — 深拷贝 struct 数据
4. `ExecutionResult` 新增 `needs_external_dispatch`, `call_target`, `call_args`
5. `ExecutionFrame` 新增析构函数（释放 struct_data）
6. `IRInstruction` 可选新增 `call_target` 字段

### `interpreter_vm.cpp`

1. `Call`/`CallVirt`/`CallBridge` — 设 needs_external_dispatch, 填充 call_target/call_args, 返回
2. `LdFld` — 支持 Struct tag（按 offset 读取字段值作为新 InterpreterValue）
3. `StFld` — 支持 Struct tag（按 offset 写入字段值）
4. `ExecutionFrame` 析构函数遍历所有值释放 struct_data

### `runtime_instantiation.cpp` (bridge)

新增 NEEDS_EXTERNAL_DISPATCH 处理路由：调用 RuntimeAbiV0.method_invoke

---

## 风险

| 风险 | 缓解 |
|------|------|
| Struct 数据深拷贝性能 | 解释器不常用路径，可接受 |
| struct_data 双重释放 | 深拷贝保证每个值独立拥有自己的数据 |
| LdFld 从 struct 读取字段类型不确定性 | 只按 offset+size 读取字节，转换为对应值 |
