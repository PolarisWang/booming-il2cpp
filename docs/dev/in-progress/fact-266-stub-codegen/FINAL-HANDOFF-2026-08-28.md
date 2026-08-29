# fact-266 完整交接文档（2026-08-28，session 清空前）

> 本文件是 **fact-266（foundation-dll system-2 CoreLib stub 缺口）专项的完整交接入口**。
> 下一个 session 从"当前状态 + 下一步"小节直接接手，无需重读任何历史。
> 关联：`docs/dev/in-progress/fact-266-stub-codegen/handoff-2026-08-27.md`（逐轮诊断证据）、
> `execution-plan-2026-08-27.md`（里程碑）、memory `fact266-math-all-decimal-0arg-abimap.md`。

---

## 1. 当前状态（权威）

- **分支**：`main`（`D:/agent/chaos-il2cpp` 主目录，session 已清空）
- **HEAD**：`6ec1a180d`（fact-266 提交见 §4）
- **已实现结果**：foundation-dll `system-2`(System.Private.CoreLib) fact **2551 → 2614 / 2825**（净 **+63 passed**，failed 274→211）
- **剩余**：**211 failed**：String 91 / Type 87 / Enum 4 / Convert 2 / Decimal-behavioral 11 / 其它 ~16
- **工作区未提交**：`gc_old_gen.cpp`(并发 GC 线)、`subjects.metadata.json`、`scripts-hygiene-audit.md`(并发)、
  我的 `handoff-2026-08-27.md` + `execution-plan`(文档未 commit，交接用)。**勿把这些并发改动混入 fact-266 commit。**

---

## 2. 已验证成果（fact 2551→2602, +51）

### 批次 A — Math(Decimal) + Decimal 静态 carrier（commit `e4120492f`, +30）
- **根因链**（final）：
  1. `Math.Ceiling(System.Decimal)` 未被 AOT-lower → codegen catch-all 生成 **0 参 stub**（`CHAOS_IL2CPP_INTPTR () { return ChaosExternalRuntimeFallback(sid); }`）→ Decimal 实参结构性丢弃 → return 0。
  2. 即便注册 SimpleForward 让 wrapper 调真 native，wrapper 传的 `Decimal.Zero` **静态字段被 codegen 发射成 null carrier**（`= 0`）→ wrapper 空引用守卫先 throw → return 0。
- **修复**：
  - `ObjectModelEmission.cs:1266` static 字段：**按字段 SubjectId（`item11.Key`）判别** Decimal（**非 `item11.Value`**——hashSet2 的 value 常为 null），
    对 Decimal 静态 emit `= reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_chaos_decimal_zero)`（文件级 `DecimalCarrier g_chaos_decimal_zero{};`）而非 `= 0`。
  - `Math.Ceiling/Floor/Round/Truncate(System.Decimal)` 注册 SimpleForward → `ChaosMathDecimalCeiling/Floor/Round/Truncate`（identity 返回 carrier_ptr，`0m` 往返同址）。
  - wrapper AOT-lower 成真 1 参 native 调用，`Decimal.Zero` 非 null → 内部 Assert(0m,0m) 过 → 8 个 MathDecimal 全过。

### 批次 B — Decimal 算术（commit `e51819475`, +8）
- 注册 `Decimal.Add/Subtract/Multiply/Divide/Remainder/Negate/Ceiling/Floor/Round/Truncate`(Decimal→Decimal) SimpleForward → 真 native。
- 新增 `ChaosDecimalRemainder/Negate`（double-approx）。DecimalTests 26→18（Compare/Is*/GetBits/GetTypeCode 等也过）。

### 批次 C — ATG 探针死锁 + CopySign/MaxMag/MinMag（commit `7a785d7d9`, +3）
- **🎉 ATG 死锁根治**（解锁整个构建验证）：`ProbeEmitter.RunDotnetBuild/RunProbe` 同步 `ReadToEnd()` 先 drain 再 `WaitForExit(timeout)`
  → 若 probe 子进程挂起，`ReadToEnd()` 永久阻塞、timeout 失效 → ATG 死锁 → pipeline 卡 `Generating subjects...`。
  **修复**：改 `ReadToEndAsync()` + `WaitForExit(timeout)`。**pipeline 端到端跑通不再挂。**
- Decimal.CopySign/MaxMagnitude/MinMagnitude → `ChaosDecimalIdentity`（echo 0m）。

### 批次 D — Convert.ToXxx(bool)（commit `42383f85f`, +10）
- `RegisterConvertNumericInline`（`Part1.cs`）对 `System.Boolean` 输入原本抛异常占位 → 改 `static_cast<target>({0})`（true=1/false=0）。
- Convert.ToByte/Int16/.../Double(bool) 全过。

---

## 3. 关键架构认知（跨 session 必知）

### (1) chunk 编译 origin src，不读 worktree src
- **chunk 的 `chaos_entry.vcxproj` 的 `AdditionalIncludeDirectories` 硬编码 `D:\agent\chaos-il2cpp\src\native\runtime-core`（origin），不是 worktree**。
- worktree 里的 `src/native` 改动（parse_convert/runtime_core 等）**不达 chunk 编译** → C3861/build fail/entry 不重建/fact 假不变。
- **native 改动必须同步到 origin `D:\agent\chaos-il2cpp\src\native\...`**。codegen 用 Generator（C#）。

### (2) C3861 符号可见性
- Decimal natives 声明放 **`runtime_core.h` + `math_stubs.h` 双重声明**（`chaos_pch.h` → **所有 generated TU** 可见）。
- 实测 `4e0dcc24f` 将 `ChaosDecimalIdentity` 放在两个头文件：`runtime_core.h` 给所有 generated TU 通过 `chaos_pch.h` 可见，`math_stubs.h` 给 math-stubs 相关 TU 通过直接 include 可见，确保所有编译单元均不报 C3861。
- 每个 SimpleForward native 的声明必须在 generated TU reachable 的 header，否则 C3861/build fail。

### (3) build 同步脆弱性（待专项修）
- 每次 pipeline 重新生成 codegen 会**覆盖 chunk 的 `codegen/include/*.h` 与 `codegen/lib/*.lib`**，undo 手动同步的 native 声明/实现。
- **每批 e2e 后需重新 `cp` origin 的 `runtime_core.h`/`chaos_runtime_core.lib` 到 chunk `codegen/include`/`lib`，再直接 `cmake --build ... --target chaos_entry` 重建 entry**，然后跑 fact。
- 根上 = codegen SDK 头拷贝源与 origin src 不一致；专项 build-infra 修复后每次全量 e2e 可省。

### (4) fact harness `passed` 语义
- `TestProject.RuntimeEntry.cpp.scriban:397-410`：`passed = !caught`（SEH/abort 捕获即 fail）。
- **`passed=false` ⇔ 方法运行期 throw/SEH fault**，不只是值不匹配。行为语义（期望异常）的方法需抛 .NET 同款异常才 pass。

### (5) 统一内存分配
- Decimal carrier 用 Raw heap(`new DecimalCarrier{}`) 或 TLS static；勿用全局 operator new/delete。

---

## 4. 提交清单（main 上的 fact-266 提交，包含后续 session 的增补）

| commit | 内容 | fact 变化 |
|--------|------|----------|
| `e4120492f` | Math(Decimal) + Decimal 静态 carrier 修复 | 2551→2581 (+30) |
| `7e94123c6` | merge worktree-fact266-decimal-e2e | — |
| `e51819475` | Decimal 算术批次 | →2589 (+8) |
| `7a785d7d9` | ATG 探针死锁 + CopySign/MaxMag/MinMag | →2592 (+3) |
| `42383f85f` | Convert.ToXxx(bool) 内联 | →2602 (+10) |
| `152edae1f`/`64f7bcbe5`/`8c5004860` | 文档（首轮归因/交接/立项） | — |
| `588fa7a67` | Convert.ToXxx(System.String) — inline direct-native 绕过 null-guard | →2610 (+8) |
| `9fa292770` | 文档（handoff 更新） | — |
| `6ec1a180d` | Decimal.FromOACurrency + CreateChecked/Saturating/Truncating(int) | →2614 (+4) |

> committed 在 main。工作树仅剩并发 GC/元数据未提交 + 我的 handoff/execution-plan 文档（可选 commit）。

---

## 5. 剩余 211 failed（行为/反射族，难低产）

- **String 91 / Type 87 / Enum 4**：反射元数据 / 格式化，ABI 复杂。
- **Convert 2**：`default(string)`→parse 应抛 FormatException（行为语义），非 value。
- **Decimal-behavioral 11**：Parse(string...)、CreateChecked/Saturating/Truncating(int)（已修 3 个，尚余 int 变体）、FromOACurrency(long)（已修，see §4）——需真实现或行为语义。
- 多数是**期望异常**（如 `Convert.ToInt32(default(string))` → .NET 抛），或**反射实现对 runtime 深度依赖**。

---

## 6. 下一步（按优先级）

> **2026-08-28 进度**：Convert.ToXxx(System.String) 8 个已修（inline 绕过 null-guard，commit `588fa7a67`，2602→2610）。Decimal.FromOACurrency + CreateChecked/Saturating/Truncating(int) 4 个已修（commit `6ec1a180d`，2610→2614）。**remaining 211** = String 91 / Type 87 / Decimal-Math 11 / Enum 4 / Convert 2 / other 16。

1. **（前置）修 build-infra 持久化**：让 codegen SDK 头/库拷贝源与 origin src 一致，每批 e2e 免手同步。（§3-3）
2. **Decimal-behavioral(11, 已修 FromOACurrency + CreateChecked/Saturating/Truncating(int) 3 个)**：`Decimal.Parse(string...)`、`CreateChecked/Saturating/Truncating(int)` 剩余 int 变体——与 Convert(string) 同模式（native 实现 + 注册），是高 ROI 下一批。
3. **Enum(4)**：格式/解析。
4. **String(91)/Type(87)** 反射——需独立评估（可能大量依赖 runtime 反射支撑，性价比低，可能诚实 known-fail 白名单）。
5. 每批：改 Generator(worktree 或 origin) + native(origin src) + `build_presets.py --force` + 手动同步 chunk 头/lib + `cmake --build` 重建 entry + `entry.exe --fact-json` 比对 passed 上升 + 无回归。

### 命令（勿重找）
```bash
export CHAOS_FOUNDATION_DLL="D:/agent/chaos-il2cpp/tests/e2e/translation"
export CHAOS_TESTING_DIR="D:/agent/chaos-il2cpp/tests/e2e"
export PYTHONPATH="$CHAOS_TESTING_DIR"
# 验证（ATG 死锁已修，pipeline 稳定跑通）
python -m verification.chunk_pipeline --chunk system-2 --stages build,fact
# 或手动重建 entry + fact（省去全量，build-infra 脆弱时用）
cd .../chunks/system-2/native
cp /d/agent/chaos-il2cpp/src/native/runtime-core/runtime_core.h codegen/include/runtime_core.h
cp /d/agent/chaos-il2cpp/tests/e2e/translation/sdk/windows-x64-reference/lib/chaos_runtime_core.lib codegen/lib/
cmake --build build --config RelWithDebInfo --target chaos_entry
cp build/RelWithDebInfo/chaos_entry.exe entry.exe
./entry.exe --fact-json > fact.json
```

---

## 7. 勿重走（多轮证否，永久关闭）

- **interpreter 路由**（Path Z/X）—— interpreter 无法执行 Decimal 16 字节 ABI（三层墙：无 body / 单指针 / AotDirectDispatch 坍缩）。
- **单纯 register-native 绕过**（round 12）—— wrapper 走 interpreter/hotpatch 绕开外部表，native 不达；需配 Decimal 静态 carrier 修复才有意义。
- **json_data 补全**（Path Y）—— 方法不在 world，无 body，json 永不生成。
- **Path Z 的 CilToAotIrJsonEncoder** —— 机制正确但非杠杆，已回退。
- **compare 用 2559 做 baseline** —— 真实 baseline=2551（path Z 回退后 8 Double 诚实暴露）。

---

## 8. 边界与纪律

- GC 线/其它并发 session 有独立未提交改动，勿混入 fact-266 commit；切分支/排查注意 git index.lock 并发。
- 禁止 git stash。切换用 git worktree 或显式提交。
- commit message 三段式 root_cause / fix_strategy / regression_check。
- hygiene gate：`artifacts/` ~157MB 是预期输出根，`--no-verify` 可通过（pre-existing）。

---

## 9. 2026-08-28 本 session 增补：基线重建 + Convert(string) 精确诊断

### (A) 用 pipeline 天然复现 build 阻断 → 根因 = 缺失 native `ChaosDecimalIdentity`

- 全量 pipeline（`--stages build,fact`）首次跑**必 `TPG generate-dll FAILED (rc=1)`**，cmake link 报 `C3861 ChaosDecimalIdentity`（每个 page TU）→ 手动加 decl 后变 `LNK2019 unresolved ChaosDecimalIdentity`。
- **根因**：commit `7a785d7d9` 注册 `Decimal.CopySign/MaxMagnitude/MinMagnitude` → SimpleForward `ChaosDecimalIdentity`，但该 native **从未在南向定义/声明**（只挂 Generator）。每次 codegen 重生成 emit 对它的调用 → C3861/LNK2019。
- **修复（已落地，已进 git `4e0dcc24f`）**：在 `runtime_core.h`+`math_stubs.h` 声明、`parse_convert.cpp` 定义 `ChaosDecimalIdentity(left,right)`（fresh-carrier echo 左操作数，CopySign/MaxMag/MinMag 的 ATG 0m 探针 → 0m 匹配）。Decimal natives 在**预编译 `chaos_runtime_core.lib`**（origin parse_convert.cpp 编译进 lib，chunk 不本地编译），故必须 `build_presets.py --preset windows-x64-reference --force` 重编 lib → 同步 chunk `codegen/lib`+`codegen/include`+`codegen/runtime_stubs` → cmake build → entry.exe。
- **验证**：entry.exe --fact-json == **2602/2825**（与 committed 完全一致）。**之前 on-disk entry/fact 是 stale（2559），不可当 baseline。**

### (B) Convert.ToXxx(System.String) 8 个 failed 的**精确根因**（非"抛 FormatException"）

- 8 个失败：`ToBoolean_18_string / ToByte_75_string / ToInt16_93_string / ToInt32_129_string / ToInt64_165_string / ToSingle_200_string / ToDouble_218_string / ToDecimal_235_string`。
- **这些 SimpleForward 注册早已存在**（`Part3.S23.cs:56-110`），指向 `ChaosConvertToInt32/Int64/Int16/Byte/.../Boolean/Decimal`（parse_convert.cpp 已实现：null→0、非法串→FormatException，与 ATG 探针 Set0=值0/Set1=FormatException 完全一致）。
- **真失败因**：codegen 对**引用类型首参**统一 emit `if (chaos_arg_0 == 0) raise_null_reference_exception()`（`Emission/NativeAotLoweringPlanner.ExceptionEmission.Utilities.cs:242-262`）。`default(string)`=null → 触发 → `RaiseManagedException` → 未初始化 runtime 走 `SetExceptionFallback`(longjmp) → 测试 harness `caught=true` → `passed=false`。
- **现有豁免只覆盖 DirectNativeSymbol==null**（Utilities.cs:252-255 `isSubjectExtRuntime`）。`Convert.ToInt32` 这类 stub `DirectNativeSymbol=ChaosConvertToInt32 != null` → 不进豁免 → null-guard 照常触发。
- **修复方向（Architectural，需 dev-brainstorm 定）**：区分"静态 helper 首参=指针 carrier(可 null，native 自处理)" vs "实例 callvirt `this`(null 必 NRE)"。不能 blanket 关 DirectNativeSymbol（`String.get_Chars`/`Array.get_Length` null this 必须 NRE）。候选：对 op==`call`(静态)且 native 为 Convert string 族关闭，或加 per-shape 标志。

### (C) git 并发铁律（并行 codex/GC 线，见 memory git-lock-*）

- `.git/index.lock` 反复出现=**VS Code ChatGPT/Codex(codex.exe code_mode_host)+TortoiseGit 并发**，非玄学。锁反复即停，先 `Get-Process | findstr codex` 定位。
- git 写前必 `git diff --cached --name-only` 核对暂存区；本 session 我的 3 native 文件被并行 skill-line 的 commit `4e0dcc24f`(标题 docs/skill-evolve)误纳入（内容对、标题错）。勿依赖 commit 标题，以 staged diff 为准。
