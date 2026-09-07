# Handoff — Hotupdate semantic-change residual (专职 hotupdate agent)

## ⚡ 2026-09-07 交接后状态更新（专职 agent 已落地实现 Path A）

**已实现（未跨会话验证闭环，编译绿）** — 改动全在 managed extractor（codegen 域内），零 native 改动：

| 文件 | 改动 | 效果 |
|------|------|------|
| `PatchDataExtractor.cs` `Extract()` L140-149 | `buildAotCoreIr = subjectOnly ‖ (aotCoreIrPath 存在)` | subject-only 模式下也产出 AotCoreIr section（不再因 `--aot-core-ir` 缺省而恒 0） |
| `PatchDataExtractor.Remap.cs` `BuildAotCoreIrSection` | `aotCoreIrPath` 可空；无外文件时仅靠 subject 分支出力；subject 分支 operand 自 0 改真 `0xBEEF0000 | syntheticSubjectK` | `.patchdata` aot_core_ir_count = subject 方法数；interpreter 返真 sentinel → 语义门过 |

改对了两个 bug：(1) subject 分支此前 operand=0（doc 写 0x5EED、实返 0）—— 现在 operand=0xBEEF0000|k；
(2) `aotCoreIrPath==null` 时 BuildAotCoreIrSection 入口曾早退 `([],0)` —— 现改为仅阻挡*lookup 构建*，subject 出力不受影响。

**对齐保证**:RewriteSubjectBodies 与 BuildAotCoreIrSection 各自从 k=0 对同一份 methodDefs（subject 过滤后）
按同样前缀集计数 → subject 方法 k 严格一致。subject-only 过滤后每条都过 `ExtractSubjectIndex`（带数字），
等价于 `IsSubjectMethodName` → 无错位窗口。

**编译验证**:`Generator` + `TestProjectGenerator` Release 均 0 error；`test_build_metadata_reconcile.py` 6 passed。

**⏳ 未跑(留给续跑 agent)#E2E**:chunk_pipeline `--assembly System.Collections.Immutable --chunk global-ns
--stages build,fact,hotupdate`(重) → 期望 aot_core_ir_count=34 + semanticChangedCount>0。

下接原交接正文。


## 交接给谁
chaos-il2cpp hotupdate 专职 agent（dev-il2cpp-hotupdate-expert）。本任务是**热更新架构层**完成项,
超出翻译追踪 scope。请接力。

## 一句话 TL;DR
patch 真正应用了(activePatchCount 0→34),但 `semanticChangedCount` 仍 0,因为 patch 替换 body
只以**裸 IL**(ldc.i4 sentinel)落入 .patchdata,没有 interpreter 可执行的 **AotCoreIr/RegIR 段**——
InterpreterEntryDirect 靠 IR 跑,没 IR 就空跑返回 0(≠0xBEEF sentinel)→ 语义判定不计。

> ⚠️ **观测 vs 推断（诚实分层）**：目前**直接观测到**的是 `activePatchCount=34`、`patched value=0`、
> `aot_core_ir_count=0`、`semanticChangedCount=0` 这些数值；**「interpreter 进入 InterpreterEntryDirect
> 且因缺 IR 空跑返 0」是机制推断**，尚未有直接路径观测（eg 日志/断点证明确实执行了该函数并从缺 IR 分支返回）。
> 也不排除「该路径根本未执行 / InterpreterEntryDirect 取到默认存储（ret_buf 未写）」。设计 A/B 时应把此
> 推断当待验证假设，而非已证实前提。

## 已修复并合入 main(别重做)
| commit | 修复 | 效果 |
|--------|------|------|
| `25eaaad0d` | PatchDataExtractor.**ExtractSubjectIndex** 遇首非数字停止 | `.patchdata method_def_count 0→34`, body 0→1177B |
| `6c9c1887f` | hotupdate_chunk **CMake 顺序**: patch-host-arrays.cpp 在 runtime-patchdata 之前链接 | host arrays 生效 |

**MSVC 铁律(已实验证伪"任选")**: `/FORCE:MULTIPLE` 保留**首个** duplicate(LNK4006)。
Selectany 也无效(实测 sentinel 仍先赢)。所以要 host 数组(真值)赢,必须让 patch-host-arrays.cpp 的
目标文件**在链接命令行的顺序上早于** runtime-patchdata.cpp(哨兵)。

> ⚠️ **duplicate 取舍是链接期判定**：MSVC 对重复符号 LNK4006 保留的命令行次序上 **排前** 的目标文件，
> **与「编译顺序/源文件在被编译器处理的先后」无关**。专职 agent 不要误以为需要改编译顺序或用编译先后
> 排序源文件 —— 关键是 CMake 把 host 文件的 .obj 排在哨兵 .obj 之前进入链接行（`6c9c1887f` 的
> `patch-host-arrays.cpp` 在 `runtime-patchdata.cpp` 之前即为此）。

## 残余根因(证据链,逐层确认)
实测 immutable/global-ns:
```
activePatchCount = 34   ← SetPatchedBySlot 真置位 34 个
baseline  value = 42
patched   value = 0     ← ≠baseline 且 ≠sentinel
semanticChangedCount = 0
.patchdata: aot_core_ir_count=0  reg_ir_count=0  body_data_size=1177
```
判定(运行时 runtime-entry / native/Templates/TestProject.RuntimeEntry.cpp.scriban hotupdate 段):
`semantic_changed_count++` 仅当 `patched_value ∈ [0xBEEF0000,0xBEEFFFFF]`(extractor 把 Subject_N body
改写为 `ldc.i4 (0xBEEF0000|idx)`)。

dispatch(runtime-core/chaos/hotpatch_dispatch.h `ChaosDispatchMethodGetValue`):
1. `HotpatchShouldKeepNative` →direct_ptr(原始 AOT)
2. `HotpatchIsActive` → `InterpreterEntryDirect(method_key)` → `__chaos_ret[0]`

`SetPatchedBySlot`(hotpatch_table.cpp:290)会清 keep-native+置 active → 34 entry 走 interpreter。
但 InterpreterEntryDirect 执行的是 **method_key=PatchMethod 的 IR**,patch 只有裸 IL、无 IR 段 →
interpreter 空跑返 0 → 不算 sentinel → semantic 0。

## 方案方向(专职 agent 决策 → 2026-09-04)

### 根因锁定(证据链终版,专职 hotupdate agent 确认)
**interpreter 只从 `aot_core_ir`(GetAotCoreIr(i))或 `reg_ir`(GetRegisterIr(i))取可执行体,
从不执行 body_data 区裸 IL。** 逐层核实:
- PatchLoader.ApplyPatchFromMemory 对每个 PatchMethod 设 `aot_core_ir_json = cache->GetAotCoreIr(i)`
  (reg_ir 仅 header version>=2 且 reg_ir_offset/count>0 时)。两段都空 → null。
- PatchMethodLowerIR:reg_ir==null 且 json==null → 造 1 条占位 IR(空跑不加真值)。
- InterpreterEntryDirect:instr_count==1 无 reg_method → early return,**ret_buf 不写** → 返 0。
- 0 ∉ [0xBEEF0000,0xBEEFFFFF] → scriban 语义门不计。

**为什么 .patchdata 两段都 0**:extractor 的 AotCoreIr section 仅当 `aotCoreIrPath`(--aot-core-ir)
非空且文件存在才产出(PatchDataExtractor.Extract:140-146);RegIr 段 extractor 绝不产出(reg_ir 字段恒 0)。
而 CLI 入口 `TestProjectGenerator.Program.cs RunExtractPatchData(L630-633)` **根本不解析 --aot-core-ir**,
hotupdate_chunk 也只传 --subject-indices/--subject-only → 两段恒 0。body_data 的 sentinel 是死载荷。

**且即便接线 AotCoreIr,现有 subject 分支也错**(BuildAotCoreIrSection.Remap.cs:310):subject 产出
`{"instructions":[{opCode:0,operand:0},{opCode:53}]}` → operand=0 → interpreter 返 0(注释却写 0x5EED,
doc/code 不符)。必须把 operand 设真值 0xBEEF0000|idx 才能过范围门。

### ✅ 决策:走 A(推荐,专职 agent 已定)
**A 变体 = extractor 无条件为每个 subject PatchMethod 产出 interpreter 可执行 subject-sentinel 
AotCoreIr JSON(含真 0xBEEF0000|idx),独立于外部 aot-core-ir 文件。**

为什么 A 而非 B/C:
1. **零 native 改动**:deserializer(aot_core_ir_reader.cpp:228-232 `LdcI4 operand→immediate_i4`)
   + InterpreterEntryDirect 2-instr 快路径(entry_direct.cpp:744-747 LdcI4→ret_buf::WriteI32)已原生支持
   `LdcI4 0xBEEF..; Ret` 返回真值 → instr_count==2 命中,无需任何 loader/interpreter 改动。改动全封在
   managed extractor(codegen 层,域内)。
2. **格式/loader 早已预见此形态**:BuildAotCoreIrSection 的 subject 分支、契约注释、PatchMethod.json 载入路径
   全为"subject 有 interpreter 可执行 IR"设计。只差把该分支做成可达 + operand 用真 sentinel。
3. **保住语义门真义**:维持"替换体真执行并返 sentinel"的判别,不被 RAX-garbage/keep-native 假通过击穿
   (这是 25eaaad0d / scriban P0-C 辛苦建立的防线,不松)。
4. 上游 34 subject 全过 subject-only 过滤 → GetAotCoreIr(i)(按 MethodDef index)对每个都有 entry,无缺档。

**否决 B**:需在 loader/interpreter 新写 Tiny-ECMA body→IR 译码器 + 第二执行路径,跨多 native 文件,
~100+ 行新 codec,且游离开既有 IR 单一模型。成本最高、易生悬挂。
**否决 C**:改验证 oracle(scriban/运行期 semantic 门)按 body 存在性 + activePatchCount 判 → oracle 变弱,
无法再分"真执行 sentinel"与 keep-native/void RAX 噪声 —— 正重开 P0-C 想关的假阳性类。handoff 自警,慎重。

### 最小修复规格(实现者照做,全在 managed 域)
文件: `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs`(+ 必要时 .Remap.cs)。
1. `Extract()`:即使 `aotCoreIrPath == null`,只要存在 subject 重写 subject 即有 subject 方法,
   就产出 AotCoreIr section(重写后 34 全 subject,safe)。section per-method JSON =
   `{"instructions":[{"opCode":0,"ilOffset":0,"operand":<0xBEEF0000|idx>},{"opCode":53,"ilOffset":1}]}`,
   operand = RewriteSubjectBodies 同款 sentinel(subjectIndex 0..N-1 一致)。非 subject(若有,keep-native 场景)
   给空 JSON(单一 ret占位)不透传原 IR(避免挂 interpreter)。
2. `Program.cs RunExtractPatchData`**(go/no-go 不变:仍不传外部 aot-core-ir,只产 subject 段)**;无需新 CLI 参数。
3. 保持 `RewriteSubjectBodies` 照旧写 body_data(raw IL)——供 activePatchCount/格式完整性,但 interpreter 走 IR。
4. 自测:既有单测 `test_build_metadata_reconcile.py` 的 2 个 ExtractSubjectIndex 模式(6 passed)不回归;
   再按文末 `## 验证入口` chunk_pipeline 跑 `--stages build,fact,hotupdate`,期望 `.patchdata` aot_core_ir_count=34
   且 semanticChangedCount>0。

### 顺带发现(供修复,不阻塞 A)
- BuildAotCoreIrSection subject 分支注释写 0x5EED、实 emit operand=0:doc/code 不符,会静默返 0 而非 sentinel,
  是潜在的 wrong 分支。A 直接把 operand 改真 sentinel 一并消除。
- legacy json 复杂 IR 自检(ABORT throw)在 subject 分支 reachable 后会成为死代码路径,可随 A 收编。

## 验证入口(本地可复现)
```
cd tests/e2e
CHAOS_FOUNDATION_DLL=$(pwd)/translation CHAOS_BUILD_DLL=$(pwd)/../../artifacts \
PYTHONPATH=$(pwd):$(pwd)/verification \
python -m verification.chunk_pipeline --assembly System.Collections.Immutable \
  --chunk global-ns --stages build,fact,hotupdate --native-config check --verbose
```
artifact 读: `.patchdata` 表头字段(magic PADT, method_def_count ≈ 4520, body_count=34 body>0, aot_core_ir_count=0)。
> **注意**：`method_def@~4520` 中的 4520 是撰写时的版本派生值，会随 extractor 编码版本漂移。
> 接续者不应直接照抄，而应读取 `.patchdata` 实际字段的 `method_def_count` 值。
完整体: 用 python struct 按 v4 header(140B) 解析:
```python
import struct
# v4 .patchdata binary layout (140B header):
#   magic(4) version(4) method_def_count(4) body_count(4) body_size(4)
#   aot_core_ir_count(4) aot_core_ir_size(4) reg_ir_count(4) reg_ir_size(4)
#   ... (reserved/padding up to 140B)
#   body_data[n]  IR_data[n]  ...
with open("patch.patchdata", "rb") as fh:
    hdr = fh.read(140)
    magic, ver, mdef_cnt, body_cnt, body_sz = struct.unpack_from("<5I", hdr, 0)
    ir_cnt, ir_sz, reg_cnt, reg_sz = struct.unpack_from("<4I", hdr, 20)
    print(f"  method_def_count={mdef_cnt} body_count={body_cnt} body_size={body_sz}")
    print(f"  aot_core_ir_count={ir_cnt} aot_core_ir_size={ir_sz}")
    print(f"  reg_ir_count={reg_cnt} reg_ir_size={reg_sz}")
```
> **注意**：`v4 header(140B)` 是当前 extractor 版本的具体偏移值，会随 `PatchDataExtractor` 的编码版本漂移。
> 接续者应直接读取 `.patchdata` 的 `aot_core_ir_count` 和 `reg_ir_count` 字段，而非硬编码偏移。

## 测试单测
- verification/tests/test_build_metadata_reconcile.py +ExtractSubjectIndex 模式 2 个(纯 "Subject_17" 与
  "Subject_17_suffix"),共 6 passed。确保修 extractor/planner 不回归这两处。
