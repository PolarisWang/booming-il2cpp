# IL2CPP 后续开发计划: Phase A-E 全量实现方案 (v1.02 — checkpoint 细化版)

> **面向执行 Agent：** 每个 checkpoint 必须逐项完成并标记，不得跳过。验收标准为硬性条件。

## Context

当前 20260409-10 计划进行中，Phase 0-7 主体完成。项目目标: C# 游戏脚本 → IL2CPP → 多平台 + 热更新，核心追求高性能运行环境。本计划定义 Phase 7 完成后的 5 个阶段。

## 依赖关系图

```
Phase A (GC + AOT 补完)
  ├──→ Phase B (三方 Benchmark) ← A 中期可启动
  ├──→ Phase C (热更端到端)     ← 依赖 A 完成
  │       └──→ Phase D (多平台真机) ← 依赖 A+C
  └──────────→ Phase E (生产化 + CI/CD) ← 依赖全部

Phase A 内部:
  A.1 → A.2 → A.3 → A.5
              → A.4 → A.6
       → A.7 (并行) → A.8, A.9 → A.10 → A.11
```

---

## Phase A: GC 集成 + AOT 运行时关键补完

**目标**: BDWGC 接入，generated C++ 在 Windows 上跑通真实 C# 代码

**Phase 退出标准**:
- [ ] BDWGC 增量 GC 正常运行，GC_get_gc_no() > 0
- [ ] MainlineFeaturePack 全部 capability (dispatch, generic, array, boxing, delegate, exception) 在 Windows x64 通过
- [ ] 连续分配 100 万对象无崩溃、无内存泄漏 (RSS 增长 < 50MB)
- [ ] ThreadingProof 多线程场景通过
- [ ] 所有已有 subject (HelloWorldObject, GenericEcho, ReflectionLite, PInvokeLite) 无回归

---

### A.1 BDWGC 构建集成

**创建**: `third_party/bdwgc/CMakeLists.txt`

- [ ] **A.1.1** 创建 `third_party/bdwgc/CMakeLists.txt`，列出核心源文件:
  - alloc.c, allchblk.c, blacklst.c, dbg_mlc.c, finalize.c, fnlz_mlc.c, gc_dlopen.c, gcj_mlc.c, headers.c, mach_dep.c, malloc.c, mallocx.c, mark.c, mark_rts.c, misc.c, new_hblk.c, obj_map.c, os_dep.c, ptr_chck.c, reclaim.c, specific.c, thread_local_alloc.c, typd_mlc.c
- [ ] **A.1.2** Windows 平台源文件: win32_threads.c; 定义 GC_WIN32_THREADS
- [ ] **A.1.3** Unix 平台源文件: pthread_support.c, pthread_stop_world.c; Apple 额外: darwin_stop_world.c
- [ ] **A.1.4** 编译定义: GC_THREADS, ALL_INTERIOR_POINTERS, GC_BUILTIN_ATOMIC, PARALLEL_MARK, THREAD_LOCAL_ALLOC, GC_NOT_DLL (静态链接)
- [ ] **A.1.5** 导出 target `chaos_bdwgc`，include 目录指向 `third_party/bdwgc/include`
- [ ] **A.1.6** 修改 `CMakeLists.txt` 根文件，在 windows-x64-reference 路径添加 `add_subdirectory(third_party/bdwgc)`
- [ ] **A.1.7** 修改 `src/native/runtime-core/CMakeLists.txt`: `target_link_libraries(chaos_runtime_core PUBLIC chaos_bdwgc)`
- [ ] **A.1.8** 创建 `tests/contracts/native/gc/CMakeLists.txt` + `gc_init_smoke.cpp`: 仅调用 GC_INIT() + GC_MALLOC(16) + 验证返回非 NULL

**验收标准**:
- `cmake --preset windows-x64-reference` 配置成功，无错误
- `cmake --build` 编译成功，chaos_bdwgc.lib 生成
- gc_init_smoke.exe 运行输出 "GC init OK"，退出码 0

---

### A.2 替换 AllocateBytes 为 GC_malloc

**修改**: `src/native/runtime-core/runtime_core.cpp`

- [ ] **A.2.1** 添加 `#include <gc.h>` 到 runtime_core.cpp 头部
- [ ] **A.2.2** 在 `RuntimeInit()` 中 TryNormalizeConfig 之后、分配 RuntimeState 之前，添加 `GC_INIT()`
- [ ] **A.2.3** 修改 `DefaultAllocate()`: `return GC_MALLOC(size);` 替换 `std::malloc(size)`
- [ ] **A.2.4** 修改 `DefaultDeallocate()`: 函数体改为空 (BDWGC 自动回收，保留函数签名不变)
- [ ] **A.2.5** 新增 `DefaultAllocateAtomic()` 辅助函数: `return GC_MALLOC_ATOMIC(size);` (用于无指针的 payload)
- [ ] **A.2.6** 修改 `StringNewUtf8()`: 字符串数据部分 (type 后面的 byte payload) 使用 GC_MALLOC_ATOMIC
- [ ] **A.2.7** 修改 `ArrayNew()`: 如果是值类型元素数组 (未来扩展点)，payload 用 GC_MALLOC_ATOMIC
- [ ] **A.2.8** 修改 `BoxValueObject()`: 值数据部分使用 GC_MALLOC_ATOMIC
- [ ] **A.2.9** 确认 `RuntimeShutdown()` 中不再调用 FreeBytes 释放 RuntimeState (让 GC 收集)
- [ ] **A.2.10** 运行已有 subjects: HelloWorldObject, GenericEcho, ReflectionLite, PInvokeLite

**验收标准**:
- HelloWorldObject subject 在 windows-x64-reference 输出与之前一致
- GenericEcho subject 通过
- ReflectionLite, PInvokeLite 通过
- 无 ASAN/MSAN 错误 (如果启用)
- 进程退出码 0，无 crash

---

### A.3 实现 GcHandleNew/Free

**修改**: `src/native/runtime-core/runtime_core.cpp`

- [ ] **A.3.1** 新增全局结构:
  ```cpp
  struct GcHandleEntry { void* object_instance; bool pinned; };
  static std::mutex g_gc_handle_mutex;
  static std::atomic<uint64_t> g_next_gc_handle{1};
  static std::unordered_map<uint64_t, GcHandleEntry> g_gc_handle_table;
  ```
- [ ] **A.3.2** 实现 `GcHandleNew()`:
  - 验证参数非空
  - 加锁，分配递增 handle ID
  - 存入 g_gc_handle_table
  - 对 pinned handle 调用 `GC_add_roots(obj, obj + sizeof(ObjectHeader))`
  - 返回 handle ID
- [ ] **A.3.3** 实现 `GcHandleFree()`:
  - 验证 handle 非 INVALID
  - 加锁，查找并移除
  - 对 pinned handle 调用 `GC_remove_roots()`
- [ ] **A.3.4** 新增 `GcHandleGetTarget()` 辅助函数: 根据 handle 返回 object_instance 指针
- [ ] **A.3.5** 创建 `tests/contracts/native/gc/gc_handle_smoke.cpp`:
  - 分配对象 → GcHandleNew(pinned=false) → 返回值 != CHAOS_GC_HANDLE_INVALID
  - GcHandleGetTarget() 返回原始指针
  - GcHandleFree() → 再次 GetTarget 返回 nullptr
  - GcHandleNew(pinned=true) → GC_gcollect() → 对象仍然存活
- [ ] **A.3.6** 在 RuntimeShutdown 中清空 g_gc_handle_table

**验收标准**:
- gc_handle_smoke 测试全部断言通过
- GcHandleNew 返回值 > 0 (非 INVALID)
- pinned 对象在 GC_gcollect() 后仍可访问
- 无内存泄漏 (handle 释放后 table size 归零)

---

### A.4 线程注册

**修改**: `src/native/runtime-core/runtime_core.cpp`

- [ ] **A.4.1** 在 `ThreadAttach()` 中，分配 ThreadState 之后:
  - 调用 `GC_get_stack_base(&sb)` 获取当前线程栈基址
  - 调用 `GC_register_my_thread(&sb)` 注册线程
  - 如果返回 GC_DUPLICATE，忽略 (主线程可能已被 GC_INIT 注册)
- [ ] **A.4.2** 在 `ThreadDetach()` 中，释放 ThreadInternalState 之后:
  - 调用 `GC_unregister_my_thread()`
- [ ] **A.4.3** 创建 `tests/contracts/native/gc/gc_thread_smoke.cpp`:
  - 主线程: ThreadAttach → 分配对象 → ThreadDetach
  - 工作线程: std::thread 中 ThreadAttach → 分配 1000 对象 → ThreadDetach
  - 主线程等待工作线程结束 → GC_gcollect() → 无 crash
- [ ] **A.4.4** 扩展 ThreadingProof subject 验证多线程 GC 场景

**验收标准**:
- gc_thread_smoke 测试通过，退出码 0
- 3 个并发工作线程各分配 1000 对象，无 crash
- GC_gcollect() 调用后无 segfault
- ThreadingProof subject 通过

---

### A.5 Finalizer 映射

**修改**: `src/native/runtime-core/runtime_core.cpp`

- [ ] **A.5.1** 在 `EnqueueFinalizer()` 中，除了现有 queue 逻辑外，额外调用:
  ```cpp
  GC_register_finalizer_no_order(object_instance,
      [](void* obj, void* cd) { reinterpret_cast<FinalizerCallback>(cd)(obj); },
      reinterpret_cast<void*>(finalizer), nullptr, nullptr);
  ```
- [ ] **A.5.2** 在 `DrainFinalizerQueue()` 中，额外调用 `GC_invoke_finalizers()` 冲刷 BDWGC 待执行 finalizer
- [ ] **A.5.3** 创建 `tests/contracts/native/gc/gc_finalizer_smoke.cpp`:
  - 分配对象 → EnqueueFinalizer(counter++) → 置空引用 → GC_gcollect() × 3 → DrainFinalizerQueue()
  - 验证 counter > 0 (finalizer 被调用)
- [ ] **A.5.4** 验证 finalizer 被调用顺序: 后分配的对象 finalizer 不保证顺序 (BDWGC 特性)

**验收标准**:
- gc_finalizer_smoke 中 counter == 已注册 finalizer 数量
- 无 double-free 或 use-after-free
- 无 crash

---

### A.6 Safepoint → GC_collect_a_little

**修改**: `src/native/runtime-core/runtime_core.cpp`

- [ ] **A.6.1** 修改 `GcSafepoint()`:
  ```cpp
  tis->at_gc_safepoint = true;
  GC_collect_a_little();
  tis->at_gc_safepoint = false;
  ```
- [ ] **A.6.2** 创建 `tests/contracts/native/gc/gc_safepoint_smoke.cpp`:
  - 记录 GC_get_gc_no() 初始值
  - 循环: 分配 100 对象 → GcSafepoint() × 1000 次
  - 记录 GC_get_gc_no() 最终值
  - 验证 final - initial > 0 (至少发生了一次增量收集)
- [ ] **A.6.3** 验证 safepoint 不会导致明显停顿: 单次 GcSafepoint 耗时 < 1ms (非 Full GC)

**验收标准**:
- GC_get_gc_no() 增长 > 0
- 1000 次 safepoint 总耗时 < 500ms
- 无 crash

---

### A.7 虚方法分派 (vtable)

**修改**: runtime-core + bootstrap + managed codegen

- [ ] **A.7.1** 在 `src/native/runtime-core/runtime_core.h` 定义:
  ```cpp
  struct VTableSlot { uint32_t method_token; void* method_pointer; };
  struct TypeVTable { TypeInfoHandle type; uint32_t slot_count; const VTableSlot* slots; TypeInfoHandle base_type; };
  ```
- [ ] **A.7.2** 创建 `src/native/runtime-core/vtable_registry.h` + `.cpp`:
  - `bool RegisterTypeVTable(RuntimeState*, const TypeVTable* vtable)`
  - `void* ResolveVirtualMethod(RuntimeState*, TypeInfoHandle instance_type, uint32_t method_token)`
  - 内部存储: `unordered_map<TypeInfoHandle, const TypeVTable*>`
  - 查找逻辑: 从 instance_type 开始，沿 base_type 链向上查找 method_token
- [ ] **A.7.3** 修改 `src/native/runtime-core/CMakeLists.txt`: 添加 vtable_registry.cpp 到源列表
- [ ] **A.7.4** 修改 `src/native/bootstrap/bootstrap.cpp`:
  - `ResolveVirtualMethod()` 委托到 vtable_registry
  - `InvokeVirtual()` 使用 ResolveVirtualMethod 拿到函数指针后调用
- [ ] **A.7.5** 修改 `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`:
  - 在 code-registration.json 中输出每个类型的 vtable slot 列表
  - 格式: `"vtables": [{ "type": "...", "slots": [{ "token": N, "methodRef": "..." }] }]`
- [ ] **A.7.6** 修改 `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`:
  - 生成 C++ vtable 初始化代码: `static VTableSlot vtable_slots_TypeName[] = { ... };`
  - 在 bootstrap 注册调用中添加 `RegisterTypeVTable(&vtable_TypeName)`
- [ ] **A.7.7** 创建 subject `VTableDispatchProof`:
  - 基类 Animal 有虚方法 Speak() → "animal"
  - 派生类 Dog : Animal 重写 Speak() → "dog"
  - 派生类 Cat : Animal 重写 Speak() → "cat"
  - 通过 Animal 引用调用 Speak()，验证多态分派
- [ ] **A.7.8** 扩展 InterfaceDispatchProof:
  - 接口 IShape.Area() → 多个实现类 Circle, Rectangle
  - 通过 IShape 引用调用 Area()，验证接口分派
- [ ] **A.7.9** 验证 MainlineFeaturePack dispatch capability 通过

**验收标准**:
- VTableDispatchProof: Dog.Speak() 返回 "dog", Cat.Speak() 返回 "cat" (通过基类引用)
- InterfaceDispatchProof: Circle.Area() 和 Rectangle.Area() 返回正确值
- MainlineFeaturePack dispatch 验证点通过
- 无额外内存泄漏 (vtable 生命周期跟随 runtime)

---

### A.8 完整异常处理

**修改**: runtime-core + interpreter

- [ ] **A.8.1** 在 `InterpreterIR.cs` 扩展 `IRExceptionRegionKind`:
  - 添加 `Filter` (filter block 求值后决定是否 catch)
  - 添加 `Fault` (无论是否异常都执行，类似 finally 但不吞异常)
- [ ] **A.8.2** 在 `ILToIRLowering.cs` 添加 IL → IR lowering:
  - `endfilter` → IROpCode.EndFilter (弹出 filter 结果 0/1)
  - fault handler block → 标记为 IRExceptionRegionKind.Fault
- [ ] **A.8.3** 在 `ManagedInterpreterExecutor.cs` 扩展异常分派:
  - Filter: 先执行 filter block → 如果返回 1 则进入 catch handler，返回 0 则跳过
  - Fault: 在离开 try block 时无条件执行 (无论正常/异常退出)
- [ ] **A.8.4** 在 `runtime_core.cpp` 扩展 `MethodInvoke` 异常路径:
  - catch (ManagedExceptionCarrier& e) 中支持 filter 求值回调
- [ ] **A.8.5** 实现跨边界异常传播:
  - AOT → Interpreter: AOT 抛出 ManagedExceptionCarrier → interpreter 的 bridge invoker catch 并转为 interpreter 异常
  - Interpreter → AOT: interpreter throw → ManagedExceptionCarrier C++ 异常 → AOT catch
- [ ] **A.8.6** 扩展 NestedExceptionProof:
  - 添加 filter block 场景: try { throw; } catch when (FilterMethod()) { ... }
  - 添加 fault block 场景: try { ... } fault { cleanup(); }
  - 添加嵌套 try/catch/finally 三层嵌套
- [ ] **A.8.7** 创建 CrossBoundaryExceptionProof:
  - AOT 方法调用 interpreter 方法，interpreter 方法 throw → AOT catch 正确
  - Interpreter 方法调用 AOT 方法，AOT 方法 throw → interpreter catch 正确
  - 三层跨越: AOT → interpreter → AOT → throw → 逐层回退到最外层 catch

**验收标准**:
- NestedExceptionProof 全部验证点通过 (包含 filter=accept, filter=reject, fault)
- CrossBoundaryExceptionProof 全部 3 个场景通过
- 异常消息字符串在跨边界后保持正确
- 无 unhandled exception crash
- finally block 在所有路径 (正常退出、异常退出、嵌套异常) 都执行

---

### A.9 泛型运行时

**修改**: runtime-core + bootstrap + codegen

- [ ] **A.9.1** 创建 `src/native/runtime-core/generic_context.h`:
  ```cpp
  struct GenericTypeInstantiation { TypeInfoHandle open_type; TypeInfoHandle* type_args; uint32_t arg_count; TypeInfoHandle closed_type; };
  struct GenericMethodInstantiation { MethodInfoHandle open_method; TypeInfoHandle* type_args; uint32_t arg_count; MethodInfoHandle closed_method; };
  struct GenericContext { const GenericTypeInstantiation* class_inst; const GenericMethodInstantiation* method_inst; };
  ```
- [ ] **A.9.2** 创建 `src/native/runtime-core/generic_context.cpp`:
  - `GenericContextHandle CreateGenericContext(...)` — 创建并缓存泛型上下文
  - `TypeInfoHandle ResolveGenericType(open_type, type_args, arg_count)` — 查找或创建闭合类型
  - 内部缓存: `map<(open_type, type_args...), closed_type>`
- [ ] **A.9.3** 修改 `runtime_core.cpp`: `MethodGetGenericContext()` 从 stub (返回 null) 改为查找已注册的 GenericContext
- [ ] **A.9.4** 修改 `bootstrap.cpp`:
  - 在 metadata registration 阶段注册泛型实例化: `RegisterGenericInstantiation(open_type, closed_type, type_args)`
  - MetadataRegistrationV0 的 generic_types / generic_methods 数组正确消费
- [ ] **A.9.5** 修改 `CodeGenStage.cs`:
  - 在 code-registration.json 输出泛型实例化表
  - 每个泛型实例: open type + type arguments + closed type token
- [ ] **A.9.6** 修改 `NativeReferenceLoweringPlanner.cs`:
  - 生成泛型方法体的 C++ 代码 (共享实现 + 泛型上下文参数)
  - 类型参数通过 GenericContext 传递
- [ ] **A.9.7** 验证 GenericEcho subject: `Echo<int>(42)` → 42, `Echo<string>("hello")` → "hello"
- [ ] **A.9.8** 验证 GenericSupplementProof: 泛型方法的热更补充元数据正确
- [ ] **A.9.9** 创建 GenericCollectionProof:
  - `List<int>.Add(1); list.Count == 1`
  - `Dictionary<string, int>["key"] = 42; dict["key"] == 42`
  - 嵌套泛型: `List<List<int>>`

**验收标准**:
- GenericEcho subject 通过 (int, string 两种类型参数)
- GenericSupplementProof 通过
- GenericCollectionProof 通过 List, Dictionary, 嵌套泛型
- MethodGetGenericContext() 返回非 null (对于泛型方法)
- 泛型实例化缓存命中率 > 0 (第二次调用走缓存)

---

### A.10 委托完整

**修改**: bootstrap

- [ ] **A.10.1** 扩展 `DelegateInstance` 结构 (bootstrap.cpp):
  - 添加 `DelegateInstance* next` — multicast 链表
  - 添加 `uint32_t invocation_count` — 调用列表长度
- [ ] **A.10.2** 实现 `CombineDelegate(left, right)`:
  - 克隆 left 的调用链
  - 将 right 追加到链尾
  - 返回新 DelegateInstance 头
- [ ] **A.10.3** 实现 `RemoveDelegate(source, target)`:
  - 从 source 调用链中移除匹配 target 的最后一个
  - 返回新链 (如果为空返回 null)
- [ ] **A.10.4** 修改 `DelegateInvoke`:
  - 遍历 multicast chain，逐个调用
  - 返回最后一个调用的返回值 (C# 语义)
- [ ] **A.10.5** 注册到 CodegenBridgeV0 (如果 v0 冻结，通过 icall 机制暴露)
- [ ] **A.10.6** 扩展 MainlineFeaturePack delegate capability:
  - 单播委托调用 ✓ (已有)
  - 多播委托: Combine → Invoke → 全部执行
  - Remove: 移除后仅剩余委托执行
  - event 模式: += / -= 操作
- [ ] **A.10.7** 创建 DelegateChainProof:
  - Action<int> 三个实例 Combine → Invoke → 三个都执行 → 计数器 == 3
  - Remove 最后一个 → Invoke → 计数器 == 2

**验收标准**:
- DelegateChainProof: multicast invoke 调用所有委托，计数正确
- Remove 后调用列表正确缩减
- Combine(null, d) == d, Combine(d, null) == d
- 空委托 Invoke 不 crash (返回 default)
- MainlineFeaturePack delegate capability 全部验证点通过

---

### A.11 代码生成 IL opcode 扩展

**修改**: CodeGen + Interpreter

- [ ] **A.11.1** 类型转换 opcodes (conv family):
  - conv.i1, conv.i2, conv.i4, conv.i8, conv.u1, conv.u2, conv.u4, conv.u8
  - conv.r4, conv.r8, conv.r.un
  - 创建 ConversionOpsProof subject
- [ ] **A.11.2** 位运算 opcodes:
  - and, or, xor, shl, shr, shr.un, not
  - 创建 BitwiseOpsProof subject
- [ ] **A.11.3** 扩展比较/分支 opcodes:
  - beq, bne.un, bge, bge.un, ble, ble.un, bgt.un, blt.un
  - ceq, clt, cgt, clt.un, cgt.un (部分已有)
  - 创建 BranchOpsProof subject
- [ ] **A.11.4** 对象操作 opcodes:
  - newobj, initobj, ldobj, stobj, cpobj, sizeof
  - isinst, castclass (类型检查/转换)
  - 创建 ObjectOpsProof subject
- [ ] **A.11.5** 间接操作 opcodes:
  - ldind.i1/i2/i4/i8/u1/u2/u4/u8/r4/r8/ref
  - stind.i1/i2/i4/i8/r4/r8/ref
  - 创建 IndirectOpsProof subject
- [ ] **A.11.6** 数组操作 opcodes:
  - ldelema (取元素地址)
  - ldelem.i1/i2/i4/i8/u1/u2/u4/u8/r4/r8/ref
  - stelem.i1/i2/i4/i8/r4/r8/ref
  - 创建 ArrayOpsProof subject
- [ ] **A.11.7** 溢出检查 opcodes:
  - add.ovf, add.ovf.un, sub.ovf, sub.ovf.un, mul.ovf, mul.ovf.un
  - conv.ovf.i1/i2/i4/i8/u1/u2/u4/u8
  - 溢出时抛出 OverflowException
  - 创建 OverflowOpsProof subject
- [ ] **A.11.8** 杂项 opcodes:
  - dup (复制栈顶), pop (弹出栈顶)
  - ldnull, ldtoken, ldftn, ldvirtftn
  - switch (跳转表)
  - 创建 MiscOpsProof subject
- [ ] **A.11.9** ILToIRLowering.cs 同步扩展所有新增 opcode 的 IR 映射
- [ ] **A.11.10** 运行全量 subject 回归测试确认无破坏

**验收标准**:
- 每类 opcode proof subject 通过
- 总覆盖 IL opcode 数量 ≥ 120 条 (从当前 ~30 条扩展)
- 所有已有 subject 无回归
- ILToIRLowering 对未支持 opcode 输出明确 NotSupportedException (非 crash)

---

## Phase B: 三方 Benchmark 报告体系

**目标**: 增量记录多平台多设备 benchmark 数据，`run benchmark --dashboard` 生成静态 HTML 总览

**设计文档**: `benchmark-design-v1-01.md`

**Phase 退出标准**:
- [ ] 8 个 Benchmark subject 全部可运行 3 种模式
- [ ] `run benchmark --dashboard --open` 生成 HTML，三个 Tab 在浏览器中正常渲染
- [ ] 每次 `run benchmark --record` 后 `docs/benchmark/dashboard.html` 自动更新
- [ ] 设备信息自动检测: Windows/macOS/Android/iOS 均能返回 OS+CPU 信息
- [ ] records.jsonl append 后 overview.json / subjects/{id}.json 同步更新
- [ ] 回归检测: 人为劣化 1 个指标 10% → overview 中标红
- [ ] 缺失模式数据时 dashboard 显示 (no data) 而非报错

---

### B.1 新增 interpreter-runtime-perf stage worker

- [ ] **B.1.1** 在 `subject_workers.py` 新增 `run_interpreter_runtime_perf()`:
  - 调用 `dotnet run` 运行 interpreter harness 项目
  - harness 内部: 加载 assembly → ILToIRLowering → ManagedInterpreterExecutor.ExecuteInt32 × N 次
  - 采集: elapsedMilliseconds, iterations, customMetrics
- [ ] **B.1.2** 注册: `DEFAULT_STAGE_WORKERS["interpreter-runtime-perf"] = run_interpreter_runtime_perf`
- [ ] **B.1.3** 参数规格:
  - dev profile: 5 samples × 1000 iterations, 0 warmup
  - release profile: 10 samples × 10000 iterations, 1 warmup
- [ ] **B.1.4** 输出格式: 与 `run_native_runtime_perf` 一致的 manifest JSON (samples, summaryMetrics)
- [ ] **B.1.5** 创建 `subjects/InterpreterPerfSmoke/` 最小验证 subject:
  - source: 简单 Add(a,b) 方法
  - manifest: 仅 interpreter-benchmark pipeline
  - 验证: 产出 perf-summary.json，meanDurationMs > 0

**验收标准**:
- InterpreterPerfSmoke 产出 perf-summary.json
- samples 数组长度 == 5 (dev mode)
- meanDurationMs, minDurationMs, maxDurationMs 均 > 0
- stdDev 已计算

---

### B.2 新增 benchmark-comparison-aggregate stage worker

- [ ] **B.2.1** 创建 `build/toolchains/run/testing/benchmark_comparison.py`:
  - `compute_comparison(managed_metrics, native_metrics, interpreter_metrics) → dict`
  - 对每个共同 metric 计算: ratio = mode_a / mode_b
  - 处理除零: 如果 baseline == 0，ratio = "N/A"
  - 处理缺失: 如果某 mode 无数据，标记 "missing"
- [ ] **B.2.2** 在 `subject_workers.py` 新增 `run_benchmark_comparison_aggregate()`:
  - 从 upstream 读取 3 个 mode 的 manifest
  - 调用 compute_comparison
  - 输出 comparison.json
- [ ] **B.2.3** 注册: `DEFAULT_STAGE_WORKERS["benchmark-comparison-aggregate"] = run_benchmark_comparison_aggregate`
- [ ] **B.2.4** 创建 `tests/unit/run/test_benchmark_comparison.py`:
  - 测试正常三方数据
  - 测试缺失一方数据 (graceful degradation)
  - 测试全零数据 (不 crash)
  - 测试 ratio 计算精度 (float comparison with tolerance)

**验收标准**:
- 单元测试全部通过
- comparison.json 包含 modes{managed, native, interpreter} + comparison{ratios}
- ratio 精度: 保留 2 位小数
- 缺失数据场景不 crash

---

### B.3 创建 8 个 Benchmark Subject

每个 subject 结构 (新增 benchmark-records 目录):
```
subjects/Bench{Name}/
├── subject.manifest.json
├── source/Bench{Name}.csproj
├── source/Program.cs
├── source/PerfHarness.cs         ← 统一 harness 格式
├── baselines/perf/               ← 现有 regression baseline (保留)
│   ├── windows-managed-perf/windows.json
│   ├── windows-native-perf/windows.json
│   └── windows-interpreter-perf/windows.json
└── benchmark-records/
    └── records.jsonl             ← Append-Only 历史记录 (新增)
```

- [ ] **B.3.1** BenchArithmetic: 10 万次 int 加减乘除循环 + float 三角函数
- [ ] **B.3.2** BenchAllocation: 循环创建 1 万个对象 + 触发 GC + 测量分配速率
- [ ] **B.3.3** BenchDispatch: 通过基类引用调用虚方法 10 万次 + 接口调用 10 万次
- [ ] **B.3.4** BenchGeneric: `List<int>.Add` × 1 万次 + `Dictionary<string,int>` 查找 × 1 万次
- [ ] **B.3.5** BenchDelegate: 单播调用 10 万次 + multicast (3 target) 调用 1 万次
- [ ] **B.3.6** BenchException: try/catch 路径 (无异常) 10 万次 + throw/catch 路径 1000 次
- [ ] **B.3.7** BenchMixed: AOT method 调用 interpreter method 1 万次 + 反向 1 万次
- [ ] **B.3.8** BenchGameLoop: 模拟 1000 帧游戏循环 (每帧: 更新 100 对象位置 + 碰撞检测 + GC safepoint)
- [ ] **B.3.9** 每个 subject 的 PerfHarness.cs 统一格式:
  - 接受 iterations 参数
  - 输出 JSON: `{ "elapsedMilliseconds": N, "iterations": N, "opsPerSecond": N }`
  - warmup 阶段不计入

**验收标准**:
- 8 个 subject 全部存在且 manifest schema 合法
- managed pipeline 可独立运行 (不依赖 native build)
- 每个 PerfHarness 输出格式一致

---

### B.4 每个 Subject 三条 Pipeline + 三组 Matrix

- [ ] **B.4.1** 每个 manifest 定义 executionPipelines:
  - `managed-benchmark`: source-resolve → host-input-build → runtime-perf-collect → report-assemble
  - `native-benchmark`: source-resolve → host-input-build → analysis-frontend → generated-native-proof → build-target → native-runtime-perf → report-assemble
  - `interpreter-benchmark`: source-resolve → host-input-build → interpreter-runtime-perf → report-assemble
- [ ] **B.4.2** 每个 manifest 定义 environmentMatrices:
  - `windows-managed-perf`: pipelineId=managed-benchmark, hostPlatform=windows-x64, runtimeProfile=managed-perf-release
  - `windows-native-perf`: pipelineId=native-benchmark, hostPlatform=windows-x64, runtimeProfile=native-perf-profile
  - `windows-interpreter-perf`: pipelineId=interpreter-benchmark, hostPlatform=windows-x64, runtimeProfile=interpreter-perf-release
- [ ] **B.4.3** 每个 matrix 定义 supportedGoals: ["perf.release"]
- [ ] **B.4.4** 验证: `run test subject --id BenchArithmetic --matrix windows-managed-perf` 通过
- [ ] **B.4.5** 验证: `run test subject --id BenchArithmetic --matrix windows-native-perf` 通过
- [ ] **B.4.6** 验证: `run test subject --id BenchArithmetic --matrix windows-interpreter-perf` 通过

**验收标准**:
- 8 × 3 = 24 组 matrix 全部可独立运行
- 每组产出 perf-summary.json
- stage 依赖链正确 (native pipeline 有 analysis+codegen+build, managed 没有)

---

### B.5 设备信息自动检测

**新增**: `build/toolchains/run/testing/device_detector.py`

- [ ] **B.5.1** 实现 `detect_current_device() → DeviceInfo`:
  - Windows: `wmic cpu get Name` → CPU 名; `platform.system()` + `platform.version()` → OS
  - macOS: `sysctl -n machdep.cpu.brand_string` → CPU; `platform.mac_ver()` → OS
  - 返回格式: `{ id, name, os, cpu, arch, isSimulator: false }`
- [ ] **B.5.2** 实现 `detect_android_device(serial) → DeviceInfo`:
  - `adb -s {serial} shell getprop ro.product.model` → 设备型号
  - `adb -s {serial} shell getprop ro.hardware` → 芯片平台
  - `adb -s {serial} shell getprop ro.product.cpu.abi` → arch
  - `adb -s {serial} shell getprop ro.build.characteristics` → 判断是否模拟器
- [ ] **B.5.3** 实现 `detect_ios_device(udid) → DeviceInfo`:
  - 真机: `xcrun devicectl list devices --json` → name + hardwareModel + osVersionNumber
  - 模拟器: `xcrun simctl list devices --json` → 设备名 + runtime; CPU = 宿主机 CPU
  - isSimulator 根据来源自动设置
- [ ] **B.5.4** `device.id` 规范化: `{platform}-{arch}-{cpu_normalized}` (全小写，空格→`-`)
  - 示例: `windows-x64-intel-core-i9-12900k`, `ios-arm64-apple-a16`, `ios-sim-arm64-apple-m3-pro`
- [ ] **B.5.5** `device.name` 格式: `{设备型号} ({CPU型号})`
  - 示例: `iPhone 14 Pro (Apple A16)`, `Windows PC (Intel Core i9-12900K)`
- [ ] **B.5.6** 结果缓存到 `artifacts/.device-cache.json`，cache TTL 1 小时
- [ ] **B.5.7** 创建 `tests/unit/run/test_device_detector.py`: mock 各平台命令输出，验证 id/name 生成正确

**验收标准**:
- Windows 检测返回 cpu 包含 "Intel" 或 "AMD" 字样
- macOS 检测返回 cpu 包含 "Apple M" 或 "Intel Core" 字样
- Android 模拟器 isSimulator == true
- iOS 模拟器 isSimulator == true，name 包含 "Simulator"
- device.id 不含空格，全小写

---

### B.6 Benchmark Records 存储层

**新增**: `build/toolchains/run/testing/benchmark_records.py`

- [ ] **B.6.1** 实现 `append_record(repo_root, record: BenchmarkRecord)`:
  - 写入 `subjects/{id}/benchmark-records/records.jsonl`
  - 目录不存在时自动创建
  - 使用文件锁防止并发写入
- [ ] **B.6.2** 实现 `query_latest(repo_root, subject_id, mode, device_id) → BenchmarkRecord | None`:
  - 反向扫描 jsonl，返回第一条匹配记录
- [ ] **B.6.3** 实现 `query_latest_all_modes(repo_root, subject_id, device_id) → dict[mode, Record]`:
  - 同时返回该设备下所有 mode 的最新记录
- [ ] **B.6.4** 实现 `query_history(repo_root, subject_id, mode, device_id, limit=20) → list[BenchmarkRecord]`:
  - 返回最近 N 条记录 (时间正序)
- [ ] **B.6.5** 实现 `query_all_devices(repo_root, subject_id, mode) → dict[device_id, Record]`:
  - 返回该 subject+mode 下所有设备的最新记录 (跨平台对比用)
- [ ] **B.6.6** BenchmarkRecord dataclass:
  ```python
  @dataclass
  class BenchmarkRecord:
      runId: str          # "{timestamp}-{subject}-{mode}-{device_id}"
      subject: str
      mode: str           # "managed" | "native" | "interpreter"
      platform: str       # "windows-x64" | "ios-arm64" | ...
      device: DeviceInfo
      recordedAt: str     # ISO 8601
      gitCommit: str
      gitBranch: str
      metrics: dict[str, float]
  ```
- [ ] **B.6.7** 创建 record schema: `contracts/artifacts/v0/schemas/benchmark-record.schema.json`
- [ ] **B.6.8** 创建 `tests/unit/run/test_benchmark_records.py`: append/query/history 单元测试

**验收标准**:
- append 后 query_latest 返回刚写入的记录
- query_history limit=5 返回最近 5 条
- 并发 append 不导致 jsonl 文件损坏 (文件锁有效)
- 空文件 / 不存在文件 query 返回 None 不 crash

---

### B.7 Dashboard 生成器

**新增**: `build/toolchains/run/testing/benchmark_dashboard_generator.py`
**新增**: `build/toolchains/run/testing/templates/benchmark-dashboard.html`

- [ ] **B.7.1** 实现 `generate(repo_root, output_path)`:
  - 扫描所有 `subjects/*/benchmark-records/records.jsonl`
  - 构建三层数据结构: subjects → modes → devices
  - 序列化为 JSON 注入 `<script>window.BENCHMARK_DATA = {...};</script>`
  - 写入自包含 HTML
- [ ] **B.7.2** 实现 `update_docs(repo_root, subject_id=None)`:
  - 更新 `docs/benchmark/subjects/{id}.json` (全部或指定 subject)
  - 更新 `docs/benchmark/overview.json`
  - 重新生成 `docs/benchmark/dashboard.html`
- [ ] **B.7.3** overview.json 结构:
  - 默认平台: `windows-x64-*` (统计次数最多的 Windows 设备)
  - 每个 subject × mode 的最新 metrics + recordedAt
  - 新鲜度标注: `staleWarning` (数据 > 7 天) + `ageMismatchWarning` (模式间时间差 > 3 天)
- [ ] **B.7.4** subjects/{id}.json 结构:
  - `latestByDimension`: `{device_id: {mode: {metrics, recordedAt, gitCommit}}}`
  - 所有历史记录 (用于趋势图)
- [ ] **B.7.5** Tab 1 总览表格:
  - 行: 全部 subject
  - 列: C# / AOT / Interpreter + 比值 AOT/C# + HU/C#
  - 默认显示 Windows 数据, 下拉切换平台/设备
  - 颜色: 绿色=达标, 红色=未达标, 灰色=缺失, 橙色=数据过旧
- [ ] **B.7.6** Tab 2 Subject 详情:
  - 模式对比柱状图 (最新记录, 可切换设备)
  - 趋势折线图 (最近 20 次, 可切换 mode + 设备)
  - 设备横向对比表格 (同 subject+mode, 不同设备)
- [ ] **B.7.7** Tab 3 设备对比:
  - 同 subject+mode, 所有设备的雷达图 (相对 Windows 基线归一化)
  - 真机 vs 模拟器颜色区分 (实线 vs 虚线)
- [ ] **B.7.8** Chart.js via CDN (`cdn.jsdelivr.net/npm/chart.js`)
- [ ] **B.7.9** 无 JS console error, 缺失数据显示 "(no data)" placeholder

**验收标准**:
- Tab 1: 8 subject × 3 mode 数据正确显示
- Tab 2: 趋势图有 ≥ 2 个数据点时正常渲染折线
- Tab 3: 雷达图有 ≥ 2 个设备时正常渲染
- 全部缺失数据时 (刚初始化) 不 crash，显示空状态提示
- `docs/benchmark/dashboard.html` 文件大小 < 500KB (不含 Chart.js CDN)

---

### B.8 CLI 命令: `run benchmark`

**新增**: `build/toolchains/run/commands/benchmark.py`

- [ ] **B.8.1** 子命令 `run benchmark --subject <id> --mode <mode> --record`:
  - 运行对应 pipeline → 采集 metrics → 调用 device_detector → append_record
  - 完成后自动调用 update_docs(subject_id)
  - stdout 输出结果摘要
- [ ] **B.8.2** 子命令 `run benchmark --all --mode <mode> --record`:
  - 遍历所有 Bench* subject 执行上述流程
- [ ] **B.8.3** 子命令 `run benchmark --dashboard [--open]`:
  - 调用 generate(repo_root, "docs/benchmark/dashboard.html")
  - `--open`: 调用系统默认浏览器打开
- [ ] **B.8.4** 子命令 `run benchmark status [--subject <id>] [--all]`:
  - `--subject`: 打印该 subject 各 mode × 设备的最新记录摘要
  - `--all`: 打印总览表格 (CLI 版, 无图表)
- [ ] **B.8.5** stdout 结果摘要格式:
  ```
  ✓ BenchArithmetic / native / Windows PC (Intel Core i9-12900K)
    meanDurationMs:  2.1 ms   opsPerSecond: 476,200
    docs/benchmark/dashboard.html updated
  ```
- [ ] **B.8.6** 修改 `build/toolchains/run/run_manifest.json` 注册 benchmark 命令
- [ ] **B.8.7** 退出码: 0=成功, 1=有 regression, 2=运行错误

**验收标准**:
- `run benchmark --help` 显示完整帮助
- `run benchmark --subject BenchArithmetic --mode native --record` 端到端成功
- `docs/benchmark/dashboard.html` 在命令完成后自动更新
- `run benchmark --dashboard --open` 打开浏览器

---

### B.9 docs/benchmark 初始化 + README

- [ ] **B.9.1** 创建 `docs/benchmark/README.md`:
  ```
  # Benchmark Dashboard
  此目录由工具自动维护，请勿手动编辑 JSON 文件。
  - dashboard.html: 最新性能总览，用浏览器直接打开
  - overview.json: 机器可读汇总数据
  - subjects/*.json: 各 subject 详细记录
  更新方式: run benchmark --subject <id> --mode <mode> --record
  ```
- [ ] **B.9.2** 将 `docs/benchmark/` 加入 `.gitignore` 的排除名单 (保留在 git 中):
  - `.gitignore` 中确认 `docs/benchmark/` **不被**忽略 (它是版本化根)
  - `artifacts/benchmark-records/` 也不忽略 (由 subjects/ 下的 jsonl 管理)
- [ ] **B.9.3** 首次运行: `run benchmark --dashboard` 生成初始空状态 dashboard

**验收标准**:
- `git status` 显示 `docs/benchmark/` 下文件被追踪
- README 可读性: 新开发者 5 分钟内理解如何更新 dashboard

---

## Phase C: 热更端到端

**目标**: Package 加载 → Interpreter 执行 → AOT 互操作完整闭环

**Phase 退出标准**:
- [ ] 热更包加载 → interpreter 执行 → 返回正确结果
- [ ] AOT 方法被热更方法替换后，调用方自动走 interpreter
- [ ] Rollback 后恢复 AOT 执行路径
- [ ] BenchHotUpdateRoundtrip 三方 benchmark 可运行

---

### C.1 Interpreter opcode 扩展

- [ ] **C.1.1** 修改 `interpreter_vm.h`: IROpCode 枚举扩展到 50+:
  - 新增: LdcI8, LdcR4, LdcR8, LdStr, LdNull
  - 新增: NewObj, NewArr, LdFld, StFld, LdElem, StElem, LdLen
  - 新增: Box, Unbox, CastClass, IsInst
  - 新增: Call, CallVirt (已在 managed 有, native 无)
  - 新增: Conv_I4, Conv_I8, Conv_R4, Conv_R8
- [ ] **C.1.2** 修改 `interpreter_vm.h`: 值类型从 int32 改为 tagged union:
  ```cpp
  enum class ValueTag : uint8_t { Int32, Int64, Float32, Float64, ObjectRef, Null };
  struct InterpreterValue { ValueTag tag; union { int32_t i32; int64_t i64; float f32; double f64; void* obj; }; };
  ```
- [ ] **C.1.3** 修改 `interpreter_vm.cpp`: ExecutionFrame 栈改为 `vector<InterpreterValue>`
- [ ] **C.1.4** 实现每个新 opcode 的 handler (switch case)
- [ ] **C.1.5** 修改 `ManagedInterpreterExecutor.cs`: ExecuteInt32 泛化为 Execute 返回 object
- [ ] **C.1.6** 创建 InterpreterFloatProof: float 加减乘除 + 比较
- [ ] **C.1.7** 创建 InterpreterObjectProof: NewObj + LdFld/StFld + virtual call
- [ ] **C.1.8** 创建 InterpreterArrayProof: NewArr + LdElem/StElem + LdLen

**验收标准**:
- Native interpreter 支持 ≥ 50 opcodes
- Float 计算精度: abs(expected - actual) < 1e-6
- 对象分配通过 GC (A.2 集成)
- 数组越界抛出 IndexOutOfRangeException

---

### C.2 HotUpdatePackage 运行时加载器集成

- [ ] **C.2.1** 修改 `hot_update.h`: 新增 `LoadHotUpdatePackage(path, out_handle)` / `UnloadHotUpdatePackage(handle)`
- [ ] **C.2.2** 修改 `hot_update.cpp`:
  - 读取 manifest.json → 验证 formatVersion, targetAotVersion
  - 加载 assembly binary → LoadAssemblyImageFromPath
  - 加载 supplemental metadata → LoadSupplementalMetadataFromPath
  - 返回 HotUpdatePackageHandle
- [ ] **C.2.3** 修改 `RuntimeManager.cs`:
  - `LoadPackage(packagePath)` → 读取 native handle → ILToIRLowering 所有方法 → 注册到 HotUpdateMethodRegistry
- [ ] **C.2.4** 扩展 HotUpdateSkeletonProof:
  - 打包一个简单方法 (Add(a,b))
  - RuntimeManager.LoadPackage → 调用热更方法 → 验证返回正确

**验收标准**:
- HotUpdateSkeletonProof 端到端: 打包 → 加载 → 执行 → 正确结果
- 加载失败 (版本不匹配) → 返回错误码而非 crash
- Unload 后内存释放 (无泄漏)

---

### C.3 AOT↔Interpreter Bridge 自动生成

- [ ] **C.3.1** 创建 `AutoBridgeGenerator.cs`:
  - 输入: AOT 方法签名列表 + 热更方法签名列表
  - 输出: 自动生成的 BridgeSpec (方法 token → bridge 函数签名)
  - 支持签名: void(), int(int), int(int,int), void(object), object(object) 等常见模式
- [ ] **C.3.2** 修改 `BridgeDispatcher.cs`: 支持动态注册 auto-generated bridge
- [ ] **C.3.3** 修改 `BridgeGenerator.cs`: 优先使用 auto-generated，回退到手动 spec
- [ ] **C.3.4** 创建 AutoBridgeProof:
  - AOT 定义 ICalculator.Add(int,int)
  - 热更替换 Calculator.Add → interpreter 执行
  - 通过 ICalculator 接口调用 → auto bridge → interpreter → 返回正确值

**验收标准**:
- AutoBridgeProof: 通过接口调用热更方法，返回正确结果
- 支持 ≥ 5 种常见方法签名模式
- 签名不匹配时明确报错 (非 crash)

---

### C.4 运行时方法替换

- [ ] **C.4.1** 创建 `src/native/hot-update/method_replacement.h` + `.cpp`:
  - `RegisterMethodReplacement(method_token, interpreter_thunk) → bool`
  - `RevertMethodReplacement(method_token) → bool`
  - `ResolveMethodPointer(method_token) → void*` (优先查替换表)
  - 内部: `unordered_map<uint32_t, MethodReplacementEntry>`
- [ ] **C.4.2** 修改 `bootstrap.cpp`: `FindMethodPointerByToken` → 先查 method_replacement，miss 再查 code_registration
- [ ] **C.4.3** interpreter thunk 函数格式: 与 AOT 函数签名一致的 wrapper，内部调用 interpreter
- [ ] **C.4.4** 创建 MethodReplacementProof:
  - AOT 定义 Greet() → "hello from AOT"
  - 加载热更包 → RegisterMethodReplacement → 调用 Greet() → "hello from interpreter"
  - RevertMethodReplacement → 调用 Greet() → "hello from AOT"

**验收标准**:
- MethodReplacementProof: 替换/回滚/再替换全路径正确
- 替换后通过虚方法调用也生效 (vtable slot 被替换)
- 多线程并发调用被替换方法不 crash (替换操作加锁)

---

### C.5 版本管理 + 回滚

- [ ] **C.5.1** 修改 `RuntimeManager.cs`:
  - `Stack<LoadedPackageSnapshot>` 保存每次加载前的状态快照
  - `Rollback()`: pop 栈顶 → RevertMethodReplacement × N → UnloadHotUpdatePackage
  - `CurrentPackageVersion` / `TargetAotVersion` 属性
- [ ] **C.5.2** 版本兼容性检查:
  - package.targetAotVersion 必须 match 当前 AOT major.minor
  - major 不同 → 拒绝加载
  - minor 更高 → 警告但允许
- [ ] **C.5.3** 创建 VersionRollbackProof:
  - 加载 v1 → 验证 → 加载 v2 → 验证 → Rollback → 验证回到 v1 → Rollback → 验证回到 AOT-only
- [ ] **C.5.4** 版本不兼容拒绝测试:
  - targetAotVersion = "2.0" + 当前 AOT = "1.0" → LoadPackage 返回错误

**验收标准**:
- VersionRollbackProof: 四步验证全部通过
- 版本不兼容: 返回明确错误码 + 错误消息
- Rollback 后内存释放 (无累积泄漏)
- 空栈 Rollback 不 crash (返回 false)

---

### C.6 热更 Benchmark Subjects

- [ ] **C.6.1** BenchHotUpdateLoad: 测量 LoadPackage() 耗时 (从文件加载到可执行)
- [ ] **C.6.2** BenchHotUpdateDispatch: AOT → interpreter thunk 分派耗时 × 10 万次
- [ ] **C.6.3** BenchHotUpdateRoundtrip: AOT → interpreter → AOT callback → interpreter 返回 × 1 万次
- [ ] **C.6.4** 每个 subject 接入三方 benchmark 框架 (Phase B)

**验收标准**:
- BenchHotUpdateLoad: package load < 500ms (含 ILToIRLowering)
- BenchHotUpdateDispatch: 单次 thunk 分派 < 100ns
- BenchHotUpdateRoundtrip: 单次 roundtrip < 10μs
- 三方 benchmark 报告中显示热更指标

---

## Phase D: 多平台真机验证

**目标**: Android/iOS 真机运行并采集 Benchmark

**Phase 退出标准**:
- [ ] Android ARM64 真机上 MobileHelloWorldProof 通过
- [ ] iOS ARM64 真机/模拟器上 MobileHelloWorldProof 通过
- [ ] 至少 4 个 Benchmark subject 在 Android + iOS 产出 metrics
- [ ] 跨平台对比报告显示 ARM64 vs x64 性能差异

---

### D.1 Android ARM64

- [ ] **D.1.1** 修改 `third_party/bdwgc/CMakeLists.txt`: Android 路径 — GC_PTHREADS, 排除 win32_threads.c
- [ ] **D.1.2** 修改 `CMakeLists.txt`: android-arm64-smoke preset 链接 chaos_bdwgc
- [ ] **D.1.3** 修改 `src/mobile/android/CMakeLists.txt`: 链接扩展后的所有 runtime 库 (runtime-core, bootstrap, interpreter, hot-update, engine-bridge)
- [ ] **D.1.4** 修改 `mobile_host_jni.cpp`: 添加热更包加载 JNI wrapper `Java_..._loadHotUpdatePackage(path)`
- [ ] **D.1.5** Android Gradle 配置: minSdkVersion 21, targetSdkVersion 34, NDK r25+
- [ ] **D.1.6** 部署验证: `adb push` native binary → `adb shell` 执行 → 输出 "Hello, World!"
- [ ] **D.1.7** MobileHelloWorldProof 通过
- [ ] **D.1.8** 热更场景: 推送热更包到 sdcard → 加载 → 执行 → 正确结果

**验收标准**:
- `cmake --preset android-arm64-smoke` 编译成功
- 真机上 MobileHelloWorldProof 输出正确
- 热更包加载 + interpreter 执行正确
- 无 SIGABRT/SIGSEGV crash
- GC 在 Android 上正常工作 (GC_get_gc_no() > 0)

---

### D.2 iOS ARM64

- [ ] **D.2.1** 修改 `third_party/bdwgc/CMakeLists.txt`: iOS 路径 — darwin_stop_world.c, GC_PTHREADS
- [ ] **D.2.2** 修改 `CMakeLists.txt`: ios-arm64-packaging preset 链接 chaos_bdwgc
- [ ] **D.2.3** 修改 `src/mobile/ios/CMakeLists.txt`: 链接扩展后的全部 runtime 库
- [ ] **D.2.4** 修改 `mobile_host_bridge.mm`: 添加热更包加载 ObjC API
- [ ] **D.2.5** Xcode 配置: 部署目标 iOS 15+, arm64 架构, bitcode OFF
- [ ] **D.2.6** 模拟器验证: xcodebuild → iOS Simulator 运行 → 输出正确
- [ ] **D.2.7** 真机验证 (如有设备): MobileHelloWorldProof 通过
- [ ] **D.2.8** 热更场景: 从 Documents 目录加载热更包 → 执行 → 正确结果

**验收标准**:
- `cmake --preset ios-arm64-packaging` 配置成功
- Xcode build 成功 (0 errors, 0 warnings 除 system headers)
- 模拟器上 MobileHelloWorldProof 通过
- GC 在 iOS 上正常工作

---

### D.3 iOS 热更分发策略

- [ ] **D.3.1** 创建 `contracts/shared/v0/ios-hot-update-strategy.md`:
  - App Store Review Guidelines 3.3.2 / 2.5.2 边界分析
  - 解释执行 (interpreter) vs 下载可执行代码 的 Apple 认定
  - 推荐: 热更包只含 IL bytecode + metadata，interpreter 内嵌在 app 中
  - 风险: Apple 政策变更、审核不一致性
- [ ] **D.3.2** 创建 `src/managed/Chaos.IL2CPP.HotUpdate/IosComplianceValidator.cs`:
  - 检查热更包不包含: native code (.dylib/.so), JIT compiled code, 可执行脚本
  - 检查包大小 < 配置上限 (默认 50MB)
  - 检查 assembly 不引用 restricted API (e.g., dlopen, mmap+exec)
- [ ] **D.3.3** IosComplianceValidator 集成到 PackageValidator 链
- [ ] **D.3.4** 单元测试: 合规包通过 / 含 native code 包拒绝

**验收标准**:
- 策略文档覆盖 Apple 3.3.2 和 2.5.2 条款
- Validator 正确拒绝含 native code 的包
- Validator 通过正常 IL-only 包

---

### D.4 跨平台 Benchmark 采集

- [ ] **D.4.1** 创建 `build/toolchains/run/testing/mobile_perf_collector.py`:
  - Android: adb push binary + adb shell 执行 + adb pull 结果 JSON
  - iOS: xcodebuild test + 解析 test output 中的 JSON
- [ ] **D.4.2** 修改 `subject_workers.py`: 新增 `run_mobile_native_perf()` worker
- [ ] **D.4.3** Benchmark subject manifest 扩展: android-arm64-native-perf / ios-arm64-native-perf matrix
- [ ] **D.4.4** 跨平台对比报告: comparison.json 扩展 platform 维度
- [ ] **D.4.5** HTML 报告: 新增平台对比 tab (Windows x64 vs Android ARM64 vs iOS ARM64)

**验收标准**:
- 至少 BenchArithmetic + BenchAllocation 在 Android/iOS 产出 metrics
- 跨平台 comparison.json 包含 ≥ 2 个平台数据
- HTML 报告显示平台对比

---

### D.5 平台特定优化

- [ ] **D.5.1** 创建 `src/native/runtime-core/platform_arm64.h`:
  - 内存对齐: ARM64 要求 16 字节对齐的分配 (GC_MALLOC 已保证)
  - cache line 大小: 64 字节 (用于 monitor 锁的 padding)
- [ ] **D.5.2** 条件编译: `#if defined(__aarch64__)` 启用 ARM64 优化路径
- [ ] **D.5.3** (可选) `interpreter_vm_arm64.cpp`: NEON intrinsics 加速 interpreter 的批量整数运算
- [ ] **D.5.4** Benchmark 验证: ARM64 优化路径 vs 通用路径对比

**验收标准**:
- ARM64 编译无新增 warning
- Benchmark 不劣化 (优化路径 ≥ 通用路径性能)

---

## Phase E: 生产化 + CI/CD

**目标**: 可交付、可回归、可监控

**Phase 退出标准**:
- [ ] CI 在 push 时自动编译 + 运行全量测试
- [ ] Benchmark 回归自动检测并报告
- [ ] 异常时产出可读 managed stack trace
- [ ] 文档覆盖: 集成/API/热更/调优/平台

---

### E.1 GitHub Actions CI

- [ ] **E.1.1** 创建 `.github/workflows/ci.yml`:
  - trigger: push + pull_request
  - jobs: windows-build (cmake + build + test), managed-test (dotnet test)
  - artifact: 上传 test report JSON
- [ ] **E.1.2** 创建 `.github/workflows/benchmark.yml`:
  - trigger: push to main (nightly 或 on-demand)
  - jobs: 运行 `run benchmark --all --report`
  - artifact: 上传 benchmark report HTML + JSON
- [ ] **E.1.3** 创建 `build/scripts/check-benchmark-regression.py`:
  - 读取 comparison.json → 检查 verdict → 失败则 exit 1
- [ ] **E.1.4** CI badge: README 中添加 build status badge

**验收标准**:
- push 触发 CI → windows-build 成功
- benchmark regression 检测: 人为劣化 → CI 失败
- artifact 可下载

---

### E.2 版本兼容矩阵

- [ ] **E.2.1** 创建 `src/managed/Chaos.IL2CPP.HotUpdate/VersionMatrix.cs`:
  - 定义 `CompatibilityRule`: major 必须相同, minor 向后兼容, patch 任意
  - `IsCompatible(aotVersion, packageVersion) → (bool, string reason)`
- [ ] **E.2.2** 修改 `HotUpdateVersionCompatibility.cs`: 调用 VersionMatrix
- [ ] **E.2.3** 单元测试: (1.0, 1.0)→OK, (1.0, 1.1)→OK, (1.0, 2.0)→REJECT, (2.0, 1.0)→REJECT

**验收标准**:
- 全部兼容性规则单元测试通过
- 集成到 PackageValidator 链

---

### E.3 错误报告 + Managed Stack Trace 恢复

- [ ] **E.3.1** 创建 `src/native/runtime-core/stack_trace.h`:
  ```cpp
  struct ManagedStackFrame { const char* method_name; const char* type_name; const char* assembly_name; uint32_t il_offset; bool is_interpreter; };
  struct ManagedStackTrace { ManagedStackFrame* frames; uint32_t count; };
  ```
- [ ] **E.3.2** 创建 `src/native/runtime-core/stack_trace.cpp`:
  - thread-local stack frame 收集器: `PushFrame() / PopFrame() / CaptureTrace()`
- [ ] **E.3.3** 修改 `runtime_core.cpp`: RaiseManagedException 时 CaptureTrace
- [ ] **E.3.4** 修改 `interpreter_vm.cpp`: 每个 Call/CallVirt 前 PushFrame, 返回后 PopFrame
- [ ] **E.3.5** 格式化输出: `"at TypeName.MethodName() [IL offset 0xNN] in AssemblyName"`
- [ ] **E.3.6** 创建 StackTraceProof: 三层调用 A→B→C throw → 捕获 → stack trace 包含 3 帧

**验收标准**:
- StackTraceProof: 异常 stack trace 包含全部 3 个方法名
- Interpreter 帧标记 `[interpreter]`
- AOT 帧标记 `[aot]`
- IL offset 非零 (正确追踪)

---

### E.4 Benchmark 持续监控

- [ ] **E.4.1** 创建 `build/toolchains/run/testing/benchmark_history.py`:
  - `append_result(history_file, comparison_json)` — JSON Lines 追加
  - `analyze_trend(history_file, window=10)` — 最近 10 次的趋势: 稳定/改善/退化
- [ ] **E.4.2** 创建 `build/scripts/store-benchmark-results.py`: CI 中调用，存入 artifacts/benchmark-history/
- [ ] **E.4.3** 创建 `.github/workflows/benchmark-nightly.yml`: 每日 UTC 3:00 运行
- [ ] **E.4.4** 趋势报告: 最近 10 次的 meanDurationMs 变化曲线 (JSON 输出)

**验收标准**:
- 3 次以上结果后趋势分析正确: 人为连续劣化 → 检测到 "degrading" 趋势
- history JSON Lines 格式正确
- nightly workflow 无报错

---

### E.5 文档

- [ ] **E.5.1** `docs/integration-guide.md`: 引擎集成步骤 (初始化 → 加载 assembly → 运行 → 热更 → 关闭)
- [ ] **E.5.2** `docs/api-reference.md`: RuntimeAbiV0 全部 20+ 函数签名 + 参数说明 + 返回值
- [ ] **E.5.3** `docs/hot-update-guide.md`: 热更包创建 (Pipeline 输出) → 分发 → 加载 → 回滚
- [ ] **E.5.4** `docs/tuning-guide.md`: GC 调参 (GC_set_max_heap_size 等) + interpreter 性能瓶颈 + benchmark 方法论
- [ ] **E.5.5** `docs/platform-notes.md`: Android NDK 版本要求 + iOS 部署目标 + Apple 合规注意事项
- [ ] **E.5.6** 文档中所有代码示例可编译验证

**验收标准**:
- 5 篇文档全部创建
- API reference 覆盖 RuntimeAbiV0 + CodegenBridgeV0 全部公开函数
- 代码示例无语法错误
- 热更指南包含完整端到端流程

---

## 全局验收标准汇总

| 指标 | 目标 | 验证点 |
|------|------|--------|
| AOT vs C# throughput | ≥ 2x faster | B.8 — BenchArithmetic + BenchGameLoop |
| HotUpdate vs C# overhead | ≤ 50x slower | B.8 — BenchArithmetic interpreter mode |
| Bridge 单次调用延迟 | ≤ 1μs | C.6 — BenchHotUpdateDispatch |
| GC pause p99 | ≤ 2ms | B.8 — BenchAllocation GC 指标 |
| Interpreter memory / C# | ≤ 1.5x | B.8 — BenchAllocation 内存指标 |
| 热更包加载时间 | ≤ 500ms | C.6 — BenchHotUpdateLoad |
| Android ARM64 / Windows x64 | ≤ 1.2x | D.4 — 跨平台 benchmark |
| IL opcode 覆盖 | ≥ 120 条 | A.11 — opcode proof subjects |
| 全量 subject 无回归 | 0 failure | 每个 Phase 退出时 |
| CI 编译 + 测试 | 全绿 | E.1 — GitHub Actions |
| 文档覆盖 | 5 篇 | E.5 |
