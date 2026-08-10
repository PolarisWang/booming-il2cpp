# Review — GC 测试工程 + 优化方案（深度分析）

> 日期：2026-08-10
> 范围：`tests/contracts/native/runtime-core/` GC 测试 target + CI 接入 + 覆盖 + sanitizer
> 目标用户诉求：单元测试覆盖全面 + 进 CI；压力测试独立项；给优化方案 + 深度分析

---

## 一、现状盘点（关键发现）

### 1. 🔴 CI：GC 单测当前**根本没有跑进任何一条 CI**

这是最重要的问题。

| 现状 | 证据 |
|------|------|
| **主 CI**（ci.yml:33-40）只 cmake 构建 + e2e small batch，**不跑 native CTest** | ci.yml |
| **ci-framework** 的 native 测试阶段用 **`-R "test_jit_*"` / `-R "test_common"` 白名单**，GC（`chaos_gc_*`）**不在这些正则内** | ci-framework.yml:138-152 |
| test-driver 的 integration 层虽有 `-LE benchmark|stress|soak`，但**它不是 CI 主链路的真正执行路径**（是手动/其他工作流） | suite_contract.yaml:40 |

**结论**：GC 单测（含全部 unit target）当前**零 CI 执行**。新增功能（K2a/b/c/d、D1、E1）更没有 CI 门禁——一旦回归，CI 不翻 red。

### 2. 压力测试：**"写了但被丢弃"，不是独立项**

- 驱动层 `ctest -LE benchmark|stress|soak`（native.py:71）把 stress 彻底排除。
- **没有 `--with-stress`/`-L stress` 反选入口**（test_driver.py 无该 CLI，搜 scripts/ 无 `-L stress`）。
- 压力 target（gc_stress_test / gc_bgc_stress_test / loh_stress_test / oversized / satb / finalizer_stress / monitor_stress / delegate_stress / capacity_stress）**无任何消费它的入口**。

### 3. GC target → label 全表（27 个 unit + 9 个 stress）

- **unit（27）**：atomic_alloc, bit_utils, bump_cache, card_table_ext, events, finalizer(_integration), gen1, handle, layout, loh, mark_bitmap, old_gen_unit, parallel_mark, poh, region, root_scanner(C++20), safepoint, sanity, scheduler, stats, tlab, young_collector, bgc_race, bgc_root_scan, bgc_smoke, worker_pool_smoke.
- **stress（9）**：bgc_stress, stress, finalizer_stress, satb_stress, loh_stress, oversized_stress, monitor_stress, delegate_stress, capacity_stress.
- **注意**：`gc_soak_test.cpp`/`gc_benchmark.cpp` 在 CMake EXCLUDE（连 target 都没建）；`gc_gen1_benchmark.cpp` 会派成 benchmark 被 `-LE` 排除。

### 4. 已知失败 / 隔离

| 项 | 现状 | 问题 |
|----|------|------|
| `gc_bgc_smoke` Test3 (BGC-YoungGc) | 源码注释 `gc_bgc_smoke.cpp:416` 靠"排最后"隔离 | **未进 known-failures baseline**，CI 全量跑会当普通失败 |
| `chaos_gc_bgc_root_scan_test` | 记入 known-failures.integration.yaml:25（慢 ~392s） | ✅ 已 baseline |
| sanitizer | `cmake/sanitizers/asan.cmake`+`rtc.cmake` 存在**但 CI 零引用** | GC 未接 ASAN/UBSAN/TSAN |

### 5. 覆盖缺口（本会话新增功能无直接单测）

| 功能 | 现状 | 缺口 |
|------|------|------|
| K2a region→gen 表 | `gc_region_test` 只测 M0/C0 base | 无 gen 断言 |
| K2b 双参屏障 | `gc_card_table_ext_test` 无 | 无 gen0→gen0 短路断言 |
| K2d card bundle | 同左 | 无 bundle 语义断言 |
| D1 provisional/半量 | `gc_scheduler_test` 只测 API 表面（注释"决策逻辑靠 stress"） | 无 |
| E1 config env | 无 | 无 env 注入测试 |

---

## 二、优化方案（按收益/成本给 3 层）

### 方案 A（推荐 · 最小闭合 · 解决"GC 无 CI"这个最致命问题）

**A1. 新增独立 `gc-ci.yml workflow`**，复用 suite_contract 的 `-LE benchmark|stress|soak` 语义，跑全量 GC unit CTest：
```yaml
ctest --test-dir artifacts/presets/windows-x64-reference -LE benchmark|stress|soak
       -j 8 --output-on-failure
```
对照 `known-failures.integration.yaml` 翻门。GC 单测首次真正进 CI。
- 成本：低（一个新 workflow + cmake 构建）。
- 收益：高（消除零 CI 空白）。

**A2. 把 `gc_bgc_smoke` Test3 的 segfault 记入 known-failures baseline**（而非只靠源码注释），避免 CI 全量跑把它当新失败。

**A3. 压力测试加 `--stress-only` 入口**：test_driver 加可选标签参数，`-L stress` 单独跑，做成 nightly/手动独立档——压力从"被丢弃"变"独立可跑"。

### 方案 B（完整 · 覆盖补齐 + sanitizer）

**B1. 为新增功能补 unit target**（本会话明确缺口，P0）：
- `gc_region_gen_test`（新）：K2a `GetRegionGen` 按 kind 返回 young/old + uncovered 保守。
- `gc_card_table_gen_test`（新）：K2b `chaos_gc_dirty_card_dst_ref` gen0→gen0 不设卡 / old→gen0 设卡 + K2d bundle。
- `gc_scheduler_reason_test`（新或扩）：D1 provisional→强制 blocking + E1 env override。
每个都是**确定性单测**（不进 stress label），进 CI。

**B2. sanitizer 接入 nightly GC 全量跑**：用现成 `cmake/sanitizers/asan.cmake` 建 ASAN 档跑 GC unit；TSAN 留给并发 stress。GC 是内存安全敏感模块，此收益最高。

### 方案 C（理想 · 分层矩阵）

```
                                         本会话
CI unit（-LE stress）      ←── A1 (立即)
CI 全量 unit + ASAN        ←── B2 (nightly)
压力独立项（-L stress）     ←── A3 (nightly/手动)
覆盖补齐（K2a/b/c,d,D1,E1）←── B1
```

---

## 三、深度分析（几个关键判断）

1. **"unit 进 CI、压力独立" 的分类已基本就位**（label 派生 + `-LE`），缺的只是**"CI 真去跑 unit"**这一个动作。所以最高杠杆不是重设计，而是 A1 把 `-LE` 语义接到真实 CI。

2. **压力"被丢弃"的根因**：`ctest_exclude` 在驱动层写死、无反选。这不是设计意图（想独立），而是**实现缺了 `-L stress` 反选**。A3 一个 CLI 参数补齐。

3. **覆盖缺口的真正风险**：K2b 双参屏障是**正确性敏感**（判错会漏跨代引用 → UAF），但只靠回归测试间接覆盖、无直接断言。这是 P0 补测优先级最高的（B1）。

4. **known-failures baseline 纪律**：bgc_smoke Test3 的已知 segfault 靠源码注释隔离是脆弱的——一旦 `-LE` 全量 CI 落地（A1），它会立刻变成"普通失败"翻门。**A2 必须在 A1 前做**，否则 A1 一上 CI 就 red。

5. **sanitizer 优先级**：GC 是内存安全最高危模块，ASAN 常态化（nightly）+ TSAN 留给并发 stress 是最优配比；现有 `cmake/sanitizers/` 闲置是浪费。

## 四、建议执行顺序

1. **A2**（5 分钟）：bgc_smoke 记入 known-failures baseline —— A1 的前置。
2. **B1（前 2 项）**：K2b 双参屏障 + K2a region-gen 单测（P0 正确性）。
3. **A1**：gc-ci workflow，GC unit 进 CI。
4. **A3**：`--stress-only` 压力独立入口。
5. **B2**：ASAN nightly；**B1 剩余**（D1/E1 单测）随补。

## 六、执行状态（2026-08-10 补测落地）

| 方案项 | commit | 状态 |
|--------|--------|------|
| **P0-1 K2b 双参屏障单测**（正确性敏感，3 短路） | `64c016bb3`+`244f1cf2c` | ✅ card_table_ext 6/6 确定性（dst/ref 分 region cell） |
| **P0-2 K2a region-gen 单测 + bounds-guard 修复** | `f825cf3a4` | ✅ region_test 5/5；修复 GetRegionGen 越界 OOB 读 |
| **P0-3 GC unit 进 CI** | `fb6556464` | ✅ 新增 gc-ci.yml（-R chaos_gc_ -LE benchmark|stress|soak） |
| **P1-1 bgc_smoke root-cause 修复** | `e511ebd05` | ✅ phase-independent BGC-YoungGC pause ack（非记录 baseline，根因修复） |
| **P1-2 --stress-only 压力独立入口** | `162515604` | ✅ test_driver 加 --stress-only；native.py -L 反选 |
| **P2 D1 provisional + E1 config 单测** | `3e718a5de`+`774daf521` | ✅ scheduler_test 10/10 |
| **nightly** | — | ✅ 按用户指示**去除**（不做定时档） |

最终验证：11 项 GC 确定性单测全部 0 失败（含新增 K2a/K2b/D1/E1）。worktree 干净。
