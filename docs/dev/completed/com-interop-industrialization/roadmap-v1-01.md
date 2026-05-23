# COM Interop 工业化 roadmap

> **task_id**: com-interop-industrialization
> **父任务**: 无
> **归档路径**: docs/dev/in-progress/com-interop-industrialization/ → docs/dev/completed/com-interop-industrialization/

## 目标

将 COM Interop 子系统（RCW、CCW、ConnectionPoint、IDispatch、marshal COM 部分）提升至与 Memory Domain / Reflection / HotUpdate 同等级的工业化标准。

## 范围边界

- **覆盖**：com_rcw、com_ccw、com_connection_point、marshal_api.cpp COM 相关、codegen scriban CCW 模板、EventPipe、完整测试矩阵
- **不覆盖**：COM+ / MTS / ActiveX 文档对象等场景特定 COM 技术；非 Win32 平台真实 COM 调用
- **基础设施复用**：IDomainHeap（Memory Domain）、EventPipe bridge 模式（Memory Domain bridge 为模板）

## 非目标

- 非 Win32 平台的真实 COM 实现
- COM+ 事务/对象池/JIT 激活
- ActiveX 文档/控件嵌入

## 阶段列表

### Phase 1 (p1-domain-heap): IDomainHeap 集成

- **goal**: RCW/CCW/ConnectionPoint 全部使用 DomainCurrentAllocateTagged/DomainFreeTagged
- **exit_criteria**: 所有 COM 分配点替换完成，build 通过，现有测试全部通过
- **deliverables**: com_rcw.cpp 修改、com_ccw.cpp 修改、com_connection_point.cpp 修改、marshal_api.cpp COM 分配点修改
- **dependencies**: 无（memory_domain.h 已存在）
- **resolved_decisions**: 使用 DomainCurrentAllocateTagged（自动路由到 CurrentDomain），不需要为 COM 单独指定 domain
- **watch_items**: DomainFreeTagged 是否能在 RCW/CCW 释放路径上正确识别 domain header

### Phase 3 (p3-platform-layer): COM 平台抽象层

- **goal**: 创建 com_platform.h/.cpp，封装所有 Win32 COM API 调用
- **exit_criteria**: 现有 #if defined(_WIN32) 的 COM 调用迁移到平台层，build 通过，测试通过
- **deliverables**: com_platform.h（新建）、com_platform.cpp（新建）
- **dependencies**: Phase 1（使用 IDomainHeap 分配模式）
- **resolved_decisions**: 平台层使用自由函数（非虚函数），Win32 实现调用 ::CoCreateInstance 等，非 Win32 返回 E_NOTIMPL
- **watch_items**: 现有 marshal_api.cpp 中的 Win32 guard 较多，逐一迁移不要遗漏

### Phase 5 (p5-ccw-dynamic): CCW 接口表动态扩容

- **goal**: ComCcw 使用 SmallVector（内嵌 4 槽 + 超限动态 DomainAllocate 扩容）
- **exit_criteria**: RegisterCcwInterface 支持超过 4 接口，build 通过，测试通过
- **deliverables**: com_ccw.h （ComCcw 结构修改）、com_ccw.cpp（RegisterCcwInterface 扩容逻辑）
- **dependencies**: Phase 1（动态分配使用 IDomainHeap）
- **resolved_decisions**: 2 倍扩容策略；4 槽以内零堆分配
- **watch_items**: 现有 CCW 内存布局变化，检查 sizeof(ComCcw) 变化对 offsetof 计算的影响（CcwFromInterface 依赖）

### Phase 4 (p4-ccw-factory): CcwFactoryFn 运行时注册表

- **goal**: 创建 com_factory.h/.cpp，管理 CCW 工厂函数的注册/注销/查询
- **exit_criteria**: 注册表功能完成，codegen 改在模块初始化中调用 RegisterFactory，build 通过，测试通过
- **deliverables**: com_factory.h（新建）、com_factory.cpp（新建）、scriban 模板修改（模块注册段添加 RegisterFactory 调用）
- **dependencies**: Phase 5（factory 创建的 CCW 需要动态接口表支持）
- **resolved_decisions**: 使用 CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY（stableId → CcwFactoryFn）
- **watch_items**: 热更新卸载时 UnregisterFactory 是否完整

### Phase 2 (p2-dispatch-v2): IDispatch V2 + 完整 TypeLib

- **goal**: 完整 IDispatch::Invoke（含 DISPPARAMS 参数编组）+ ITypeLib/ITypeInfo 实现
- **exit_criteria**: GetTypeInfo 返回有效 ITypeInfo、GetIDsOfNames 返回正确 DISPID、Invoke 支持完整 DISPPARAMS、EXCEPINFO 异常映射，build 通过，测试通过
- **deliverables**: com_typelib.h/.cpp（新建）、com_ccw.cpp（CcwGetTypeInfo/CcwInvokeFull）、com_ccw.h（CCW typelib_data 指针）、scriban 模板修改（TypeLib 数据生成）
- **dependencies**: Phase 3（平台抽象层用于 VARIANT 操作）、Phase 4（工厂注册表完成后的 CCW 创建路径）
- **resolved_decisions**: codegen 生成 ComTypeLibData/ComTypeInfoData 静态数据；运行时 ComTypeInfoImpl 包装；Invoke 参数编组复用 marshal_api.cpp VARIANT 转换；非核心 ITypeInfo 方法返回 E_NOTIMPL
- **watch_items**: DISPPARAMS rgvarg 逆序约定；byref 参数回写

### Phase 6 (p6-eventpipe): EventPipe COM 桥接

- **goal**: 创建 ep_com_bridge.h/.cpp，注册 COM 事件（0x0700 范围）
- **exit_criteria**: 7 个 COM 事件类型定义完成，桥接初始化/关闭在 EpInitialize/EpShutdown 中注册，build 通过
- **deliverables**: ep_com_bridge.h（新建）、ep_com_bridge.cpp（新建）、ep_protocol.h 修改、ep_event.h/.cpp 修改、ep_instance.cpp 修改
- **dependencies**: Phase 1（事件 payload 需要 DomainId）
- **resolved_decisions**: 7 事件类型：RcwCreated/RcwReleased/CcwCreated/CcwReleased/ConnectionPointAdvise/ConnectionPointUnadvise/DispatchInvoke

### Phase 7 (p7-tests): 测试工业化

- **goal**: 补齐压力/基准/managed 测试，达到 Memory Domain 同等级测试覆盖
- **exit_criteria**: 所有测试 build 通过、运行通过、benchmark 数据锚点记录
- **deliverables**: com_rcw_stress_test.cpp、com_ccw_stress_test.cpp、com_typelib_test.cpp、com_dispatch_test.cpp、com_platform_test.cpp、com_factory_test.cpp、com_benchmark_test.cpp、CMakeLists.txt 修改
- **dependencies**: 全部 Phase 1-6
- **resolved_decisions**: stress 使用 100 并发线程；benchmark 记录基线；managed 测试沿用 marshal Phase G 计划

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|--------------|--------------|---------------|----------------|-----------------|
| p1-domain-heap | Phase 1 | ready | main | IDomainHeap 集成 | 无 | 所有 COM 分配点替换 | com_rcw.cpp, com_ccw.cpp, com_connection_point.cpp, marshal_api.cpp | build + 现有 69 tests 通过 | src/native/runtime-core/com_*.cpp, core/marshal_api.cpp | 1 天 |
| p2-dispatch-v2 | Phase 2 | completed | main | IDispatch V2 + TypeLib | p3, p4 | ITypeLib/ITypeInfo + DISPPARAMS Invoke | com_typelib.h/.cpp, scriban 修改, com_ccw.cpp | build + 测试通过 | src/native/runtime-core/com_typelib.*, com_ccw.*, *.scriban | 5 天 |
| p3-platform-layer | Phase 3 | planned | main | COM 平台抽象层 | p1 | Win32 COM API 封装 | com_platform.h/.cpp | build + 现有测试通过 | src/native/runtime-core/com_platform.* | 1 天 |
| p4-ccw-factory | Phase 4 | planned | main | CcwFactoryFn 注册表 | p5 | 注册/注销/查询 | com_factory.h/.cpp, scriban 修改 | build + 测试通过 | src/native/runtime-core/com_factory.*, *.scriban | 1 天 |
| p5-ccw-dynamic | Phase 5 | planned | main | CCW SmallVector 动态接口表 | p1 | inline[4] + 动态扩容 | com_ccw.h, com_ccw.cpp | build + 现有测试通过 | src/native/runtime-core/com_ccw.* | 1 天 |
| p6-eventpipe | Phase 6 | planned | main | EventPipe COM 桥接 | p1 | 7 事件 + bridge | ep_com_bridge.h/.cpp, ep_protocol.h, ep_event.*, ep_instance.cpp | build 通过 | src/native/diagnostics/eventpipe/ep_com_bridge.* | 1 天 |
| p7-tests | Phase 7 | planned | main | 测试工业化 | p1-p6 | stress/benchmark/managed | 8 新测试文件 + CMakeLists | 全部测试通过 + benchmark 基线 | testing/src/native/ | 2 天 |

## 执行顺序

```
p1-domain-heap (Phase 1)
    │
    ▼
p3-platform-layer (Phase 3)
    │
    ▼
p5-ccw-dynamic (Phase 5)
    │
    ▼
p4-ccw-factory (Phase 4)
    │
    ▼
p2-dispatch-v2 (Phase 2)
    │
    ▼
p6-eventpipe (Phase 6)
    │
    ▼
p7-tests (Phase 7)
```

## 风险

| 风险 | 级别 | 缓解 |
|------|------|------|
| IDispatch V2 仅核心方法实现，常被注意的非核心方法返回 E_NOTIMPL 导致兼容性问题 | 🟡 中 | 文档记录已知限制 |
| Phase 5 ComCcw 内存布局变化破坏 CcwFromInterface 的 offsetof 计算 | 🟡 中 | 验证 sizeof(ComCcw) 和 offsetof(vtable) 不变 |
| marshal_api.cpp (1263 行) 中的 COM 相关 Win32 guard 分散，迁移遗漏 | 🟢 低 | 每迁移一个函数立即 build 验证 |
| marshalling VARIANT 转换与现有 marshal_api 重复逻辑 | 🟢 低 | 复用现有函数，不重复实现 |

## 当前建议推进顺序

串行推进，从 p1-domain-heap 开始。

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- 每个子任务先用 dev-writing-plans 做最小 preflight self-review
- preflight pass|warn → 自动继续执行
- preflight stop → 停止并说明
- 子任务完成后必须物理移入 docs/dev/completed/ 并回写父 STATUS.md
