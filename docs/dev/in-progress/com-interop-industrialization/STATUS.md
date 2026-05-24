---
task_id: com-interop-industrialization
task_type: roadmap
phase: roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
design_doc: n/a
clearance_source: brainstorm
---

## 设计摘要

COM Interop 工业化补齐，对标 Memory Domain / Reflection / HotUpdate 的工业化完成度。

### 已覆盖

- com_platform.h/.cpp — COM 平台抽象层
- com_abi.h — Win32 COM ABI 类型定义
- com_ccw.h/.cpp — CCW 完整实现（CreateCcw、CcwQueryInterface、CcwDispatchMethodInvoke 含 DISPPARAMS→managed）
- com_rcw.h/.cpp — RCW 完整实现
- com_connection_point.h/.cpp — 连接点
- com_factory.h/.cpp — CcwFactoryFn 运行时注册表
- com_typelib.h/.cpp — ITypeInfo + ITypeLib 完整 vtable
- ep_com_bridge.h/.cpp — EventPipe COM 桥接
- 10 个 COM 测试注册在 CMakeLists.txt

### 已识别差距

1. CCW GetTypeInfoCount/GetTypeInfo 为 stub（com_ccw.cpp），phase2 有真实实现
2. 测试构建 & 运行验证
3. Managed COM interop AOT 集成测试

## 阶段

### Phase 1: CCW Phase2 升级
用 com_ccw.cpp.phase2 替换 com_ccw.cpp，合并 address-distance sanity check。

### Phase 2: 测试构建 & 验证
构建所有 COM 测试并运行通过。

### Phase 3: Managed COM AOT 集成测试
创建 managed COM AOT 端到端测试。

## 验证

```bash
cmake -S testing -B build/testing
cmake --build build/testing --config RelWithDebInfo
ctest --test-dir build/testing -C RelWithDebInfo -R "com_"
```

## 最新摘要

- Phase 1: CCW Phase2 升级 ✅ — 用 `com_ccw.cpp.phase2` 替换当前版本，新增 ResolveCcw 地址距离检查 + ITypeInfo/ITypeLib QI 真实实现（原为 stub）
- Phase 2: 测试构建 & 验证 ✅ — 全部 11 个 COM 测试构建通过，ctest 10/10 通过（100%）
  - 132 个测试用例（含 9 个 DISABLED）：CCW(28) + RCW(17) + ConnectionPoint(22) + Platform(13) + Factory(10) + TypeLib(8) + Dispatch(8) + Benchmark(5) + CCW Stress(4) + RCW Stress(4) + Integration(13)
  - 9 个 DISABLED connection point 测试需要 MemoryDomain 基础设施
  - com_integration_test 需要 chaos_codegen.lib（主构建系统补齐）
- Wiki 已更新：版本号 V3→V4，测试计数刷新，运行方法统一
- P0 差距（G1 malloc/free、G2 LOG_WARN stubs）已在 phase2 中自然解决
