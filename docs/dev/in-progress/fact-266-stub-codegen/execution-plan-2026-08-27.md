# fact-266 执行方案（多 session，可落地）

> 目的：把 foundation-dll `system-2`（System.Private.CoreLib）fact 的 **266 个 failed（value=0）** 逐族消灭，最终 passed 从 2559 → 2825。
> 本文是**唯一权威的执行入口**。任何 session 从"下一步"小节接手，无需重读历史。
> 关联诊断已沉淀：`docs/dev/in-progress/fact-266-stub-codegen/handoff-2026-08-27.md`（第 6-10 轮是**已完成证据**，别再重做）。

---

## 0. 铁律（每步必守）
1. **改源码后先跑** `python tests/runner/test_driver.py --layer unit --group codegen`——出现**非 known 的 `[FAIL]`（OVERALL: FAILED）**即真回归，必须修复再推。
2. **e2e 验证**：每次改完重编 chunk + fact，**同一 codegen 配置比对**（见 §2），passed 只许升、不许降、不许引入新 failed/崩溃。
3. **比对基准必须同配置**：不要拿带 Path Z / 旧二进制的 baseline 对比（这会伪造回归——见 handoff 第 8 轮教训）。
4. commit 消息含 `root_cause`/`fix_strategy`/`regression_check`。
5. **禁止 git stash**；切换上下文用 `git worktree`（仓库有现成 22G worktree 教训）。

---

## 1. 环境（已跑通，直接复用）
```bash
# 前置 SDK（首次/跑管道前；新鲜则秒过）
cd D:/agent/chaos-il2cpp/tests/e2e/translation
python artifacts/build_presets.py --preset windows-x64-reference
# 每次跑 chunk（build+fact 约 8-15 分钟；这是事实唯一的权威验证）
export CHAOS_FOUNDATION_DLL="D:/agent/chaos-il2cpp/tests/e2e/translation"
export CHAOS_TESTING_DIR="D:/agent/chaos-il2cpp/tests/e2e"
export PYTHONPATH="$CHAOS_TESTING_DIR"
python -m verification.chunk_pipeline --chunk system-2 --stages build,fact --verbose
# 成功标志（fact 行）：
#   <<< + [passed] ... -> aot: <passed>/2825
```
**关键**：pipeline 的 AOT entry 是 `native/entry.exe`（不是 `native/build/RelWithDebInfo/chaos_entry.exe`，那个是旧的）。看 `native/entry.exe --fact-json` 才是正确读数；但更稳的是直接看 pipeline 打的 `aot: N/2825`。

---

## 2. 基准与 failed 集（起手先做一次）
1. 干净 build+fact → 记 `aot: 2551/2825`（**2026-08-27 实测**；Path Z 回退后 8 个 `Convert.ToX(Double)` 诚实暴露，比旧记的 2559 少 8，failed=274）。
2. 导出当前 failed 方法清单（methodIndex→`s_hotpatch_entries` 注释→subject）：
   ```python
   # 用 native-aot.generated.cpp 的 s_hotpatch_entries[9576] 每行 "}, // NAME" 映射
   # fact failed 集合：native/entry.exe --fact-json > fact.json; 解析 factResults[].methodIndex
   ```
3. **保存这份 baseline failed 集**（含每项 methodIndex/subject），作为后续所有比对锚。
   > 274 failed 按 *Tests 族：Convert 42 / String 91 / Type 76 / Decimal 26 / Math 8 / Enum 13 / 其它 18。

---

## 3. 里程碑（每里程碑 = 可独立验证的一块，跨 session 顺序推进）

### M0（已完成，勿重做）
- Path Z 判否、P0-B 主径确立、e2e 环境、Convert 三阻塞证据 —— handoff 第 6-10 轮。

### M1 [前置，1 个 session]：拿 TPG cmake 真实 LNK
**why**：Convert Decimal→scalar 生成的 C++ **编译全过**，但 TPG `generate-dll` 在 **cmake link entry.dll/exe** 阶段 fail（`[TPG:err] [build] cmake build FAILED`），确切的 LNK 行被 `stages/build.py:1260-1271` 截断（只回显 stdout 尾 5 行）。

**do**：
1. 改 `tests/e2e/verification/stages/build.py`：`tpg_result` 失败时打印**完整 stdout**（不只尾 5 行），或把 cmake/MSBuild 输出重定向到文件再读。
2. 临时加回探针注册（`Convert.ToInt32(System.Decimal)` SimpleForward→`ChaosDecimalToInt32`，注意后者 native 已存在于 parse_convert.cpp/convert_stubs.h）。
3. `--stages build` → 读完整日志，拿到那个真实的 `LNK????` / `error`。
**验收**：看完下述 M2 的 A/B/C 三因，能判定是哪一类。

### M2 [核心，1-2 session]：Convert Decimal→scalar 真修（依据 M1 的真实 LNK）
按 M1 判出的因：
- **A. 符号没进 link set**（`LNK2019 unresolved ChaosDecimalToInt32`）：
  `convert_stubs.h`/parse_convert.cpp 已在 runtime core，需确保 entry 工程 link 了 runtime-core（那 11 个 SDK lib）。核对 TPG 生成的 `chaos_entry.vcxproj`/cmake 是否含 runtime-stubs lib。
- **B. 返回 ABI 不匹配**（`LNK#` 或 codegen 生成的 stub 与 native 签名对不上）：
  stub 体是 `return ChaosDecimalToInt32(arg)` 返回 `CHAOS_IL2CPP_INT32`，但外层 stub 声明 `CHAOS_IL2CPP_INTPTR ... ()`——**注意**：SimpleForward 生成的 stub 若用 `CreateInt32AbiSlot()` 返回，应将 stub 声明为 `INT32`；若外层声明 `INTPTR`，用 `CreateNativeIntAbiSlot()`。核对 RegisterDecimalToScalar/SimpleForward 的 returnAbi 与 native 签名一致。
- **C. 其它**：拿 LNK 行对症。
**验收**：单加 `Convert.ToInt32(Decimal)` 后 `--stages build,fact` → `generate-dll` 从 rc=1→过，fact 不降。

### M3 [1 session]：Convert Decimal→scalar 全族 + 防 Double 回归
- 在 M2 打通单个后，扩到 `ToInt32/Int64/Int16/Byte/SByte/UInt16/UInt32/UInt64(Decimal)`（natives 已写好：`ChaosDecimalToInt32/Int64/Int16/Byte/SByte/UInt16/UInt32/UInt64`，见 parse_convert.cpp + convert_stubs.h，需重新加回）+ `ToBoolean(UInt64)`。
- **关键防回归**：加每个 handler 后，确认 `Convert.ToX(System.Double)`（8 个）仍过。若再变坏，停，回到 M2 的"共享 dispatch 遮蔽"调查（见 M5）。
**验收**：ConvertTests failed 34 → 下降 5（ToByte/Int16/Int32/Int64/Double+Boolean 一批），passed 升，Double 8 个不降。

### M4 [1-2 session]：Object+IFormatProvider 变体（Convert 族另一半）
- `ToByte/Int16/Int32/Int64/SByte/UInt16/UInt32/UInt64(System.Object, System.IFormatProvider)` —— 这些是真实对象转换，需 runtime 支持（`IConvertible` 分发 + culture）。**较难**，单独批次。
- 先只做无 culture 依赖的（用 `IFormatProvider` 时忽略/默认），或注册 `Convert.ToX(Object, IFormatProvider)` → 查对象实际类型再转。
**验收**：下一批 through 后 passed 再升，无新 failed。

### M5 [调查型，机动]：codegen 加 handler 为何扰动 `Convert.ToX(Double)`
- 若 M3 仍现 Double 回归（或想彻底根治），做一次 codegen-internal 调查：加单个 handler → 重编 codegen → 对比 `Convert.ToX(Double)` 的 `_externalRuntimeSubjects` 槽 / hotpatch method_key / interpreter `_TryInvoke` 可达性。
- 这可能是**共享 dispatch bug** 的根，或需要让 Convert 族走统一（不 shadow）的机制。

### M6+ [后续族]：String（91）/Type（76）/Decimal（26）/Math（8）+ 其它（31）
- 每族按同模式：识别 failed methodIndex→subject → 写 native/注册/interpreter 特判 → build+fact 比对 baseline → passed 升 + 无回归。

> ⚠️ **2026-08-27 首轮实证（修正 M6+ 前提）**：**Math(8) 不是"纯算术最易"** —— 全部 8 个 failed 是 **Decimal overloads**（`Math.Ceiling/Floor/Round/Truncate(System.Decimal)`）。
> A/B 对照（同文件相邻 wrapper 体）：
> - `Math.Ceiling(double)` → AOT-lower 成真 native `ChaosMathCeiling(arr)` **传参** → **通过**。
> - `Math.Ceiling(Decimal)` → codegen 生成 **0 参** `chaos_external_runtime_..._Math__Ceiling_System_Decimal_()` stub → **Decimal 参数结构性丢弃** → return 0 → failed。
> **根因 = codegen 对 Decimal(16字节 ValueType) 外部方法生成 0-arg stub**（同 Convert-L1 round 8-10 的墙）。Math 与 Convert/Decimal 同壁，不是独立易啃族。
>
> **优先顺序（修正）**：Math 并入转换/Decimal 族（同为 0-arg ABI 墙）→ 先攻"让 codegen 对带复杂 ABI(Decimal/对象) 的外部方法生成真调用"这一 **cross-cutting 根因**，而非逐族手写 stub。String(91)/Type(76)/反射族 ABI 更复杂，放最后单独评估。
>
> ⚠️ **2026-08-27 第 12 轮决定性翻案（SimpleForward 证伪，勿重走）**：补 typed SimpleForward native 让 `Math.X(Decimal)` 走真 native **已实证无效且已回退**。
> 实证：Decimal carrier 实参/返修在 TPG **可编译**（无 C3861/LNK，round-10 的 rc=1 实为 Convert-family 遮蔽），但 fact **一行不升**
> （MathTests decimal 8 个仍 failed, value=0）。因：ATG wrapper 经 **interpreter/hotpatch direct dispatch** 执行，整个调用链**绕开
> external-runtime SimpleForward 表** —— 我注册的 native 只挂在外部 stub 定义，从未在事实执行路径被调用。**register-native 这条路三次证伪（rounds 8-11 + 12），永久关闭。**
> **唯一残余真方向 = 让 interpreter/hotpatch 收到真实 Decimal 实参**（`ChaosExternalRuntimeFallback` 接 args buffer + codegen 传参），或 AOT-lower 被测 Decimal 方法本身。跨域专项，见 handoff round 12。
>
> 🔴 **2026-08-27 第 13 轮（interpreter 可行性专项）决定性结论**：interpreter **无法**执行 `Math::Ceiling(System.Decimal)`，三层墙闭环：
> (1) 无 body（aot-core-ir 0 定义 + raw-CIL→IR Phase 2 未实现）；(2) 16 字节 ABI 无路（`CacheSignature` Decimal→单 ObjectRef，`ArgBuffer` 无多字读）；(3) wrapper 的 IR `call` 在 `AotDirectDispatch` 里 Decimal 坍缩成单指针、`direct_fn` 落到 0 参 stub → return 0。
> **唯一可行路径 = AOT-lower `Math::Ceiling(System.Decimal)` 使 wrapper 的 `call` 解析到真 native**（复刻 `Math.Ceiling(Double)`→`ChaosMathCeiling(double)`，round-11 A/B）。精确机制（`direct_fn` 为何 Double→ChaosMathCeiling、Decimal→0 参 stub）待 round-14 落实。

---

## 4. 每 session 的收尾（保证交接）
1. `git status` 确认工作区干净 / 无意外改动（GC 线有各自 session 的未提交改动，勿混入本专项）。
2. 手写 natives + 注册若落地，**逐批次 commit**（三规则 root_cause/fix_strategy/regression_check）。
3. 更新 handoff：记"本 session 完成里程碑 / 下一步明确入口 / 命令"。

---

## 5. 风险与注意事项
- **比对基准别混**：Path Z 已回退、勿再用带 json_data 的 baseline（handoff 第 8 轮教训）。
- **e2e 每次 8-15 分钟**：别为一个猜测反复重编；先窄化（单个 handler + 单 batch）再编。
- **TPG cmake 输出被截断**：M1 先修好日志捕获，别在没 LNK 时猜。
- **Convert 的 `Convert.ToX(Double)` 是经 interpreter `_TryInvoke` 通过的**：任何 Convert handler 都可能扰动它——这是 M3 防回归的重点。
- 与 P0-B 111 缺口重叠：`p0b-stub-sweep` 族可能在动，先核对避免撞车。
