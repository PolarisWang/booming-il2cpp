# GC 整体修复 Session 交接文档（2026-08-28 清空前）

> 状态：GC P0 批量修复全部完成并已 cherry-pick 进 main；整 session 工作已固化。
> 目的：清空会话前，供下一 session / 用户无缝续跑。

---

## 一、本 session 交付盘点（已全部提交并进 main）

### 1.1 四轮 GC P0 稳定失败修复（本次 session 核心，全部 cherry-pick 进 main）

| main 上 hash | 内容 | 根因 | 验证 |
|------|------|------|------|
| `a97e0fd34` | **P0#1** max_promote `MultipleYoungGcsWithTimeout` SEH | GC-N7 类 Phase-2 out-of-bounds scan：`ReadStableId(first_word)` dang TypeInfo deref @ `gc_young_collector.cpp:564`，scan_ptr 飞出 region 到 unmapped（cdb 栈 `GcYoungCollection+0x954`、`db rax` 全 `??`）。修=Phase-2 全程 confine 到 `[region->begin, region->end)`（clamp + loop break + interior-slot/advance clamp） | max_promote **4/4 PASS 稳定** |
| `7d0f9ac89` | **P0#2** gen1 `SingleLiveObject` | 惰性二次 `InitYoungGeneration`：`TlabClaimFromYoungGen` 在首次 NurseryAllocate 重跑（fixture 直调 InitYoungGeneration 不设私有 `s_young_gen_state`）→ 重分配 nursery+gen1 → orphan 之前 `TryAllocateInGen1` 对象。修=幂等 lazy-init（region!=null 即 ready 不重跑） | gen1 **14/14 PASS** |
| `4813df131` | **P0#3** `ConservativeSweepSelfRefs` | **GC 正确、测试断言不可观测**：Phase-0 重写调用线程栈局部到 tenured → 原始 `objB_fw & 1` 断言读 tenured 副本非 forwarding tag。修=断言真实契约（objA/objB 晋升出 nursery + objA->ptr@8 修到 tenured objB + payload 完好） | 隔离 5/5 PASS |
| `f6cbe9d90` | **P0#4** A2b untyped | **verify 假阳性**：`GcVerifyHeap` first-word 检查把 interior 槽 + 合法 untyped/raw old-gen 分配（`Allocate(size,true)`）误当损坏。修=skip interior + 接受 untyped | demotion/old_gen HeapVerify=2 **0 hard ERROR** |
| `47822c64d` | 可信度闭环 step1 | 移除 stale bgc_root_scan known-failure（现 0.22s 4/4 PASS） | baseline reconcile |
| `ec0f71007` | **P2 文档漂移** | 校正 24-CRAG "已知差距全部已解决"过度声明 + STATUS P0 复核 | docs |

### 1.2 更早 session 的 GC 提交（已在此 main 之前）
`8cb5a945d`（YoungCollectionEmpty）、`0adc9c12b`（demotion GcVerify）、`049d6e65c`（region-gen benign + poison gate）、`075603c1a`/`f64e9d8b0`（docs/handoff）。

---

## 二、git 状态（session 结束瞬间，实测）

- **当前主工作区分支：`main`**（HEAD `ec0f71007`，含全部 GC 提交）。已从并行轨道游离分支 `feat/skill-trigger-chain-fix` 切回。
- **main 的 GC 提交**：`a97e0fd34`→`7d0f9ac89`→`4813df131`→`f6cbe9d90`→`47822c64d`→`ec0f71007`（依赖顺序）。
- 注意：这些是 **cherry-pick 副本**（新 hash），不是原 feature 分支上的原始提交（原 hash 786c3fcb8 等仍在 `feat/skill-trigger-chain-fix` 分支）。
- **worktree**：主工作区 `[main]` + 一个遗留 `wf_86236b2e-f25-3`（并行轨道，有未提交 CMake/unsafe_stubs 改动，勿删）。之前占 main 的 `fact266-merge-main` worktree 已被清理。
- **主工作区未提交改动（6 个，属并行轨道 codegen/parse_convert，非 GC）**：`docs/.../repo-cleanliness/STATUS.md`、`fact-266-stub-codegen/handoff`、`NativeAotLoweringPlanner.*.cs`、`CoreStubs.*.cs`、`gc_old_gen.cpp`、`parse_convert.*`、`runtime_stubs/math_stubs.h`、`runtime_core.h`、`ProbeEmitter.cs`、`subjects.metadata.json` 等——**勿动，属并行轨道**。

---

## 三、未完成 / 交接项（下一 session 选做）

按 review `gc-legacy-plan-review-2026-08-27.md` 的计划序：

### 继承1（P0#3 full-suite flaky）— 已深挖已定论，交接
- **结论**：`ConservativeSweepSelfRefs`/`CollectionWithDirtyCard` 完整套件 ~40-60% flake（隔离稳定）是 **GC-side young-collection 在全局态累积下的稳定性 bug**（多个测试体 `GcYoungCollection()` 偶发 SEH 0xC0000005），与 GC-N7 `YoungGcPauseUnderLoad` 同源，**需真机 page-heap**（memory `gc-n7-release-benchmark-crash-2026-08-25.md`，A/B/C 已 revert）。
- **测试无法修**（fixture bump reset 无效）。已还原测试到 `4813df131` 状态。

### 继承2（P2 文档漂移）— 已提交 `ec0f71007` ✅
- 24-CRAG + STATUS.md 校正完成。

### 继承3（P0#5 全量闭环）— 部分验证
- **已验证**：`build/native` ctest 8 个核心 GC 套件 100% pass；`contracts-native-gc` group（suite_contract.yaml:52）已配好。
- **待做**：全量 72 GC 测试 preset build + driver 产可复现 `test-report.json` + CI 接线。

### 继承4（region->gen mismatch）— 未做
- full_gc HeapVerify=2 残留 6 `region->gen mismatch`（跨池 clobber 遗漏），可能真 state bug。

### full_gc bitmap-poison 已定性（非 bug）
- 47 `bitmap poison clobbered` = `GcMarkBitmap::Clear()`（gc_mark_bitmap.h:91-95）`memset(byte_count_)` 清零 poison 的 **verify 假阳性**，非 OOB。修法若追=Clear() 跳过 poison。

---

## 四、本轮已定性的复核结论（memory 已记录）

- `gc-pp-zero-batch-fixes-2026-08-28.md`（P0 全批 + full_gc poison 定性 + P0#3 flaky 结论）。
- `gcverify-demotion-and-regiongen-clobber-resolution.md`（verify 可靠化）。
- `youngcollectionempty-fixed-conservativesweep-preexisting.md`。

---

## 五、给下一 session 的注意

1. **并行轨道活跃**：`feat/skill-trigger-chain-fix`/`feat/skill-token-optim-a-b` 分支 + 主工作区未提交 codegen/parse_convert 改动，GC 源码改动前先核对分支，避免 commit 落错（本次已用 cherry-pick 补救）。
2. **git index.lock 频繁**：并行轨道 git 操作密集产生 0 字节 stale lock。无 git 进程时安全删除（`rm -f .git/index.lock`），但绝不 `git stash`（禁止）。
3. **GC 测试构建**：`build/native` VS generator；cdb 定位 SEH 用 `D:/.../Debuggers/x64/cdb.exe -cf script`.
4. **已知残局优先级**：GC-N7 堆破坏（真机）> P0#5 CI 基建 > region->gen mismatch。

---

## 六、本 session 已提交提交清单（git log 参考）

`ec0f71007` docs P2 漂移
`47822c64d` test gc bgc_root_scan known-fail
`f6cbe9d90` fix GcVerifyHeap A2b
`4813df131` test ConservativeSweepSelfRefs
`7d0f9ac89` fix idempotent lazy-init
`a97e0fd34` fix Phase-2 OOB
（以上 6 个为本 session main 的 GC 提交，均来自 cherry-pick。）

> 交接完成。下一 session 可直接从此状态续跑继承3/4 或 GC-N7。
