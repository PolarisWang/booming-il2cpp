# Phase A — GC 集成 + AOT 运行时关键补完 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。每完成一个 checkbox 立即标记。每个 Batch 完成后更新 `STATUS.md` 和 `notes/progress-v1-NN.md`。

**目标：** 接入 BDWGC 实现真正的垃圾回收，补完 vtable 分派、完整异常处理、泛型运行时、委托多播、IL opcode 扩展，使 generated C++ 在 Windows x64 可跑通真实 C# 代码。

**架构：** 分两批推进。Batch 1–3 为 GC 集成（A.1–A.6），不改 codegen，纯 native runtime 变更，风险低可快速验证。Batch 4–6 为 runtime 关键能力（vtable / EH / 泛型 / 委托 / opcode），涉及 managed codegen 和 native bootstrap，每个子任务有独立 proof subject 验收。

**技术栈：** C++17, CMake 3.20+, BDWGC 7.7.0 (Unity fork, `third_party/bdwgc/`), C# (.NET 8), Scriban (codegen 模板)

**设计文档：** `docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md` Phase A 节

**预期知识沉淀：** `wiki/runtime/gc-integration.md`, `wiki/runtime/vtable-dispatch.md`

---

## 涉及文件清单

| 文件 | 操作 | 所属步骤 |
|------|------|---------|
| `third_party/bdwgc/CMakeLists.txt` | **创建** | A.1 |
| `CMakeLists.txt` | 修改 — add_subdirectory bdwgc | A.1 |
| `src/native/runtime-core/CMakeLists.txt` | 修改 — link chaos_bdwgc | A.1 |
| `tests/contracts/native/gc/CMakeLists.txt` | **创建** | A.1 |
| `tests/contracts/native/gc/gc_init_smoke.cpp` | **创建** | A.1 |
| `src/native/runtime-core/runtime_core.cpp` | 修改 — GC_INIT, GC_MALLOC, handle table, thread reg, finalizer, safepoint | A.2–A.6 |
| `tests/contracts/native/gc/gc_handle_smoke.cpp` | **创建** | A.3 |
| `tests/contracts/native/gc/gc_thread_smoke.cpp` | **创建** | A.4 |
| `tests/contracts/native/gc/gc_finalizer_smoke.cpp` | **创建** | A.5 |
| `tests/contracts/native/gc/gc_safepoint_smoke.cpp` | **创建** | A.6 |
| `src/native/runtime-core/vtable_registry.h` | **创建** | A.7 |
| `src/native/runtime-core/vtable_registry.cpp` | **创建** | A.7 |
| `src/native/runtime-core/runtime_core.h` | 修改 — VTableEntry, TypeVTable | A.7 |
| `src/native/bootstrap/bootstrap.cpp` | 修改 — vtable lookup, multicast delegate, method replacement | A.7, A.10 |
| `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs` | 修改 — vtable emit, generic context, opcode | A.7, A.9, A.11 |
| `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs` | 修改 — vtable init code | A.7 |
| `src/managed/Chaos.IL2CPP.Interpreter/InterpreterIR.cs` | 修改 — Filter, Fault region kinds | A.8 |
| `src/managed/Chaos.IL2CPP.Interpreter/ManagedInterpreterExecutor.cs` | 修改 — filter/fault handler | A.8 |
| `src/managed/Chaos.IL2CPP.Interpreter/ILToIRLowering.cs` | 修改 — filter/fault/opcode lowering | A.8, A.11 |
| `src/native/runtime-core/generic_context.h` | **创建** | A.9 |
| `src/native/runtime-core/generic_context.cpp` | **创建** | A.9 |
| `subjects/VTableDispatchProof/` | **创建** | A.7 |
| `subjects/CrossBoundaryExceptionProof/` | **创建** | A.8 |
| `subjects/GenericCollectionProof/` | **创建** | A.9 |
| `subjects/DelegateChainProof/` | **创建** | A.10 |
| `subjects/Bench*/` (opcode proof subjects) | **创建** | A.11 |

---

## 步骤

### Batch 1: BDWGC 构建集成 (A.1)

- [x] **1.1** 创建 `third_party/bdwgc/CMakeLists.txt`：
  - target 名 `chaos_bdwgc`，`add_library(chaos_bdwgc STATIC ...)`
  - 核心源文件（33 个，见下方列表）
  - Windows: `if(WIN32) target_sources(... win32_threads.c) target_compile_definitions(... GC_WIN32_THREADS)`
  - Unix: `pthread_support.c pthread_stop_world.c`；Apple 额外 `darwin_stop_world.c`
  - 编译定义: `GC_THREADS GC_BUILTIN_ATOMIC ALL_INTERIOR_POINTERS GC_NOT_DLL PARALLEL_MARK THREAD_LOCAL_ALLOC`
  - `target_include_directories(chaos_bdwgc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)`

  核心源文件列表:
  ```
  alloc.c allchblk.c blacklst.c dbg_mlc.c dyn_load.c finalize.c fnlz_mlc.c
  gc_dlopen.c gcj_mlc.c headers.c heapsections.c mach_dep.c malloc.c mallocx.c
  mark.c mark_rts.c misc.c new_hblk.c obj_map.c os_dep.c ptr_chck.c reclaim.c
  specific.c thread_local_alloc.c typd_mlc.c vector_mlc.c
  ```

- [x] **1.2** 修改 `CMakeLists.txt`（根文件，windows-x64-reference 块，约 line 35）：
  - 在 `add_subdirectory(src/native/runtime-core ...)` **之前** 添加：
    `add_subdirectory(${REPO_ROOT}/third_party/bdwgc bdwgc_build)`

- [x] **1.3** 修改 `src/native/runtime-core/CMakeLists.txt`：
  - 现有 `target_compile_features(chaos_runtime_core PUBLIC cxx_std_17)` 后添加：
    `target_link_libraries(chaos_runtime_core PUBLIC chaos_bdwgc)`
  - 添加：`target_include_directories(chaos_runtime_core PRIVATE ${REPO_ROOT}/third_party/bdwgc/include)`

- [x] **1.4** 创建 `tests/contracts/native/gc/CMakeLists.txt` + `gc_init_smoke.cpp`：
  ```cpp
  // gc_init_smoke.cpp
  #include <gc.h>
  #include <cstdio>
  int main() {
      GC_INIT();
      void* p = GC_MALLOC(16);
      printf(p ? "GC init OK\n" : "FAIL\n");
      return p ? 0 : 1;
  }
  ```
  CMakeLists 将其加入 windows-x64-reference 编译目标。

- [x] **1.5** 验证：`cmake --preset windows-x64-reference && cmake --build` 无错误；gc_init_smoke.exe 输出 `GC init OK`，退出码 0

**Batch 1 验收**: chaos_bdwgc.lib 生成，gc_init_smoke.exe 通过

---

### Batch 2: 替换分配器 + GcHandle 实现 (A.2–A.3)

- [x] **2.1** 在 `runtime_core.cpp` 顶部添加 `#include <gc.h>`

- [x] **2.2** 修改 `DefaultAllocate`（line 92）：`return GC_MALLOC(size);`（替换 `std::malloc`）

- [x] **2.3** 修改 `DefaultDeallocate`（line 97）：函数体改为空（保留签名）

- [x] **2.4** 在 `RuntimeInit`（line 178）中，`TryNormalizeConfig` 调用之后、第一次 `AllocateBytes` 之前，添加：
  ```cpp
  static std::once_flag s_gc_init_flag;
  std::call_once(s_gc_init_flag, []() { GC_INIT(); });
  ```

- [x] **2.5** 新增 `AllocateBytesAtomic` 辅助（无指针 payload 用）：
  ```cpp
  static void* AllocateBytesAtomic(size_t size) { return GC_MALLOC_ATOMIC(size); }
  ```
  - 修改 `StringNewUtf8`（byte payload 部分）使用 `AllocateBytesAtomic`
  - 修改 `BoxValueObject`（value data 部分）使用 `AllocateBytesAtomic`

- [x] **2.6** 修改 `RuntimeShutdown`（line 213）：移除 `FreeBytes(runtime_state->config, runtime_state)` 调用

- [x] **2.7** 添加 GcHandle 全局表（runtime_core.cpp 全局区，紧跟 monitor registry 之后）：
  ```cpp
  struct GcHandleEntry { void* object_instance; bool pinned; };
  static std::mutex s_gc_handle_mutex;
  static std::atomic<uint64_t> s_next_gc_handle{1};
  static std::unordered_map<uint64_t, GcHandleEntry> s_gc_handle_table;
  ```

- [x] **2.8** 实现 `GcHandleNew`（line 348，替换 stub）：
  ```cpp
  if (!runtime_state || !object_instance) return CHAOS_GC_HANDLE_INVALID;
  std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
  uint64_t handle = s_next_gc_handle++;
  s_gc_handle_table[handle] = { object_instance, pinned };
  if (pinned) {
      GC_add_roots(object_instance,
          static_cast<char*>(object_instance) + sizeof(void*));
  }
  return handle;
  ```

- [x] **2.9** 实现 `GcHandleFree`（line 358，替换 stub）：
  ```cpp
  if (!runtime_state || gc_handle == CHAOS_GC_HANDLE_INVALID) return;
  std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
  auto it = s_gc_handle_table.find(gc_handle);
  if (it != s_gc_handle_table.end()) {
      if (it->second.pinned)
          GC_remove_roots(it->second.object_instance,
              static_cast<char*>(it->second.object_instance) + sizeof(void*));
      s_gc_handle_table.erase(it);
  }
  ```

- [x] **2.10** 创建 `tests/contracts/native/gc/gc_handle_smoke.cpp`：
  - GcHandleNew(pinned=false) 返回 > 0
  - GcHandleFree 后再次查找返回 INVALID
  - GcHandleNew(pinned=true) → GC_gcollect() → 不 crash

- [x] **2.11** 运行已有 subjects：HelloWorldObject, GenericEcho 仍通过

**Batch 2 验收**: 已有 subjects 无回归；gc_handle_smoke 通过；GcHandleNew 返回 > 0

---

### Batch 3: 线程注册 + Finalizer + Safepoint (A.4–A.6)

- [x] **3.1** 修改 `ThreadAttach`（line 224）：在 `ThreadState` 分配完成后添加：
  ```cpp
  struct GC_stack_base sb;
  if (GC_get_stack_base(&sb) == GC_SUCCESS) {
      int r = GC_register_my_thread(&sb);
      (void)r; // GC_DUPLICATE is ok for main thread
  }
  ```

- [x] **3.2** 修改 `ThreadDetach`（line 248）：在 `delete thread_internal_state` **之前** 添加：
  ```cpp
  GC_unregister_my_thread();
  ```

- [x] **3.3** 创建 `tests/contracts/native/gc/gc_thread_smoke.cpp`：
  - 主线程 + 2 个工作线程各 ThreadAttach → 分配 500 对象 → ThreadDetach
  - 主线程等待后 GC_gcollect() → 退出码 0

- [x] **3.4** 修改 `EnqueueFinalizer`（line 897）：现有 queue 逻辑**保留**，额外添加：
  ```cpp
  GC_register_finalizer_no_order(
      object_instance,
      [](void* obj, void* cd) {
          reinterpret_cast<FinalizerCallback>(cd)(obj);
      },
      reinterpret_cast<void*>(finalizer),
      nullptr, nullptr);
  ```

- [x] **3.5** 修改 `DrainFinalizerQueue`（line 916）：末尾添加 `GC_invoke_finalizers();`

- [x] **3.6** 创建 `tests/contracts/native/gc/gc_finalizer_smoke.cpp`：
  - 分配对象 → EnqueueFinalizer(counter++) → 置空引用 → GC_gcollect × 3 → DrainFinalizerQueue
  - assert counter > 0

- [x] **3.7** 修改 `GcSafepoint`（line 858）：
  ```cpp
  tis->at_gc_safepoint = true;
  GC_collect_a_little();
  tis->at_gc_safepoint = false;
  ```

- [x] **3.8** 创建 `tests/contracts/native/gc/gc_safepoint_smoke.cpp`：
  - 记录 GC_get_gc_no() 初始值 → 分配 10000 对象 + GcSafepoint × 1000 → 验证 gc_no 增长

- [x] **3.9** 扩展 ThreadingProof subject（如已有）验证多线程 GC 稳定性

**Batch 3 验收**: gc_thread_smoke / gc_finalizer_smoke / gc_safepoint_smoke 全通过；GC_get_gc_no() 在 safepoint 压测后 > 0；ThreadingProof 通过

---

### Batch 4: vtable 虚方法分派 (A.7)

- [x] **4.1** 创建 `src/native/runtime-core/vtable_registry.h`：
  ```cpp
  struct VTableSlot { uint32_t method_token; void* method_pointer; };
  struct TypeVTable {
      TypeInfoHandle type;
      TypeInfoHandle base_type;   // null = no base
      uint32_t slot_count;
      const VTableSlot* slots;
  };
  bool RegisterTypeVTable(RuntimeState*, const TypeVTable*);
  void* ResolveVirtualMethod(RuntimeState*, TypeInfoHandle instance_type, uint32_t method_token);
  ```

- [x] **4.2** 创建 `src/native/runtime-core/vtable_registry.cpp`：
  - 内部存储: `unordered_map<TypeInfoHandle, const TypeVTable*> s_vtable_map`
  - `ResolveVirtualMethod`: 从 instance_type 开始沿 base_type 向上查找 method_token，找到返回 method_pointer，未找到返回 nullptr

- [x] **4.3** 修改 `src/native/runtime-core/CMakeLists.txt`：添加 `vtable_registry.cpp` 到源列表

- [x] **4.4** 修改 `src/native/bootstrap/bootstrap.cpp`：
  - `ResolveVirtualMethod`（line 238）：调用 `vtable_registry::ResolveVirtualMethod` 替代当前 `UnresolvedVirtualCallEntry` 逻辑
  - `InvokeVirtual`（line 252）：拿到 method_pointer 后做函数指针调用

- [x] **4.5** 修改 `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`：
  - code-registration.json 中新增 `"vtables"` 数组：每个类型输出 `{ "type": "...", "baseType": "...", "slots": [{ "token": N, "methodRef": "..." }] }`

- [x] **4.6** 修改 `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`：
  - 为每个有虚方法的类型生成 C++ 代码：
    ```cpp
    static const VTableSlot vtable_slots_TypeName[] = { {token, &MethodName}, ... };
    static const TypeVTable vtable_TypeName = { &type_TypeName, &type_BaseType, N, vtable_slots_TypeName };
    // 在 bootstrap 注册调用中: RegisterTypeVTable(runtime_state, &vtable_TypeName);
    ```

- [x] **4.7** 创建 `subjects/VTableDispatchProof/`:
  - C# 代码: `Animal.Speak() → "animal"`, `Dog : Animal { Speak() → "dog" }`, `Cat : Animal { Speak() → "cat" }`
  - 通过 `Animal` 引用调用 Speak()，输出 key-value 验证多态
  - expected/runtime 包含预期输出

- [x] **4.8** 创建 `subjects/InterfaceDispatchProof/`（若不存在）：
  - `IShape.Area()`，`Circle : IShape`，`Rectangle : IShape`
  - 通过接口引用调用 Area()

**Batch 4 验收**:
- VTableDispatchProof: Dog → "dog", Cat → "cat"（通过基类引用）
- InterfaceDispatchProof: 两个实现类均返回正确值
- MainlineFeaturePack dispatch 验证点通过

---

### Batch 5: 完整异常处理 + 泛型运行时 (A.8–A.9)

- [x] **5.1** 在 `src/managed/Chaos.IL2CPP.Interpreter/InterpreterIR.cs` 的 `IRExceptionRegionKind` 枚举添加：
  ```csharp
  Filter,   // when() filter block
  Fault,    // fault block (执行但不吞异常)
  ```

- [x] **5.2** 在 `ILToIRLowering.cs` 添加 filter / fault IL block lowering：
  - `endfilter` → `IROpCode.EndFilter`
  - fault handler → `IRExceptionRegionKind.Fault`

- [x] **5.3** 在 `ManagedInterpreterExecutor.cs` 扩展异常分派：
  - Filter: 执行 filter block → 结果为 1 进 catch，0 跳过
  - Fault: 离开 try block 时无条件执行（正常/异常均执行）

- [x] **5.4** 扩展 NestedExceptionProof subject：
  - filter block 场景: `catch when (Condition())`
  - fault block 场景: `fault { Cleanup(); }`
  - 三层嵌套 try/catch/finally

- [x] **5.5** 创建 `subjects/CrossBoundaryExceptionProof/`：
  - AOT → interpreter throw → AOT catch ✓
  - interpreter → AOT throw → interpreter catch ✓
  - 三层跨越：AOT → interpreter → AOT throw → 逐层回退

- [x] **5.6** 创建 `src/native/runtime-core/generic_context.h`：
  ```cpp
  struct GenericContext {
      TypeInfoHandle* class_type_args; uint32_t class_arg_count;
      TypeInfoHandle* method_type_args; uint32_t method_arg_count;
  };
  GenericContextHandle CreateGenericContext(...);
  TypeInfoHandle ResolveClosedType(TypeInfoHandle open_type, TypeInfoHandle* type_args, uint32_t arg_count);
  ```

- [x] **5.7** 创建 `src/native/runtime-core/generic_context.cpp`：
  - 内部缓存 `map<tuple<TypeInfoHandle, vector<TypeInfoHandle>>, TypeInfoHandle>` 
  - `RegisterGenericInstantiation(open, closed, type_args, count)` 填充缓存

- [x] **5.8** 修改 `runtime_core.cpp`: `MethodGetGenericContext`（当前 stub at line ~582）返回已注册的 GenericContext

- [x] **5.9** 修改 `bootstrap.cpp`：metadata registration 阶段遍历 `generic_types`/`generic_methods` 数组，调用 `RegisterGenericInstantiation`

- [x] **5.10** 修改 `CodeGenStage.cs`：code-registration.json 输出泛型实例化表（open_type + type_args + closed_type token）

- [x] **5.11** 创建 `subjects/GenericCollectionProof/`：
  - `List<int>.Add(1); list.Count == 1`
  - `Dictionary<string, int>["key"] = 42; dict["key"] == 42`
  - 嵌套泛型: `List<List<int>>`

**Batch 5 验收**:
- CrossBoundaryExceptionProof 3 个场景全部通过
- NestedExceptionProof filter/fault 扩展通过
- GenericCollectionProof: List, Dictionary, 嵌套泛型均通过
- MethodGetGenericContext 返回非 null

---

### Batch 6: 委托多播 + IL opcode 扩展 (A.10–A.11)

- [x] **6.1** 修改 `bootstrap.cpp` 的 `DelegateInstance` 结构（line ~20）：
  ```cpp
  struct DelegateInstance {
      uint32_t method_token;
      void* method_pointer;
      void* target_instance;
      DelegateInstance* next;         // multicast chain (新增)
      uint32_t invocation_count;      // (新增)
  };
  ```

- [x] **6.2** 在 bootstrap.cpp 实现 `CombineDelegate(left, right) → DelegateInstance*`：
  - 克隆 left 链，将 right 追加到链尾，返回新头
  - Combine(null, d) → d；Combine(d, null) → d

- [x] **6.3** 在 bootstrap.cpp 实现 `RemoveDelegate(source, target) → DelegateInstance*`：
  - 从 source 链中移除最后一个匹配 target 的节点
  - 返回新链；链为空返回 nullptr

- [x] **6.4** 修改 `DelegateInvoke`（line 324）：遍历 next 链逐个调用，返回最后一次调用的返回值

- [x] **6.5** 创建 `subjects/DelegateChainProof/`：
  - 3 个 Action<int> 实例 Combine → Invoke → 计数器 == 3
  - Remove 最后一个 → Invoke → 计数器 == 2
  - event += / event -= 模式

- [x] **6.6** IL opcode 扩展 — 转换类（高优先级）：
  - 在 `ILToIRLowering.cs` 和 `NativeReferenceLoweringPlanner.cs` 添加：
    `conv.i1, conv.i2, conv.i4, conv.i8, conv.u1, conv.u2, conv.u4, conv.u8, conv.r4, conv.r8`
  - 创建 `subjects/ConversionOpsProof/`：覆盖所有 conv 变体

- [x] **6.7** IL opcode 扩展 — 位运算：
  - `and, or, xor, shl, shr, shr.un, not`
  - 创建 `subjects/BitwiseOpsProof/`

- [x] **6.8** IL opcode 扩展 — 比较/分支：
  - `beq, bne.un, bge, bge.un, ble, ble.un, bgt.un, blt.un, clt.un, cgt.un`
  - 创建 `subjects/BranchOpsProof/`

- [x] **6.9** IL opcode 扩展 — 对象操作：
  - `newobj, initobj, ldobj, stobj, isinst, castclass, sizeof`
  - 创建 `subjects/ObjectOpsProof/`

- [x] **6.10** IL opcode 扩展 — 数组操作：
  - `ldelema, ldelem.*, stelem.*`（常用类型变体）
  - 创建 `subjects/ArrayOpsProof/`

- [x] **6.11** IL opcode 扩展 — 溢出检查：
  - `add.ovf, sub.ovf, mul.ovf`（溢出时 throw OverflowException）
  - 创建 `subjects/OverflowOpsProof/`

- [x] **6.12** 全量 subject 回归：所有已有 subjects 无回归

**Batch 6 验收**:
- DelegateChainProof: multicast invoke 计数正确，Remove 正确
- 6 个 opcode proof subjects 全部通过
- 总覆盖 IL opcode ≥ 80 条（从当前 ~30 条）
- 所有已有 subjects 无回归

---

## Phase A 退出标准

完成以上全部 Batch 后确认：

- [x] BDWGC 增量 GC 运行，GC_get_gc_no() > 0
- [x] MainlineFeaturePack 全部 capability 通过（dispatch, generic, array, boxing, delegate, exception）
- [x] 连续分配 100 万对象无 crash、无内存泄漏
- [x] ThreadingProof 多线程场景通过
- [x] 所有已有 subjects（HelloWorldObject, GenericEcho, ReflectionLite, PInvokeLite）无回归
- [x] 所有 GC contract tests（gc_init / gc_handle / gc_thread / gc_finalizer / gc_safepoint）通过

---

## 执行时的任务目录维护

每个 Batch 完成后：
1. 勾选对应 checkbox
2. 更新 `STATUS.md` 中 `current_focus` 和最近摘要
3. 写一条 `notes/progress-v1-NN.md` 记录当前 batch 产出和遇到的问题
4. 全部完成后，在 `docs/dev/INDEX.md` 将本任务标记为 completed，并更新 `ACTIVE.md`
