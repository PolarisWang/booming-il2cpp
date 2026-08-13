# [HotUpdate] 解释器派发缺陷 A2 — 根因诊断交接文档

> **日期**：2026-08-13 ｜ **性质**：根因诊断 + 交接（供 interpreter/hotpatch 专项接手）
> **前置**：A1 `tool_helpers.py` 指纹失效修复（commit `cccb96a8c`）诚实重建后**新暴露**的 5 个主-fact 失败
> **状态**：✅ 根因已诊断到"确定性可复现 + 可交接"深度；❌ 未修复（涉解释器深域，不仓促 patch）
> **声明**：本档为只读分析，代码证据均附 file:line。

---

## 一、一句话结论

A1 诚实重建后，numerics chunk 主-fact **确定性**暴露 5 个 `[HotUpdate]` 标记的 `System.Numerics.BitOperations` test-subject 失败，其 `passed=false` 是**执行时抛异常**（非值错），根因指向**解释器对 `UIntPtr/uint/ulong` 返回 + 特定 method_key 的 arg 编组(marshaling)缺陷**——全 chunk **596 个解释器路由 subject 仅这 5 个失败**，非通用解释器问题，**与 SIMD Equals* 完全独立**。

---

## 二、复现信息（确定性）

**命令**（foundation-dll pipeline，`CHAOS_FOUNDATION_DLL=tests/e2e/translation`）：
```
python -u -m verification.chunk_pipeline --assembly System.Private.CoreLib --chunk numerics --stages build,fact --skip-probe
```
**结果**：`aot: 175/180 passed (partial), jit: 175/180 passed (partial)`。二次重跑结果一致（确定性，非 flaky）。

**5 个失败 subject**（aot、jit 各同一组）：

| si | generatedMethodId | subjectId | 调用体 |
|----|-------------------|-----------|--------|
| 49 | `RotateLeft_23_System_UIntPtr_int_1` | `BitOperations::RotateLeft:System.UIntPtr(System.UIntPtr,System.Int32)` | `RotateLeft(UIntPtr.Zero, 0)` |
| 52 | `RotateRight_24_uint_int_1` | `RotateRight:System.UInt32(UInt32,Int32)` | `RotateRight(0, 0)` |
| 54 | `RotateRight_25_ulong_int_0` | `RotateRight:System.UInt64(UInt64,Int32)` | `RotateRight(0, 0)` |
| 60 | `Crc32C_27_uint_byte_0` | `Crc32C:UInt32(UInt32,Byte)` | `Crc32C(0, 0)` |
| 62 | `Crc32C_27_uint_byte_2` | `Crc32C:UInt32(UInt32,Byte)` | `Crc32C(0, 0)` |

> 注：同方法其他 value-set 变体（`_0`/`_1`/`_2`）**部分 pass 部分 fail**，且调用体完全相同、全部零输入。

---

## 三、硬证据链（根因定位）

1. **`passed=false` 语义 = 执行时抛异常，不是值错**
   - `TestProject.RuntimeEntry.cpp.scriban:473` — `printf("...contractIndex:-1,...passed:%s,...value:%" PRId64, si, v_i, caught?"false":"true", caught?result:result)`
   - `passed = !caught`；`value` 是异常路径返回的 `result=0`。所以失败=**dispatch 时 `try/catch`/`setjmp` 捕获到异常**。

2. **`kind=hotupdate` 来源**（ATG 分类）：`src/tools/.../AutoTestGenerator/Program.cs:391/397`
   - `pr.IsDeterministic && !pr.IsVoid` → `kind="hotupdate"`（非 `fact`）。probe-results 显示这些方法 `hasException:false, returnValue:0`（probe 零输入→0），被正确归为 deterministic-nonvoid → hotupdate。

3. **这些 test-subject 无 AOT C++ body**（纯解释器路由）
   - `chaos_generated_module.cpp:378-386`（仅 extern 声明，无定义）
   - dispatch 表：`native-aot.generated.cpp:8150-8198` → `&InterpreterEntryDirect` 路由
   - flags 完全一致：`kHotpatchKeepNative | HotpatchEncodeArgCount(0)`

4. **非通用解释器问题**：全 chunk `grep -c "InterpreterEntryDirect"` = **596**，仅这 5 个 fail。

5. **诡异的变体分叉**：同一方法 `_0/_1/_2` 的 body（零输入）与 dispatch flags 字节相同，仅 index 不同 → 有 pass 有 fail → **索引/编组相关的确定性派发缺陷**，非语义差异。

---

## 四、根因假设（待专项验证到可修）

- **H1（主）**：`InterpreterEntryDirect(method_key, args, ret)` 对返回 `UIntPtr/uint/ulong` 的取回编组（`ret_buf` 布局）或对特定 `method_key`（`UIntPtr` native-int type）的解释器解析抛异常。
- **H2**：`HotpatchShouldKeepNative` 为真时 `DispatchDirectVoid(entry.direct_ptr)` 调用了指向 **body-less 符号**的指针（`kHotpatchKeepNative` 被设上但 body 未生成），触发未定义行为/异常。
- **H3**：ATG `hotupdate` 变体与 `HotpatchEncodeArgCount(0)` 交互，编组 0 参数但底层调用有实参。

**建议切入**：`src/native/runtime-core/chaos/hotpatch_dispatch.h:92-116`（KeepNative→direct_ptr 分支）+ `src/native/interpreter/interpreter_entry.cpp`（InterpreterEntryDirect 实现）。对比 **596 个 pass 的** subject 与 5 个 fail 的，找编组差异（尤其 `UIntPtr` 类型）。

---

## 五、为什么不仓促修

- 影响面风险：只有 5/596 失败，改动解释器/hotpatch 编组可能波及其他 591 个正常解释器 subject。
- 跨 **解释器 × hotpatch flag × ATG kind 分类** 多域，需要专项专注追踪（非 P0-B codegen 语境）。
- 避免"修错一处、引入更大回归"。

---

## 六、建议下一步

1. **合并本文档到 A2 专项**：`docs/dev/in-progress/hotupdate-interpreter-a2/`（含 STATUS.md）。
2. 专项先做 H1 验证：给 `InterpreterEntryDirect` 加临时日志定位抛异常点在编组 vs 解释执行。
3. 修复后回归：重跑上述 build,fact，期望 `aot/jit: 180/180` 且其余 596 subject 不受影响。

## 七、关联

- A1 根因/落地：memory `p0b-stub-sweep-classification.md`（A1 段 + A2 段）
- 本 bug 是 A1 诚实重建**暴露**的既有缺陷（非 A1 引入），见同一 memory。
