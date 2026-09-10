# Phase C — 热更端到端 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。每完成一个 checkbox 立即标记。每个 Batch 完成后更新 `STATUS.md` 和 `notes/progress-v1-NN.md`。

**目标：** 实现热更包加载 → Interpreter 执行 → AOT 互操作的完整闭环，使 HotUpdateSkeletonProof 端到端可运行，AOT 方法可被热更方法替换。

**架构：** 分 5 批推进。Batch 1 扩展原生 Interpreter VM（int32 → tagged union）；Batch 2 串联包加载到 RuntimeManager；Batch 3 实现方法替换（bootstrap 钩子）；Batch 4 实现 Bridge 自动生成；Batch 5 版本管理 + 回滚 + 热更 benchmark subjects。

**技术栈：** C++17, C# .NET 8, CMake, std::variant (tagged union), std::atomic (method replacement)

**设计文档：** `docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md` Phase C 节

**预期知识沉淀：** `wiki/hot-update/interpreter-vm.md`, `wiki/hot-update/method-replacement.md`

---

## 涉及文件清单

| 文件 | 操作 | 所属步骤 |
|------|------|---------|
| `src/native/interpreter/interpreter_vm.h` | 修改 — 值类型从 int32 扩展到 InterpreterValue tagged union | C.1 |
| `src/native/interpreter/interpreter_vm.cpp` | 修改 — 新增 50+ opcode handler | C.1 |
| `tests/contracts/native/interpreter/interpreter_smoke.cpp` | 修改 — 补充 float/object 测试 | C.1 |
| `src/managed/Chaos.IL2CPP.Interpreter/ManagedInterpreterExecutor.cs` | 修改 — ExecuteInt32 → Execute，支持 object 返回 | C.1 |
| `src/native/hot-update/hot_update.h` | 修改 — 新增 LoadHotUpdatePackage/UnloadHotUpdatePackage | C.2 |
| `src/native/hot-update/hot_update.cpp` | 修改 — 实现包加载、元数据解析 | C.2 |
| `src/managed/Chaos.IL2CPP.HotUpdate/RuntimeManager.cs` | 修改 — LoadPackage 串联 ILToIRLowering + InterpreterVM | C.2 |
| `src/native/hot-update/method_replacement.h` | **创建** | C.3 |
| `src/native/hot-update/method_replacement.cpp` | **创建** | C.3 |
| `src/native/hot-update/CMakeLists.txt` | 修改 — 添加 method_replacement.cpp | C.3 |
| `src/native/bootstrap/bootstrap.cpp` | 修改 — FindMethodPointerByToken 查 method_replacement 表 | C.3 |
| `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs` | **创建** | C.4 |
| `src/managed/Chaos.IL2CPP.HotUpdate/BridgeDispatcher.cs` | 修改 — 动态 bridge 注册 | C.4 |
| `subjects/HotUpdateSkeletonProof/` | 修改 — 扩展为加载真实包并执行 | C.2, C.3 |
| `subjects/BenchHotUpdateLoad/` | **创建** | C.5 |
| `subjects/BenchHotUpdateDispatch/` | **创建** | C.5 |
| `subjects/BenchHotUpdateRoundtrip/` | **创建** | C.5 |

---

## 步骤

### Batch 1: Interpreter VM 类型扩展 (C.1)

**目标**: 原生 interpreter 从 int32 only → 支持 float/double/object/string/array

- [x] **1.1** 修改 `interpreter_vm.h`，扩展 `IROpCode` 枚举至 50+ 条:
  - 新增: `LdcI8, LdcR4, LdcR8, LdStr, LdNull`
  - 新增: `NewObj, NewArr, LdFld, StFld, LdElem, StElem, LdLen`
  - 新增: `Box, Unbox, CastClass, IsInst`
  - 新增: `Call, CallVirt, CallBridge`
  - 新增: `Conv_I4, Conv_I8, Conv_R4, Conv_R8`
  - 新增: `EndFilter` (从 Phase A InterpreterIR.cs 同步)

- [x] **1.2** 修改 `interpreter_vm.h`，将值类型从 `int32_t` 改为 tagged union:
  ```cpp
  enum class ValueTag : uint8_t { Void, Int32, Int64, Float32, Float64, ObjectRef, Null };
  struct InterpreterValue {
      ValueTag tag;
      union { int32_t i32; int64_t i64; float f32; double f64; void* obj; };
      // 默认构造为 Void/0
      static InterpreterValue from_i32(int32_t v) { InterpreterValue r{}; r.tag=ValueTag::Int32; r.i32=v; return r; }
      static InterpreterValue from_obj(void* p)   { InterpreterValue r{}; r.tag=ValueTag::ObjectRef; r.obj=p; return r; }
      static InterpreterValue null_val()           { InterpreterValue r{}; r.tag=ValueTag::Null; return r; }
  };
  ```

- [x] **1.3** 修改 `ExecutionFrame` — 将所有 `std::vector<int32_t>` 改为 `std::vector<InterpreterValue>`:
  - `arguments`, `locals`, `evaluation_stack` 全部改为 `InterpreterValue`

- [x] **1.4** 修改 `InterpreterVM::Execute()` 返回 `ExecutionResult`，将 `has_return_value` + `int32_value` 替换为单个 `InterpreterValue return_value`

- [x] **1.5** 实现 `interpreter_vm.cpp` 中每个新 opcode 的 handler（switch case 扩展）:
  - LdcR4/R8: push float literal
  - LdStr: push string pointer（从 IRInstruction 的 string operand 读取）
  - LdNull: push null ObjectRef
  - NewObj/NewArr: 通过 GC_MALLOC 分配，返回 ObjectRef
  - LdFld/StFld: 通过 byte offset 访问 ObjectRef 的字段
  - LdElem/StElem/LdLen: 数组操作
  - Box/Unbox: 值类型装箱/拆箱
  - Conv_I4/I8/R4/R8: 类型转换
  - CallBridge: 调用注册的 bridge 函数指针（通过 bridge_registry）

- [x] **1.6** 保持向后兼容：现有 `gc_init_smoke / gc_handle_smoke / gc_thread_smoke / gc_finalizer_smoke / gc_safepoint_smoke` + `chaos_interpreter_smoke` 全部仍通过

- [x] **1.7** 修改 `ManagedInterpreterExecutor.cs`：`ExecuteInt32` 可继续工作，新增 `Execute(method, args) → object?` 泛化接口

**Batch 1 验收**:
- `cmake --build` 成功（无 error）
- 现有 5 个 GC smoke tests 通过
- `chaos_interpreter_smoke.exe` 通过（int32 arithmetic 仍正确）
- 新增 float/object 操作 unit test: `LdcR4(3.14f) → Ret` 返回正确 float

---

### Batch 2: HotUpdatePackage 运行时加载器串联 (C.2)

**目标**: 包加载 → ILToIRLowering → InterpreterVM 注册 → 可调用

- [x] **2.1** 修改 `src/native/hot-update/hot_update.h`，新增:
  ```cpp
  struct HotUpdatePackageHandle {
      HotUpdateAssemblyImage assembly_image;
      SupplementalMetadataImage metadata_image;
      char* package_id;          // GC_MALLOC'd
      char* target_aot_version;  // GC_MALLOC'd
      bool loaded;
  };
  bool LoadHotUpdatePackage(const char* package_root_utf8, HotUpdatePackageHandle* out_handle);
  void UnloadHotUpdatePackage(HotUpdatePackageHandle* handle);
  ```

- [x] **2.2** 实现 `hot_update.cpp` 中 `LoadHotUpdatePackage`:
  - 读取 `{package_root}/manifest.json` → 解析 PackageId, TargetAotVersion
  - 验证 formatVersion == "v0"
  - 加载 assembly DLL → `LoadAssemblyImageFromPath`
  - 加载 supplemental metadata → `LoadSupplementalMetadataFromPath`
  - 返回 `HotUpdatePackageHandle`，失败返回 false

- [x] **2.3** 修改 `RuntimeManager.cs`：`LoadPackage` 新增重载:
  ```csharp
  public bool LoadPackage(
      string packageRootPath,
      string currentAotVersion,
      Func<string, IRMethod>? methodResolver = null,
      Func<string, IReadOnlyList<object?>, object?>? bridgeInvoker = null)
  ```
  - 读 manifest.json → 构建 HotUpdatePackage
  - 对每个 assembly → 调用 `ILToIRLowering.Lower(methods)` → 得到 `IRMethod[]`
  - 将每个 IRMethod 注册到 `HotUpdateMethodRegistry`，key = MethodSubjectId
  - 切换 RuntimeMode → Mixed

- [x] **2.4** 扩展 `subjects/HotUpdateSkeletonProof/` 源码，改为测试真实加载路径:
  - C# 源码: 一个 `Add(int a, int b) → a + b` 方法
  - HotUpdate package 构建脚本（或预构建 artifacts）
  - 验证: `RuntimeManager.LoadPackage → DispatchInt32("Add", 3, 4) == 7`

- [x] **2.5** 验证: HotUpdateSkeletonProof subject 端到端 `source-resolve → analysis → package-build → runtime-execute` 通过

**Batch 2 验收**:
- `LoadHotUpdatePackage` 成功加载真实 .dll 包
- `RuntimeManager.LoadPackage` 执行后 `IsPackageLoaded == true`
- `DispatchInt32("HotUpdateSkeletonProof/Add", 3, 4)` 返回 7

---

### Batch 3: 运行时方法替换 (C.3)

**目标**: 加载热更包后，AOT 方法的调用方自动走 interpreter

- [x] **3.1** 创建 `src/native/hot-update/method_replacement.h`:
  ```cpp
  namespace chaos::il2cpp::method_replacement {
  struct MethodReplacementEntry {
      uint32_t method_token;
      void*    original_pointer;  // 保存原始 AOT 函数指针（用于 Revert）
      void*    replacement_thunk; // interpreter dispatch thunk
      bool     active;
  };
  bool Register(uint32_t method_token, void* thunk);
  bool Revert(uint32_t method_token);
  void RevertAll();
  void* Resolve(uint32_t method_token);  // null = 无替换
  uint32_t ActiveCount();
  }
  ```

- [x] **3.2** 实现 `src/native/hot-update/method_replacement.cpp`:
  - 内部存储: `std::unordered_map<uint32_t, MethodReplacementEntry>`
  - 写锁 (`std::mutex`) 保护 Register/Revert，读路径尽量无锁
  - `Resolve(token)` — 查表，命中返回 replacement_thunk，否则 nullptr

- [x] **3.3** 修改 `src/native/hot-update/CMakeLists.txt`: 添加 `method_replacement.cpp`

- [x] **3.4** 修改 `src/native/bootstrap/bootstrap.cpp` 的 `FindMethodPointerByToken`:
  ```cpp
  void* FindMethodPointerByToken(uint32_t method_token) {
      // 1. 先查 method_replacement 表（热更优先）
      if (void* thunk = chaos::il2cpp::method_replacement::Resolve(method_token))
          return thunk;
      // 2. 回落到原始 code_registration 表
      // ... 原有逻辑
  }
  ```

- [x] **3.5** 创建 `subjects/MethodReplacementProof/` subject:
  - AOT 方法 `Greet() → "hello from AOT"`
  - 加载热更包 → `Register(method_token, thunk)` → 调用 `Greet()` → `"hello from interpreter"`
  - `Revert(method_token)` → 调用 `Greet()` → `"hello from AOT"`

- [x] **3.6** 验证 MethodReplacementProof:
  - replace → call → revert → call 四步全部返回正确值
  - 并发调用不 crash（写操作加锁验证）

**Batch 3 验收**:
- `method_replacement.cpp` 编译到 `chaos_hot_update.lib`
- MethodReplacementProof: 4 步验证全部通过
- 并发 smoke test：10 线程同时调用被替换方法，无 crash

---

### Batch 4: AOT↔Interpreter Bridge 自动生成 (C.4)

**目标**: 根据方法签名自动生成 bridge stub，不需要手动配置 spec

- [x] **4.1** 创建 `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs`:
  ```csharp
  public static class AutoBridgeGenerator {
      // 支持的签名模式 (扩展可加):
      // void()           → VoidVoidThunk
      // int(int)         → Int32UnaryThunk
      // int(int, int)    → Int32BinaryThunk
      // string(string)   → StringUnaryThunk
      // object(object)   → ObjectUnaryThunk
      public static BridgePlan Generate(
          IReadOnlyList<ManagedMethodModel> aotMethods,
          IReadOnlyList<ManagedMethodModel> hotUpdateMethods) { ... }
  }
  ```
  - 对每个热更方法，自动匹配签名 → 生成对应 bridge spec
  - 不匹配的签名 → 抛出 `NotSupportedException`（给出清晰错误消息）

- [x] **4.2** 修改 `BridgeDispatcher.cs` — 新增动态注册接口:
  ```csharp
  public void RegisterAutoGenerated(BridgePlan plan) { ... }
  public object? Dispatch(string bridgeId, IReadOnlyList<object?> args) { ... }
  ```

- [x] **4.3** 集成到 `RuntimeManager.cs`：`LoadPackage` 完成后自动调用 `AutoBridgeGenerator.Generate` + `BridgeDispatcher.RegisterAutoGenerated`

- [x] **4.4** 创建 `subjects/AutoBridgeProof/`:
  - AOT 定义 `ICalculator.Add(int, int) → int`
  - 热更替换 Calculator.Add → interpreter 执行
  - 通过接口调用 → auto bridge → interpreter → 返回正确值
  - 测试 5 种签名: void(), int(int), int(int,int), string(string), object(object)

**Batch 4 验收**:
- AutoBridgeProof: 5 种签名全部通过
- 签名不匹配时 `NotSupportedException` 包含清晰错误信息
- `BridgeDispatcher.Dispatch` 正确路由所有已注册 bridge

---

### Batch 5: 版本管理 + 回滚 + 热更 Benchmark (C.5)

**目标**: 包历史栈 + 回滚 + 三个热更 benchmark subjects

- [x] **5.1** 修改 `RuntimeManager.cs`，新增包历史栈:
  ```csharp
  private readonly Stack<LoadedHotUpdatePackage> _history = new();
  public bool CanRollback => _history.Count > 0;
  public void Rollback() {
      // pop → RevertAll() → UnloadHotUpdatePackage → 切回 AOT mode
  }
  ```

- [x] **5.2** 实现版本兼容性检查（已有 `HotUpdateVersionCompatibility.cs`）:
  - package.TargetAotVersion 与 currentAotVersion major.minor 必须匹配
  - major 不同 → 拒绝加载，返回明确错误

- [x] **5.3** 创建 `subjects/VersionRollbackProof/`:
  - 加载 v1 → 验证 → 加载 v2 → 验证 → Rollback 到 v1 → 验证 → Rollback 到 AOT-only → 验证
  - 版本不兼容拒绝测试: targetAotVersion="2.0" + 当前 AOT="1.0" → LoadPackage 返回 false

- [x] **5.4** 创建 `subjects/BenchHotUpdateLoad/`:
  - 测量 `LoadPackage` 从文件到可执行的耗时 (< 500ms)
  - 输出 JSON: `{"loadDurationMs": N, "methodCount": N}`

- [x] **5.5** 创建 `subjects/BenchHotUpdateDispatch/`:
  - AOT → interpreter thunk 分派 × 10000 次
  - 输出 JSON: `{"elapsedMilliseconds": N, "iterations": N, "opsPerSecond": N}`

- [x] **5.6** 创建 `subjects/BenchHotUpdateRoundtrip/`:
  - AOT → interpreter → AOT callback → interpreter 返回 × 1000 次
  - 输出 JSON: `{"elapsedMilliseconds": N, "roundtripsPerSecond": N}`

- [x] **5.7** 将 BenchHotUpdate* 接入 benchmark CLI (manifest + benchmark-records)

**Batch 5 验收**:
- VersionRollbackProof: 四步验证全通过
- 版本不兼容: `LoadPackage` 返回 false 且错误消息包含版本号
- BenchHotUpdateLoad: 输出 `loadDurationMs < 500`
- 三个热更 benchmark subjects 可通过 `run benchmark --record`

---

## Phase C 退出标准

- [x] HotUpdateSkeletonProof 端到端：加载包 → DispatchInt32 → 正确结果
- [x] MethodReplacementProof：替换 → 调用走 interpreter → Revert → 走 AOT
- [x] AutoBridgeProof：5 种签名 auto bridge 全通过
- [x] VersionRollbackProof：4 步回滚测试全通过
- [x] BenchHotUpdateLoad / Dispatch / Roundtrip 三个 benchmark subjects 可运行
- [x] interpreter_vm 支持 ≥ 50 opcodes（从当前 17 扩展）
- [x] CMake 全量构建无 error

---

## 执行时的任务目录维护

每个 Batch 完成后:
1. 勾选 checkbox
2. 更新 `STATUS.md`
3. 写 `notes/progress-v1-NN.md`
4. 全部完成后归档到 `docs/dev/completed/`，删除 `ACTIVE.md`
