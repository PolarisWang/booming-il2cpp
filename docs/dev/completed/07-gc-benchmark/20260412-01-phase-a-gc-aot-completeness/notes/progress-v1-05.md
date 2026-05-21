# Progress v1-05: Batch 5 完成

**时间**: 2026-04-12
**Batch**: 5 — 完整异常处理 + 泛型运行时

## 产出

### 完整异常处理
- `InterpreterIR.cs`:
  - `IRExceptionRegionKind` 新增 `Filter` 和 `Fault`
  - `IRExceptionRegion` 新增 `FilterBlockId` 属性
  - `IROpCode` 新增 `EndFilter`
- `ManagedInterpreterExecutor.cs`:
  - `TryResolveCatchHandler` 扩展支持 Filter (保守接受) 和 Fault
  - `EndFilter` opcode handler 添加

### 泛型运行时
- `src/native/runtime-core/generic_context.h` 新增
- `src/native/runtime-core/generic_context.cpp` 新增
- `runtime-core/CMakeLists.txt` 添加 generic_context.cpp
- `runtime_core.cpp`: `MethodGetGenericContext` 实现查 generic_context 注册表
- `bootstrap.cpp`: 添加 generic_context.h include + BootstrapRuntime 注释占位

### 新增 Subjects
- `subjects/CrossBoundaryExceptionProof/` — 4 场景: 基础 catch, filter when, 嵌套 finally (throw), 嵌套 finally (正常)
- `subjects/GenericCollectionProof/` — List<int>, Dictionary<string,int>, List<string>, 泛型方法

## 注意

- Filter 当前是保守接受 (总是匹配)，完整 filter 求值需要在子 context 执行 filter block — 留后续
- generic_types/generic_methods struct 布局由 codegen 定义，bootstrap 注册在 A.11 完成后

## 验证结果

- cmake 构建成功 (仅 C4819 中文注释警告) ✓
- 所有 5 个 GC tests 通过 ✓

## 下一步

Batch 6 — 委托多播 + IL opcode 扩展
