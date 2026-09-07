# Handoff — Hotupdate semantic-change residual (专职 hotupdate agent)

## 交接给谁
chaos-il2cpp hotupdate 专职 agent（dev-il2cpp-hotupdate-expert）。本任务是**热更新架构层**完成项,
超出翻译追踪 scope。请接力。

## 一句话 TL;DR
patch 真正应用了(activePatchCount 0→34),但 `semanticChangedCount` 仍 0,因为 patch 替换 body
只以**裸 IL**(ldc.i4 sentinel)落入 .patchdata,没有 interpreter 可执行的 **AotCoreIr/RegIR 段**——
InterpreterEntryDirect 靠 IR 跑,没 IR 就空跑返回 0(≠0xBEEF sentinel)→ 语义判定不计。

## 已修复并合入 main(别重做)
| commit | 修复 | 效果 |
|--------|------|------|
| `25eaaad0d` | PatchDataExtractor.**ExtractSubjectIndex** 遇首非数字停止 | `.patchdata method_def_count 0→34`, body 0→1177B |
| `6c9c1887f` | hotupdate_chunk **CMake 顺序**: patch-host-arrays.cpp 在 runtime-patchdata 之前链接 | host arrays 生效 |

**MSVC 铁律(已实验证伪"任选")**: `/FORCE:MULTIPLE` 保留**首个编译**的 duplicate(LNK4006)。
Selectany 也无效(实测 sentinel 仍先赢)。所以要 host 数组(真值)赢,必须让 patch-host-arrays.cpp
link 在 runtime-patchdata.cpp(哨兵)之前。

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

## 方案方向(请专职 agent 设计,勿我实现)
A. extractor 为 patch subjects 产出 interpreter 可执行的 IR 段(aoc_core_ir 或 reg_ir),而非裸 IL tiny body
   —— 需理解 interpreter 从 .patchdata 哪段取可执行体。
B. patch_loader/interpreter 支持直接执行 .patchdata body 区的 tiny IL(ldc.i4/ret)。
C. semantic 判定改用 body 存在性 + activePatchCount(而非依赖运行时真执行到 sentinel 值)作为
   "patch 应用且替换体在险"的通过标准 —— 但需防 keep-native 假通过,谨慎。

## 验证入口(本地可复现)
```
cd tests/e2e
CHAOS_FOUNDATION_DLL=$(pwd)/translation CHAOS_BUILD_DLL=$(pwd)/../../artifacts \
PYTHONPATH=$(pwd):$(pwd)/verification \
python -m verification.chunk_pipeline --assembly System.Collections.Immutable \
  --chunk global-ns --stages build,fact,hotupdate --native-config check --verbose
```
artifact 读: `.patchdata` 表头字段(magic PADT,method_def@~4520,body_count=34 body>0,aoc_core_ir_count=0)。
完整体: scriptable 用 python struct 按 v4 header(140B) 解析,见我 session 笔记。

## 测试单测
- verification/tests/test_build_metadata_reconcile.py +ExtractSubjectIndex 模式 2 个(纯 "Subject_17" 与
  "Subject_17_suffix"),共 6 passed。确保修 extractor/planner 不回归这两处。
