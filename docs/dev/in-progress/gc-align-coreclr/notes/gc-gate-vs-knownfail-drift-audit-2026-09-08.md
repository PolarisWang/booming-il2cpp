# M1 GC Gate 真实失败集 vs known-failures 清单脱节审计（2026-09-08）

## 结论（确凿）：known-failures.integration.yaml 与真实 full-GC gate -j4 失败集几乎完全脱节

known-fail yaml 记录基准是 2026-08-28/-09-04 audit，期间 main 合入大量 GC/并行线改动，当前 HEAD（2026-09-08）真实现状已变。文档漂移使 M1 门禁判断失真——这是本次 audit 要固化的核心事实。

## 证据（参考 windows-x64-reference Debug 构建，ctest -R "chaos_gc_|test_gc_" -LE "benchmark|stress|soak" -j4）

### 真实 full gate 稳定失败集（3-5 轮均出现，5-6 项）——全部不在 known-fail yaml
| ctest 项 | 性质 | isolated 状态 |
|----------|------|--------------|
| `chaos_gc_atomic_alloc_test` | SEGFAULT | true（isolated 6/6 SEGFAULT）→ true bug |
| `test_gc_atomic_alloc` | SEGFAULT | true（isolated 崩）→ true bug（GC 域专项处理中） |
| `chaos_gc_fullgc_raw_scanning_object_test` | Failed | pass（isolated rc=0）→ 并行门 flake |
| `chaos_gc_young_collector_test` | Failed | pass（isolated rc=0）→ 并行门 flake（contracts 版） |
| `test_gc_demotion` | Timeout(60s) | isolated >90s 仍超时 → 非并行 flake，需查(真慢/deadlock) |
| `chaos_gc_bgc_root_scan_test` | flaky Timeout | isolated ? |
偶发：`chaos_gc_region_barrier_stress_test`（SEGFAULT，偶发轮出现）

### known-fail yaml 已列但实况（此 HEAD）不再在 gate 失败集
| yaml 条目 | full gate 3+轮 | 判定 |
|-----------|----------------|------|
| `test_gc_loh` | 未失败（isolated 3/3 pass） | 从 gate 失败集消失（可复核摘除） |
| `test_gc_gen1` | 未失败（isolated 3/3 pass） | 从 gate 失败集消失 |
| `test_gc_general_stress`/`loh_stress`/`delegate_stress` | 是 **stress label**，被 `-LE stress` 排除在主 gate 外 | 本属 gc-stress job；放 integration(非stress) known 层错位 |
| `test_gc_young_collector`（本轮已由我摘除） | 已修 save/restore g_heap_base | ✅ 移除正确 |

## Why this matters
- M1 剩余🟡"已知失败维护"指 yaml 里的 loh/gen1/...——但真实卡 GC 门的是 atomic_alloc/fullgc_raw/young_collector_test/demotion。/ 清单与真实现状脱节 → 维护者针对错误目标使劲，M1 无法借由该通道闭合。
- atomic_alloc（isolated 确定性 SEGFAULT）是 **G0 "无非确定崩溃"判据的直接反例**，此前 "GC suite 全绿" 结论漏掉了它。

## 处置（本次已做 + 建议接续）
1. **本次**：`be09ed381` 修 unit `test_gc_young_collector`（ConservativeSweep cross-test SEH）。
2. **接续**（GC 域）：
   - `atomic_alloc`（true bug，isolated SEGFAULT）→ 不塞 known-fail（会洗白真崩、违背 no-skip），保持红，修好才摘。证据见 `notes/atomic-alloc-segfault-oldgen-concurrency-2026-09-08.md`，GC 域 deep-dive sub-agent (ac20f387) 处理中。
   - `fullgc_raw`/`young_collector_test`（contracts 版，isolated pass 并行翻）→ 真并行门 flake，核实 isolated pass 后可入 known 或修根因。
   - `demotion` Timeout（isolated 也 >60-90s 未完成）→ 查是真慢（超时阈值）还是 deadlock，类 delegate_stress BGC-safepoint stall 需确认。
   - known-fail yaml：把已消失的 loh/gen1 标"复核待摘"，把 stress 项迁到其真属层或注明。

## 复现命令
```
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "chaos_gc_|test_gc_" \
  -LE "benchmark|stress|soak" -j4 --timeout 60
```
失败集在 "The following tests FAILED:" 之后。已知 bug 交叉验证见下述 atomic_alloc note。
