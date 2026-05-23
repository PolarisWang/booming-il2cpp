---
task_id: com-interop-industrialization
task_type: roadmap
phase: roadmap
lifecycle_status: in-progress
roadmap_or_plan: roadmap-v1-01.md
design_doc: n/a (design summary in STATUS.md)
arch_review_mode: critical
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
clearance_source: brainstorm-design-confirmed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
recommended_next_child: p6-eventpipe
latest_stop_point: p2-dispatch-v2 已完成 — com_typelib.h/.cpp TypeLib 完整实现（ITypeInfo/ITypeLib vtable、ComTypeInfoImpl 包装），CcwDispatchMethodInvoke DISPPARAMS 参数编组（VARIANT→managed object 转换→method_invoke），scriban 模板生成 ComTypeLibData/ComMethodData 静态描述符。build 通过（chaos_runtime_core.lib），28/28 com_ccw 测试通过，11 com_rcw 测试通过（6 skip expected）。
---

# COM Interop 工业化完善

## 设计摘要

### 边界拍板
- **覆盖范围**：com_rcw, com_ccw, com_connection_point, marshal_api COM 部分, codegen scriban 模板, EventPipe 桥接, 完整测试矩阵
- **不覆盖**：COM+ / MTS / ActiveX 文档对象等场景特定 COM 技术；非 Win32 平台的真实 COM 调用（仅 mock 测试）
- **IDispatch V2**：完整实现 ITypeLib/ITypeInfo，包含 GetTypeAttr/GetFuncDesc/GetIDsOfNames/Invoke（含 DISPPARAMS 参数编组、EXCEPINFO 异常映射）
- **COM 平台抽象层**：封装 Win32 COM API 调用到 com_platform.h/.cpp，非 Win32 返回 E_NOTIMPL
- **CCW 接口表**：SmallVector 模式（内嵌 4 槽 + 超限动态扩容）
- **RCW 缓存**：保留 8-slot 线性扫描（已验证足够）

### 结构决策
1. IDomainHeap 集成：RCW/CCW/ConnectionPoint 的 `std::malloc`/`std::free` → `DomainCurrentAllocateTagged`/`DomainFreeTagged`
2. IDispatch V2 ：codegen 生成 ComTypeLibData/ComTypeInfoData 静态描述符，运行时 ComTypeInfoImpl 实现 ITypeInfo 接口
3. COM 平台抽象：`com_platform.h/.cpp` 隔离 `#if defined(_WIN32)` 调用
4. CcwFactoryFn 运行时注册：`com_factory.h/.cpp` 哈希表注册/注销/查询
5. EventPipe 桥接：`ep_com_bridge.h/.cpp`，事件范围 0x0700
6. CCW SmallVector：`ComCcw.inline_interfaces[4]` + `dynamic_interfaces*`，2 倍扩容

### 测试策略
- 单元测试：TypeInfo 查询、GetIDsOfNames 哈希、Invoke 参数编组、Factory 注册/注销、Platform 抽象层 mock
- 压力测试：并发 RCW 创建/释放、CCW AddRef/Release 高压
- 基准测试：RCW 查找、CCW QI、Dispatch Invoke 延迟
- Managed 测试：Phase G 12 managed 测试（复用 marshal Phase G 计划）

## 三优先级权衡结论

| 决策 | P1 (性能) | P2 (架构) | P3 (HotUpdate) | 裁决 |
|------|-----------|-----------|----------------|------|
| IDomainHeap 集成 | 8-16B header 开销，COM 非热路径 | Memory Domain 对齐 | 域卸载可追踪 COM 分配 | 明确收益 |
| CCW SmallVector | 零开销（多数不触发 realloc） | 略复杂 | 支持新类型注册 | 推荐 |
| IDispatch V2 TypeLib | Invoke 参数解包固有开销 | 完整 COM 合规 | N/A | 用户要求 |
| COM 平台抽象层 | 1 次间接调用 | 干净分层 | 未来可扩展 | 用户要求 |
| CcwFactory 注册表 | 1 次哈希查询 | 运行时元数据 | 热更新新增类型的 CCW 创建 | 必须做 |
| RCW cache 保留 8-slot | 热路径零开销 | 保持简单 | N/A | 保留现状 |
| EventPipe 桥接 | PROFILE/SHIP 编译消除 | 对标 Memory Domain | N/A | 必须做 |

## 风险评估摘要

| 风险 | 级别 | 缓解 |
|------|------|------|
| IDispatch V2 TypeLib 实现量大 | 🟡 中 | 核心方法完整实现，其余返回 E_NOTIMPL |
| COM 平台抽象层引入虚函数调用 | 🟢 低 | 非热点路径，可 inline |
| DISPPARAMS 参数编组与 marshal 层重复 | 🟡 中 | 复用 marshal_api.cpp VARIANT 转换逻辑 |
| CCW 动态扩容并发悬挂指针 | 🟡 中 | realloc 后更新 CCW 指针而非原地缩扩 |
| Phase 3 平台层与 Win32 guards 混合遗漏 | 🟢 低 | 逐步迁移，每点验证 |

## Phase 依赖关系

```
Phase 1 (IDomainHeap) ▶ Phase 3 (Platform) ▶ Phase 5 (Dynamic CCW) ▶ Phase 4 (Factory) ▶ Phase 2 (Dispatch) ▶ Phase 6 (EventPipe) ▶ Phase 7 (Tests)
```

## 最近摘要

- **2026-05-23**: brainstorm 完成，所有边界拍板和问题清零。IDispatch V2 完整 TypeLib + COM 平台抽象层已确认。7 Phase 串行 roadmap 编制完成。
- **2026-05-23 (p3-platform-layer)**: com_platform.h/.cpp 创建，封装 IUnknown QI/AddRef/Release、CoCreateInstance、BSTR。4 源文件迁移到平台层。49/49 测试通过。

## 下一步

启动 p2-dispatch-v2：IDispatch V2 完整 TypeLib 实现 — ITypeLib/ITypeInfo 接口，codegen 生成 ComTypeLibData/ComTypeInfoData 静态描述符，运行时 ComTypeInfoImpl 包装，Invoke 支持完整 DISPPARAMS 参数编组。
