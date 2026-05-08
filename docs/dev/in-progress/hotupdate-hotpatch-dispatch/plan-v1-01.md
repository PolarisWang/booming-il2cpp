# HotUpdate Hotpatch 双层分派架构 — 实现计划

> **面向执行 Agent：** 使用 `dev:executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 为 foundation-dll 验证体系实现真实 hotupdate 路径：patch 代码以托管 DLL 形式存在，构建期提取为 `.patchdata`，运行时通过 Dispatch Table + Interpreter 执行 IL，替代当前静态 3-TU C++ 预编译模式。先打通 string-char-text-core 全链路。

**架构：** Hotpatch（原名 D3）双层分派。每个模块生成静态 Dispatch Table，call site 采用模式感知分支——AOT→AOT 零额外开销，patch 时走 InterpreterEntryDirect → Lazy IL→IR 降低 → Interpreter 执行。PatchMetadataCache 本地自洽，不注册全局类型系统（iOS 兼容）。

**技术栈：** C++17, codegen C# emit pipeline, Python 测试脚本, FlatBuffer 自定义二进制格式

**架构审核模式：** critical

**结构告警重点：** PatchLoader 与现有 runtime_core 的集成边界；PatchMetadataCache 与 DefaultTokenResolver 的交互；codegen emit-patch-data 与现有 emit-native-aot 的职责隔离

**权责图审核主题：** `patch_loader`（PatchLoader/PatchMethod/PatchMetadataCache）vs 现有 `runtime_instantiation`/`generic_method_instantiation` 的职责重叠

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: foundation-dll (string-char-text-core)
- capabilityItem: HotUpdateHotpatch
- ownerSubjectId: runtime-core
- proofRequired: true
- benchmarkRequired: false（hotupdate 验证不涉及 benchmark）
- hotupdateImpact: Proof（验证 patched method 通过 interpreter 执行的全路径）
- formalVerificationObjects: [PatchMethod IL→IR 降低, DispatchTable flags 管理, PatchMetadataCache token 解析, ApplyPatch/Revert 生命周期]
- requiredGates: unit test -> native build -> hotupdate e2e

**设计文档：** `wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchHotpatch-决策.md`

**问题清零来源：** direct user confirmation（17 条决策逐条对齐）

**计划来源：** direct-plan

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 合并&提交"固定链路。

---

## 决策汇总

| # | 决策项 | 结论 |
|:-|-------|------|
| 1 | 方法匹配 | 运行时 Token 重映射（名称匹配 + NameIndex） |
| 2 | Dispatch 机制 | Hotpatch 双层 ABI：dispatch table + direct call |
| 3 | Dispatch Table | Per-module 平面静态表，构建期确定 |
| 4 | Call site 模式 | 模式感知分支 |
| 5 | PE 解析 | 构建期完整 Metadata 提取，PatchMetadataCache 本地自洽 |
| 6 | 数据格式 | FlatBuffer 风格 .patchdata |
| 7 | 提取工具 | 集成到 codegen C# 管道 |
| 8 | Metadata 注册 | 本地 cache，不注册全局类型系统 |
| 9 | Name Index | 两级索引（Type → Method），Bootstrap 集成 |
| 10 | Token → Slot | 构建期 emit 反向索引 |
| 11 | IL→IR 降低 | Lazy（首次调用降低） |
| 12 | Token 解析 | 本地自洽（引用闭包） |
| 13 | 参数解析 | 运行时签名解析 |
| 14 | 卸载 | 逐方法恢复，第一阶段无 RCU |
| 15 | Interpreter entry | 直接指针 PatchMethod* |
| 16 | 测试框架 | 改造 11 步流程 → 统一 Harness |
| 17 | 阶段 | 先打通全链路 |

---

## 步骤 1：构建期 NameIndex + Token→Slot emit

**目标：** codegen 为每个 AOT 模块额外 emit NameIndex（两级索引）和 Token→Slot 反向索引表。

- [ ] **1.1** 在 codegen C# 中新增 `NameIndex` 数据结构定义（TypeIndexEntry + MethodIndexEntry）
- [ ] **1.2** 在 NativeAotLoweringPlanner 中收集所有公开方法的 (type_name, method_name, method_token, slot)
- [ ] **1.3** 按 type→method 两级排序，emit `g_type_index[]` + `g_method_index[]`
- [ ] **1.4** emit `g_token_to_slot[]`（升序排列，供 bsearch）
- [ ] **1.5** Codegen emit `RegisterModuleNameIndex()` 函数，插入 Bootstrap 注册流程
- [ ] **1.6** 验证：生成的 C++ 中 `.rodata` 段包含 NameIndex 数据

**涉及文件：**
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.*.cs` — 新增 emit
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs` — 注册流程
- `contracts/native/v0/codegen_bridge.h` — 可能的 struct 扩展

---

## 步骤 2：运行时 NameIndexRegistry + DispatchTable 基础设施

**目标：** 运行时侧接收 codegen 生成的 NameIndex 数据，支持加载期名称查询和 dispatch 路由。

- [ ] **2.1** 新增 `src/native/runtime-core/dispatch_table.h`：`DispatchEntry` struct + `DispatchTable` per-module 管理
- [ ] **2.2** 新增 `NameIndexRegistry`：两级 bsearch 查找（先查 type → method 范围，再 bsearch method）
- [ ] **2.3** 实现 `TokenSlotIndex`：token→slot 反向 bsearch
- [ ] **2.4** Bootstrap 集成：在 `RegisterCodegen()` / `BootstrapRuntime()` 中注册 NameIndex
- [ ] **2.5** 实现 `g_dispatch_table` 静态声明宏（codegen 使用的 extern 符号）
- [ ] **2.6** 实现 `RuntimeDispatchLookup(token)`：查 NameIndex → slot → entry（patch 加载时使用）
- [ ] **2.7** 实现 `CallViaSlot(slot, args_buf, ret)`: dispatch table 调用 helper

**涉及文件：**
- `src/native/runtime-core/dispatch_table.h`（新增）
- `src/native/runtime-core/dispatch_table.cpp`（新增）
- `src/native/bootstrap/bootstrap.cpp` — 集成注册

---

## 步骤 3：构建期 patch data 提取工具

**目标：** codegen 能处理 patch variant，读取 patch.dll 的 PE Metadata，输出 `.patchdata` 文件。

- [ ] **3.1** 在 codegen C# 中新增 `PatchDataExtractor` 类
- [ ] **3.2** 实现 PE→CLR header→Metadata root→#~ table 解析（复用 `LoaderStage` 现有 PE 读取能力）
- [ ] **3.3** 实现 TypeDef/MethodDef/TypeRef/MemberRef/AssemblyRef 提取
- [ ] **3.4** 实现 #Strings / #Blob stream 提取
- [ ] **3.5** 实现 MethodBody RVA → raw IL byte 提取
- [ ] **3.6** 定义 `PatchDataHeader` + `PatchTypeEntry` + `PatchMethodEntry` struct 布局
- [ ] **3.7** 实现 FlatBuffer 序列化输出 `.patchdata`
- [ ] **3.8** 新增 `emit-patch-data` codegen 命令（或 patch variant emit 分支）
- [ ] **3.9** 集成到 `batch_hotupdate_runner.py`：patch variant 改为 `emit-patch-data` + copy .patchdata
- [ ] **3.10** 验证：单个 family 能产出合法的 `.patchdata` 文件

**涉及文件：**
- `src/managed/Chaos.IL2CPP.CodeGen/PatchDataExtractor.cs`（新增）
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.*.cs` — 新增 emit path
- `build/toolchains/run/testing/foundation_dll/batch_hotupdate_runner.py` — 修改 pipeline

---

## 步骤 4：运行时 PatchLoader

**目标：** PatchLoader 加载 `.patchdata`，构建 PatchMethod，通过名称匹配填写 dispatch table。

- [ ] **4.1** 新增 `src/native/runtime-core/patch_loader.h/.cpp`
- [ ] **4.2** 定义 `PatchMethod` struct（IL bytes + 签名 + cached_ir）
- [ ] **4.3** 定义 `PatchMetadataCache` struct（内存映射 FlatBuffer 的访问接口）
- [ ] **4.4** 实现 `ApplyPatchFromMemory(data, size)`：
  - 验证 magic + version
  - mmap/cast 到 PatchDataHeader
  - 遍历 MethodEntry → NameIndexRegistry 查找 → token→slot → 构建 PatchMethod → 填 dispatch table
- [ ] **4.5** 实现 `Unpatch(patch_ctx)`：逐 slot 恢复 direct_ptr、清 flag、delete PatchMethod
- [ ] **4.6** NameIndex 查找返回 aot_token 后，通过 token→slot 索引找到 slot 号
- [ ] **4.7** 实现 PatchMetadataCache 的 token resolver 接口（供 `il_to_ir_lowerer` 使用）
- [ ] **4.8** 实现 TokenResolverContext 构建：用 PatchMetadataCache 作为 source_image

**涉及文件：**
- `src/native/runtime-core/patch_loader.h`（新增）
- `src/native/runtime-core/patch_loader.cpp`（新增）
- `src/native/runtime-core/dispatch_table.h/.cpp` — dispatch table 操作

---

## 步骤 5：InterpreterEntryDirect

**目标：** dispatch table 的 interrupt_ptr 指向的入口函数，负责签名解析、ExecutionFrame 构建和 InterpreterVM 调用。

- [ ] **5.1** 新增 `src/native/runtime-core/interpreter_entry.h/.cpp`
- [ ] **5.2** 实现 `InterpreterEntryDirect(method_key: uintptr_t, args: ArgBuffer*) -> void`：
  - (PatchMethod*)method_key → 获取 IL + param_types
  - GetOrLowerIR()：double-checked locking 延迟降低
    - ParseMethodBodyHeader + TokenResolverContext 构建（使用 PatchMetadataCache 作为 source_image）
    - LowerILToIR() → 缓存到 PatchMethod::cached_ir
  - 运行时签名解析：遍历 param_types → ReadArgFromBuffer → InterpreterValue[]
  - 构建 ExecutionFrame，设置 dispatch_fn = InterpreterDispatch
  - InterpreterVM::Execute()
  - WriteRetToBuffer (return_type → ArgBuffer)
- [ ] **5.3** `ArgBuffer` 工具类：支持按类型 read/write（i32/i64/f32/f64/ptr）
- [ ] **5.4** 共享的 `InterpreterEntryDirect` 函数指针（被所有 dispatch table entry 的 interrupt_ptr 使用）
- [ ] **5.5** 单元测试：直接调用 InterpreterEntryDirect 验证 Invocation、返回值、Lazy IR 缓存

**注意：** IL→IR 降低需要正确的 TokenResolverContext。PatchMetadataCache 必须实现 `ILTokenSource` 接口，拦截 token 解析：
- TypeDef/MethodDef 范围的 token → 本地查找
- TypeRef/MemberRef → 委托给 token_resolver.cpp 的 `DefaultTokenResolver()` 解析

**涉及文件：**
- `src/native/runtime-core/interpreter_entry.h`（新增）
- `src/native/runtime-core/interpreter_entry.cpp`（新增）
- `src/native/runtime-core/patch_loader.h/.cpp` — PatchMethod::GetOrLowerIR

---

## 步骤 6：Codegen 改造 — Dispatch Table + 模式感知分支

**目标：** 改造 codegen 的 call site emit：不再直接调用 `FindMethodPointerByToken` + 函数指针转换，而是生成 Dispatch Table 查找 + 模式感知分支。

- [ ] **6.1** codegen emit 每个模块的 `g_dispatch_table[]`（静态填充 direct_ptr、interrupt_ptr、flags）
- [ ] **6.2** 修改 call site emit 逻辑：
  - 对公开方法调用：emit `if (entry.flags & kPatched) ... else direct_call`
  - 对私有/内部调用：保留直接调用（不经过 dispatch table）
- [ ] **6.3** 保留 `FindMethodPointerByToken` 用于 `method_replacement` 测试
- [ ] **6.4** 保证原有的 genuine/benchmark 路径不受影响（dispatch table 初始时 interrupt_ptr = direct_ptr）
- [ ] **6.5** 验证：生成的 C++ 中 call site 包含模式分支，dispatch table 段存在

**涉及文件：**
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — call site emit
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` — dispatch table emit

---

## 步骤 7：Patch data 嵌入 + HotUpdateTest 改造

**目标：** HotUpdateTest 不再引用 3-TU 的 patch C++ 符号，改为嵌入 `.patchdata` + 统一 Harness 验证。

- [ ] **7.1** 修改 `generate_hotupdate_test.py`：
  - patch 变体的 C++ TU 不再生成
  - 使用 `xxd -i` 或等效方法将 `.patchdata` 嵌入为 `const uint8_t[]` + 注册函数
  - 生成统一 Test Harness（遍历 kTestTable，执行 7 步验证）
- [ ] **7.2** Test Harness 流程：
  ```
  1. ApplyPatchFromMemory(kPatchData, size) → PatchContext
  2. 遍历 kTestTable[]:
     a. baseline = entry_fn()
     b. assert(dispatch_table[slot].flags & kPatched)
     c. patched = entry_fn()
     d. assert(patched != baseline)
     e. method_replacement::Revert(token)
     f. dispatch_table[slot].flags &= ~kPatched
     g. restored = entry_fn()
     h. assert(restored == baseline)
  3. Unpatch(ctx)
  ```
- [ ] **7.3** 修改 CMakeLists.txt：链接 PatchLoader 库 + .patchdata 嵌入，不再链接 patch TU
- [ ] **7.4** 删除不再需要的 semantic-patch TU（patch 通过 interpreter 执行后，semantic-patch 的职责被 `ApplyPatch` 替代）
- [ ] **7.5** 保留 genuine-fixed TU（AOT 代码）
- [ ] **7.6** 验证：以 string-char-text-core 为例，exe 正确执行 7 步验证

**涉及文件：**
- `build/toolchains/run/testing/foundation_dll/generate_hotupdate_test.py` — 重写
- `verification/foundation-dll/System.Private.CoreLib/string-char-text-core/il2cpp_dist/hotupdate/CMakeLists.txt` — 改链接
- `verification/foundation-dll/System.Private.CoreLib/string-char-text-core/il2cpp_dist/hotupdate/HotUpdateTest.cpp` — 测试入口

---

## 步骤 8：构建与全链路验证

- [ ] **8.1** 构建 `chaos_runtime_core` 库（含 PatchLoader + DispatchEntry + InterpreterEntry）
- [ ] **8.2** 对 string-char-text-core 跑全量 codegen：patch variant → emit-patch-data → .patchdata
- [ ] **8.3** 修改后的 `batch_native_aot_runner.py` 完整通过
- [ ] **8.4** `chaos_hotupdate_string_char_text_core.exe` 跑通
- [ ] **8.5** 验证 7 步验证全部通过（baseline→patch→revert→restore）
- [ ] **8.6** 验证 Trace 系统可定位失败原因（trace span 覆盖 dispatch、interpreter entry、Lazy IR lower）

**验证命令：**
```bash
cmake --build solution_native --target chaos_runtime_core --config Release
python build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py --family string-char-text-core
python build/toolchains/run/testing/foundation_dll/batch_hotupdate_runner.py --family string-char-text-core
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/string-char-text-core/native/Release/chaos_hotupdate_string_char_text_core.exe
```

---

## 涉及文件完整列表

| 文件 | 操作 | 说明 |
|------|------|------|
| `src/managed/Chaos.IL2CPP.CodeGen/PatchDataExtractor.cs` | 新增 | 构建期 patch.dll → .patchdata 提取 |
| `src/managed/Chaos.IL2CPP.CodeGen/Emission/*.cs` | 改 | 新增 NameIndex / Token→Slot / DispatchTable emit |
| `src/native/runtime-core/dispatch_table.h/.cpp` | 新增 | DispatchEntry + NameIndexRegistry + TokenSlotIndex |
| `src/native/runtime-core/patch_loader.h/.cpp` | 新增 | PatchMethod + PatchMetadataCache + ApplyPatch/Unpatch |
| `src/native/runtime-core/interpreter_entry.h/.cpp` | 新增 | InterpreterEntryDirect + ArgBuffer + Lazy IR lower |
| `src/native/bootstrap/bootstrap.cpp` | 改 | Bootstrap 注册 NameIndexRegistry |
| `contracts/native/v0/codegen_bridge.h` | 改 | 可能扩展 NameIndexModule 结构 |
| `build/toolchains/run/testing/foundation_dll/batch_hotupdate_runner.py` | 改 | patch variant 改为 emit-patch-data |
| `build/toolchains/run/testing/foundation_dll/generate_hotupdate_test.py` | 改 | 不再生成 patch TU，嵌入 .patchdata + Harness |
| `verification/.../*family*/il2cpp_dist/hotupdate/CMakeLists.txt` | 改 | 链接 PatchLoader |
| `wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchD3-决策.md` | 已创建 | 架构决策记录 |
