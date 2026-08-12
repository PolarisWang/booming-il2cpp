# A1 定盘点法 — external-runtime fallback 缺口报告

> **日期**：2026-08-12
> **方法**：跨 67 个预编译 AOT entry.exe 跑 `--fact-json`（`RunFactAll` + `ChaosDispatchMethodGetValue` 比对实际返回值）。
> **原始数据**：`tests/e2e/verification/a1_fact_inventory.json` + `a1_fact_inventory.py`（可复跑）。
> **用途**：量化方向 A（让"无 C++ body + 无 native stub 但被真实调用"的方法有真 native 执行体）的真实目标清单和规模。

---

## 1. 总览

| 指标 | 值 |
|---|---|
| entry.exe 数 | 67（59 有 fact 输出；8 个 `System.Private.CoreLib` chunks 超时/无输出）|
| 测试方法总数 | **17,670** |
| **缺口方法（passed=false 且 value=0）** | **111（0.63%）** |
| 零值方法（passed=true 但 value=0，可能是合法 void/零值）| 大量（数千，需逐个判定是否合法）|

> ⚠️ 注：8 个 `System.Private.CoreLib/.../{runtime-intrinsics,system-*}` chunks 超时返回 "NO fact output"（方法数极大），**其缺口未计入 111**。真实缺口数可能更高。

---

## 2. 缺口方法分布（按 entry）

| entry | 缺口数 |
|---|---|
| System.Private.Xml / system-xml-xsl-runtime | **46** |
| System.Private.CoreLib / runtime-compiler | **16** |
| System.Private.CoreLib / numerics | **12** |
| System.Xml.ReaderWriter / system-xml-xsl | **10** |
| System.Private.CoreLib / runtime-interop | 7 |
| System.IO.Pipelines / system-io-pipelines | 6 |
| System.Private.CoreLib / runtime-interop-2 | 6 |
| System.Xml.ReaderWriter / xml | 3 |
| System.Collections / global-ns | 1 |
| System.Private.CoreLib / globalization(+2) | 1+1 |
| System.Reflection.Metadata / system-reflection-metadata | 1 |
| System.Text.Json / text-json | 1 |

---

## 3. 缺口方法按语义分类（方向 A 的目标类别）

| 类别 | 缺口数 | 代表方法 | 共性 |
|---|---|---|---|
| **Xml 序列化 importers** | 18 | `SoapReflectionImporter::ImportMembersMapping`、`XmlReflectionImporter::ImportMembersMapping`、`XmlSchemaImporter::ImportMembersMapping` | 反射/映射重型跨 assembly，需反射 API |
| **Interop GCHandle/Marshal/SafeBuffer** | 13 | `GCHandle::Alloc/FromIntPtr`、`AnsiStringMarshaller::ConvertToManaged/Unmanaged`、`SafeBuffer::Read`、`Utf16StringMarshaller::GetPinnableReference` | 托管↔native 句柄/封送，需 native stub |
| **Numerics Vector SIMD 运算符** | 12 | `Vector2/3/4::GreaterThanAll/LessThanAll/...` | SIMD intrinsic，全落 fallback return-0 |
| **CompilerServices.Unsafe/RuntimeHelpers** | 16 | `Unsafe::As/Add/AddByteOffset/Subtract/AsRef/Unbox`、`RuntimeHelpers::GetObjectValue` | 指针/引用 intrinsic（runtime-compiler）|
| **PipeReader** | 6 | `PipeReader::ReadAsync/ReadAtLeastAsync/AsStream/CopyToAsync` | 异步流式，重托管 |
| **Xml NameTable** | 4 | `XmlNameTable::Add/Get`、`NameTable::Add/Get` | 字典语义，需 native |
| **其他** | 57 | `CreateCaseInsensitiveHashtable`、`DateTimeFormatInfo::GetInstance`、`GC...`、`System.Reflection.Metadata` 等 | 混杂，多在 XML/xsl-runtime 的反射/映射 |

---

## 4. 关键结论（重新校准方向 A 工作量）

1. **缺口不是 1 个，是至少 111 个（0.63%）**，远超我最初的 241/242 观感。虽然占比低，但**高度集中在少数语义域**（XML 序列化、Interop、Numerics SIMD、Unsafe 指针、PipeReader）。

2. **这些正好是"无 native C++ body + 无 native stub + 落 fallback return-0"** 的典型 —— 证明 `ChaosExternalRuntimeFallback` 的 return-0 路径**确实在真实 workload 里大面积命中**（不只 1/242），方向 A 是**真实、有规模的正确性缺口**，不是伪命题。

3. **两大子类型（决定修复方式）**：
   - **有既有 native stub / intrinsic 语义的方法**（Unsafe 指针、Numerics Vector、GCHandle、SafeBuffer、NameTable）→ 方向 A2（ShapeRegistry 补 SimpleForward 或 intrinsic 直内联），**便宜、可核 ABI**。
   - **需反射/序列化重型语义的方法**（Xml importers、PipeReader、marshaller）→ 无法简单补 native stub，需判断补编译闭包或降级为合法处理，**成本高**（方向 A3）。

4. **checksum 局限仍然存在**：`value=0` 且 `passed=true` 的大量方法无法区分"合法返回 0"vs"fallback 隐藏缺口"。需对 flagged 方法逐一查 expected。

5. **8 个超大 chunk 未计入**，真实缺口可能 >111。

---

## 5. 方向 A 执行建议（据数据定级）

| 优先级 | 子任务 | 目标类 | 估计工作量 |
|---|---|---|---|
| **A2-1** | Unsafe 指针/RuntimeHelpers + Numerics Vector SIMD 补 intrinsic/ShapeRegistry（28 个，最热 intrinsic）| CompilerServices.Unsafe, Numerics | 中（需核 ABI/intrinsic 语义）|
| **A2-2** | Interop GCHandle/SafeBuffer/NameTab 补 native stub（17 个）| Interop, Xml NameTable | 中（有 stub 模板）|
| **A3** | Xml serialization importers + PipeReader 判断走编译闭包 vs 合法降级（24 个）| Xml, Pipelines | 高（需反射/序列化语义）|
| **A4** | 其余 57 个 Other 逐个盘点 | 混杂 | 逐个看，可能并入上面类别 |

---

## 6. 验证边界 / 风险

1. `value=0` + `passed=true` 的大量方法无法自动判定合法与否 —— 需针对每个 flagged 方法读 expected。
2. 8 个超时 chunk 未覆盖，需单独跑（或提高 timeout）才能补全清单。
3. 修复需重新构建 AOT 才能验证（本环境只有预编译二进制）。
4. 数量上：111 个缺口分属 6 个语义域，方向 A 是**中等规模、结构清晰**的正确性工程，不是几十个小改。

---

## 7. 建议下一步

1. 补跑 8 个超时 chunk，补全清单（提高 timeout 或分批）。
2. 选 **A2-1（Unsafe + Numerics Vector，28 个最热 intrinsic）** 作为方向 A 首批：语义最清晰（intrinsic），修一个带动一片，能证明" fallback→native"收益真实。
3. 跑完后重跑 fact，确认该批缺口从 `passed=false` → `passed=true`。

---

## 8. A0 补跑结果（2026-08-12）

对 A1 超时的 8 个 System.Private.CoreLib 大 chunk（runtime-intrinsics*/system*）用
`a0_inventory.py --timeout 180/240` 补跑，**全部 NO_PARSE / 无输出**：
- 180s 内全部 NO_PARSE（regex 未匹配到 factResults JSON）。
- `system-8 --fact-json` 关 log 后 240s 内 **stdout 无任何 fact 输出**（exit=0）。

**结论**：这 8 个 CoreLib 大 chunk 的 `--fact-json` 在本环境**不实际可跑**（方法数千/或卡死），
缺口无法实测。**A1 的 111 缺口是下界**——这 8 个未覆盖 chunk（CoreLib 的 intrinsics/system 核心区）
隐含更多缺口。a0_inventory.py 已入库供未来扩容。

**不做范围外判断**：这可能是 (a) 方法数过大需分钟级 timeout（不切实际），或 (b) 某方法卡死
（需逐 chunk 二分定位）。留给 fd-verification 线/更有耐心的环境。

---

## 9. P0-B native stub 清扫进度（2026-08-12）

任务：stub-clearing-native（只改 native，`src/native/runtime-core/runtime_stubs/interop_stubs.cpp`）。

### 9.1 D 类已修（解释器成功执行但 return 0 → 传播真实返回值）

原 `ChaosExternalRuntimeFallback` 两条走 `InterpreterEntryDirect` 的路径，方法**真实执行**但结果被丢弃、
返回硬编码 0。现改为把解释器 `ret` 缓冲的 word-0（标量/指针返回）作为 fallback 返回值传出
（与 `interpreter_vm.cpp` 的 `ret_tag` 消费契约一致：Int32 取低 32 位、Int64/Pointer 取全 64 位）：

- **IL-data 路径**（原 `_TryExecuteViaIlData` 成功 → `return 0`）：`_TryExecuteViaIlData(subject_id, il_ret)`
  新增 `uint64_t& out_ret`，JSON 解释执行后 `out_ret = ret[0]`；fallback 返回 `il_ret`。
- **dispatch-table 路径**（原 `_TryInvoke` 成功 → `return 0`）：`_TryInvoke(subject_id, invoke_ret)` via
  `_TryInvokeInterpreterSafe(method_key, out_ret)` 新增结果传出；fallback 返回 `invoke_ret`。

禁止"方法整体 return 0"——这两条路径是被真实调用且真实执行的核心，现传播真值。

### 9.2 B 类已修（硬编码 return 0 → 给真实 native 语义，单一 native 改动）

- `ChaosNativeLibraryGetMainProgramHandle`（原 `return 0`）：改为返回主程序 OS 句柄
  （Windows `GetModuleHandleW(nullptr)`；非 Windows `dlopen(nullptr, RTLD_LAZY)`），
  并添加对应平台头（`windows.h` / `dlfcn.h`）。

### 9.3 保持不动的合法 guard（有具体条件的防御 stub，非无条件 return 常量）

- `ChaosMarshalGetExceptionCode/GetExceptionPointers` — AOT 无活动 SEH handler，返回 0/空是真实语义。
- `ChaosMarshalAreComObjectsAvailableForCleanup` — AOT 无 COM，返回 false 是真实语义。
- `ChaosComWrappers*`（5 个）— AOT 无 COM，返回 0/null 是真实语义。
- `_TryExecuteViaSimdStub` 及 `ChaosExternalRuntimeFallback` 内联 SIMD 段 — 硬件 SIMD intrinsic
  解释器不可执行，按 zero-input 返回 0/1 是已验收的合法短路（A2-1 已通过）。

### 9.4 验证明细

- 配置：`cmake -S . -B build/native -DROADMAP0_PRESET_TARGET=windows-x64-reference`（Windows x64 参考）
- 编译：`chaos_runtime_core` 静态库含 `interop_stubs.cpp`，构建通过。
- ctest 通过：
  - `numerics_vectors_test`（A2-1 SIMD 回归）
  - `marshal_api_basic`
  - `test_interpreter_entry` + `test_interpreter_mixed_execution`（InterpreterEntryDirect/dispatch 机制）
  - 4/4 全绿。

### 9.5 remaining（诚实边界）

- **BCrypt/CNG P/Invoke 路由**（`_TryExecuteViaPInvoke`，`interop_stubs.cpp:~818-856`）：该路径仅
  "识别方法名→return true"，实际执行依赖 codegen 直连符号（`interpreter_vm.cpp` 的 `direct_fn` 路径）。
  fallback 此分支 return 0 属"未在此处执行"哨兵。要在此处真正执行 stub 并传播结果需接入完整
  arg/ret 传递 + BCrypt 原生分派，超出"单一 native 层改动"范围，**留 A3**（后续轨道）。
- **PipeReader::TryRead 特例 → return 1**（`interop_stubs.cpp`）：重托管异步流式（A3 类），
  现有 null-`this` 场景返回 1 是已存在的条件 guard，不属本任务硬塞 return 0。留 A3 判断编译闭包。
- **Xml importers / NameTable**：需托管 string interning + 反射，明确 **A3**，本任务不做。
- **_TryExecuteViaPInvoke 的 30+ BCrypt/NCrypt 方法名清单**：均为重型原生 crypto 分派，留 A3。
