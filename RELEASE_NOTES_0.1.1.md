# Release 0.1.0

> Auto-generated release notes for range `v0.1.0..HEAD`.

## Summary

- **Commits**: 93
- **Authors**: PolarisWong,
- **Size**:  137 files changed, 10743 insertions(+), 942 deletions(-)

## Features

### Build

- feat(presets): linux-x64-native preset + toolchain for TSAN CI

### CI

- feat(release): 产品级发布流程 — release.sh + GitHub Release + SBOM + checksums + Dependabot + CodeQL
- feat(gc-ci): Linux x64 GC CI + TSAN per-PR

### CodeGen

- feat(codegen): ReadOnlyCollection<T> 真实对象模型 + string.Join 真遍历 — HelloWorld 完整跑通 exit 0
- feat(publish): Phase 1-4 通用 IL2CPP 发布引擎 — ConvertService + PublishController + App 模式

### Docs

- feat(governance): P0.1 SemVer 版本号 + P0.2 README 修正 + 发布规范检查器

### GC

- feat(gc): LEAF barrier — FORCEINLINE DirtyCard/CardBundleSet
- feat(gc): A3 hard-suspend drive — PalSuspendThread integrated into safepoint
- feat(gc): A3 Hybrid safepoint — global trap + rendezvous + EnterCooperativeMode
- feat(gc): ForbidSuspendThreadHolder — cross-thread hard-suspend mutual exclusion
- feat(gc): pal_suspend.h — hard suspension PAL for A3 Hybrid safepoint
- feat(publish): app 模式用 deterministic GC profile — HelloWorld 完整跑通|PolarisWong

### General

- feat(review-fix): triager 复杂度分级 + complex 子协议接入 dev-brainstorm
- feat(governance): release_bump.sh 脚本+CLAUDE.md 发布规范

### Testing

- feat(b4): 双MLC TFM感知扫描 — net8/net10编译隔离 + benchmark_report修复|PolarisWong
- feat(fact266-followup): 重加 Convert.ChangeType 多值探针 — 真 native box dispatch 覆盖非蒙过|PolarisWong
- feat(fact266-followup): 防线 4 硬阻断 — stub 蒙过高危方法只有 default 探针即拒绝|PolarisWong
- feat(fact266-followup): 防线 4 覆盖门禁 + 防线 3 semantic edge test + ChangeType inline direct native
- feat(fact266-followup): ATG multi-value probe for ChangeType/Enum.TryParse + real Convert.ChangeType + fix generator IsStaticConstructorMethod

## Bug Fixes

### Build

- fix(review): VitalityCheck three-level outcome + sdkPreset dynamic + S23 ChangeType inline native
- fix(publish): revert M2 runtime_stubs 编译进 CMake — 依赖内部头破坏 self-contained
- fix(publish): 重写 publish 命令链 — TPG generate-dll + 真实 SDK lib 覆盖 + BuildService 修复|PolarisWong

### CI

- fix(review): 15 条 review findings 修复 — pal_suspend ARM64 + forbid_suspend 注释 + A3 文档一致性 + ChangeType 重载 + CI 基线
- fix(gc-ci): ASAN per-PR gate (from nightly-only)
- fix(gc-ci): per-PR concurrency stress gate at SCALE=50
- fix(review): 13 findings across 3 domains — scriban cctor short-circuit, parse_convert defensive guards, VitalityCheck WARNING, lock doc, MarkObject preemptive, clean catch, S23 ChangeType inline native
- fix(gc): parse_convert GcAllocateAtomic namespace + Server GC LOH stress + CI scale 30
- fix(gc): replace non-ASCII arrow in HeapForCurrentThread comment (L3 review)

### CodeGen

- fix(publish+convert+codegen): 20+ review findings across 3 domains

### Docs

- fix(review): 6 findings — Activator.CreateInstance stub doc + check_release_governance docstring + pal_sync_win32 timeout budget decay + gc_region orphan TLAB cleanup + gc_young_collector Phase-2 scan step

### GC

- fix(gc): EnterCooperativeMode trap — exclude safepoint owner from rendezvous
- fix(gc): PohAllocate std::mutex → GcSpinLock + preemptive wrap (deadlock)
- fix(gc): gc_runtime_gates.cpp forward decl 加 namespace 前缀 — 修复 LNK2001
- fix(review): 16 findings across 3 domains — ChangeType pointer corruption, List.AsReadOnly throw, GcVitalityCheck AOT skip, Enum.TryParse probe fix, cctor scriban return false
- fix(gc): LOH GCSpinLock (CoreCLR-aligned) replaces std::mutex
- fix(publish): P0 修复 — GC profile 改 kDefault + null args 警告
- fix(gc): LOH Allocate preemptive wrapper for mutex block (CoreCLR aligned)
- fix(gc-gates): B3+B4 — GcRuntimeGates control surface + startup safepoint vitality self-check
- fix(gc+publish): app 模式禁用 BGC + low-memory 监控，消除 safepoint 挂起
- fix(gc): preemptive-wrap mutator+BGC old-gen alloc paths — AP C-aligned
- fix(gc): safepoint park lost-wakeup — re-check loop + APC ack-and-return
- fix(gc): OOM routing, Phase-2 scan, alertable safepoint wait — CoreCLR-aligned

### General

- fix(release): pre-flight 容忍 STATUS.md 自滚时间戳
- fix(publish): P2 — 主 DLL 选对 + stub 残留校验 + manifest 增强
- fix(publish): P1 修复 — 移除 FORCE + 实现 --clean + 输入校验
- fix(scriban): cctor runner else branch return true (no cctors = success, not failure)
- fix(review-fix): v2.1 — 自审修正 8 隐患，加固流程质量防线|PolarisWong

### Testing

- fix(fact266-followup): ChangeType decl in convert_stubs.h + inline direct ChaosConvertChangeType — fact 3480→3481
- fix(fact266-followup): Convert.ChangeType inline direct ChaosConvertChangeType — real IConvertible dispatch bypasses 0-arg catch-all
- fix(fact266-followup): Convert.ChangeType inline return-0 — default probe asserts null
- fix(fact266-followup): revert ChangeType to inline return-0 stub — remove SimpleForward native
- fix(fact266-followup): remove Convert.ChangeType multi-value probes — boxed-value tests break; keep Enum.TryParse probes
- fix(gc-ci): correct young-collector test count 5->6 in comment
- fix(gc): optimize gc_server_stress_test per review + SCALE=50 in CI
- fix(gc): Server GC test SEH 0xc0000005 - heap array init order + thread binding
- fix(fact-266): Nullable GetValueRefOrDefaultRef — echo the byref arg (return its address)
- fix(fact-266): Nullable GetValueRefOrDefaultRef — accept any param count, add System.Nullable prefix
- fix(fact-266): Nullable`1.GetValueRefOrDefaultRef inline prefix — use backtick generic prefix
- fix(fact-266): inline all 11 remaining reflection-family stubs — bypass null-guard + return-0 for runtime-dependent methods
- fix(fact-266): EmitAbiReturnPush — add PushSlotType for all carrier kinds (方案 B)
- fix(fact-266): remove InlineShapeDescriptor.SubjectId — not a property of InlineShapeDescriptor record
- fix(fact-266): BitConverter.GetBytes(Single/Double) inline — bypass ABI Float32 return misinference
- fix(fact-266): BitConverter.GetBytes float/double native array length fix — 4/8
- fix(fact-266): GetBaseException/Random.NextDouble/Activator inline + drop duplicate Join — fact 3516 (no regression)
- fix(fact-266): String.Concat 4-arg — fix dropped {3} in pair-composition inline
- fix(fact-266): BitConverter.GetBytes(float/double) + String.Concat(4-arg) natives+inline — fact 3512→3514

## CI / Build

### CI

- ci(gc): raise Server GC stress SCALE 40->45 (GCSpinLock enabled)
- ci(gc): raise Server GC stress SCALE 30->40 (GCSpinLock enablement)
- ci(gc): restore HeapVerify coverage on young-collector bookkeeping
- ci(gc): extend gc-server-smoke with multi-heap general stress (SCALE=50)
- ci(gc): CHAOS_GC_HeapVerify=2 in CI gate — verify_heap productionized

### Testing

- ci(gc): add gc-server-stress multi-heap test to gc-server-smoke job

## Refactor

### Testing

- refactor(gc): route GcAllocateFast/Profiled through unified Allocate()
- refactor(gc): Allocate() — unified single-region entry point

## Docs

### Docs

- docs(gc-industrialization): STATUS — P1 A3 Hybrid safepoint 骨架完成
- docs(review): 8 doc findings — STATUS batch-1 gate 矛盾消除、a3-safepoint 结论+未决项交叉引用、leaf-barrier mode 交接约束、research 行号精度声明
- docs(gc-industrialization): STATUS — P1 E0 安全重构 (GcBumpCache + POH)
- docs(review): roadmap/design A3 深度定义与 T-B 修正同步 + forbid_deprecate→强化 + A2/A3 档位区分标注
- docs(gc-industrialization): CHECKPOINT-1 — batch-1 收口，等真实 CI 护网绿|PolarisWong
- docs(gc-industrialization): 同步 A3 档语义为 Hybrid，batch-1 收口
- docs(gc-industrialization): batch-1 T-B4/B5 + T-A4/A5/A6 设计/调查定稿|PolarisWong
- docs(review): roadmap A1/A2/A3 深度分级澄清 + DISPATCH 粒度统一 + fact_chunk 防线4注释指向 build.py 真实门禁|PolarisWong
- docs(runtime): HelloWorld app-mode e0000001 托管异常专项交接
- docs(governance): P1.1 协作基础设施 + P1.2 外部贡献者 CONTRIBUTING

## Testing

### Testing

- test(cfix): ReadOnlySpan.ToArray 真 native + ATG IsNotNull 消除
- test(gc): register test_gc_gen1 as cross-test global-state pollution flake
- test(gc): add gc_server_stress_test — multi-heap concurrent pressure (Server GC)
- test(gc): register 3 stress-limit binaries as known-failures

## Chores / Dependencies

### GC

- chore(gc): 保留并行工作流的 GC 未提交改动（发布前快照）

### General

- chore(status): 刷新 hygiene dashboard 时间戳|PolarisWong
- chore(status): 刷新 hygiene dashboard 时间戳|PolarisWong
- chore(status): 更新 STATUS.md 发布前快照

## Other

### Testing

- cleanup(gc): remove GcBumpCache (zombie) + tests
