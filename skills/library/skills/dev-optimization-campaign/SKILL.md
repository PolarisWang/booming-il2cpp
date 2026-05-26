---
name: dev-optimization-campaign
description: 多 family 性能优化战役的编排技能。基于基线驱动 pipeline 模式（方案 C）：coordinator 先跑全量 baseline，多个 agent 各自 claim family 做优化，最后聚合对比报告。通过文件级协调机制避免冲突。
---

# 优化战役编排

## 工作流

```
Phase 0 ─ Coordinator: 全量 baseline → optimization-campaign/state.json phase=baseline_done
              ↓
Phase 1 ─ Worker 并行优化（通过 state.json 原子 claim 协调）：
  Worker 1: claim → 优化 → build → benchmark → 写结果 → state.json status=done
  Worker 2: claim → 优化 → build → benchmark → 写结果 → state.json status=done
              ↓
Phase 2 ─ Coordinator: 聚合 diff → 生成 comparison report
```

## state.json 完整 schema（核心协调文件）

所有协调状态集中在一个文件，通过**原子写入**（write temp → rename）保证一致性。

```json
{
  "schema_version": 1,
  "campaign_name": "optimization-2026-05",
  "phase": "created | baseline | baseline_done | optimizing | aggregation | done",
  "phase_owner": "coordinator",
  "phase_transition_at": null,
  "families": {
    "array-indexing-copy": {
      "status": "available | claimed | done | excluded",
      "agent": null,
      "claim_stamp": null,
      "claimed_at": null,
      "heartbeat_at": null,
      "result_path": null,
      "baseline_path": "optimization-campaign/baseline/array-indexing-copy.json",
      "health": "healthy | partial | broken",
      "health_note": null
    }
  },
  "next_claim_stamp": 1,
  "benchmark_lock": null,
  "sdk_version": 1,
  "created_at": "2026-05-26T10:00:00",
  "updated_at": "2026-05-26T10:00:00"
}
```

### Phase 流转

```
created → (coordinator 选 families) → baseline → (coordinator 跑完) → baseline_done
  → (coordinator 开放 claim) → optimizing → (所有 family done) → aggregation
  → (coordinator 聚合完) → done
```

每个 phase 推进由特定角色触发，其他角色读到 phase 变化后自适应。

## 角色

| 角色 | 数量 | 职责 |
|------|------|------|
| **Coordinator** | 1 | 跑 baseline、管理 state.json、处理共享层 proposal、聚合结果 |
| **Worker** | 1-N | 原子 claim family、优化、build、benchmark、写结果 |

## 目录契约

```
optimization-campaign/
├── state.json              # 唯一协调文件（原子写入）
├── baseline/
│   ├── array-indexing-copy.json    # 每个 family 独立 baseline
│   ├── enum-parsing.json
│   └── ...
├── proposals/              # 共享层变更提案（Worker → Coordinator）
│   ├── 001-rename-inline-helper.md
│   └── 002-add-gc-method.md
├── workers/
│   ├── agent-01/
│   │   ├── status.md       # 进度日志（worker 自维护）
│   │   ├── blocker.md      # 阻塞项（如有）
│   │   └── results/        # 优化后 benchmark 结果
│   │       └── array-indexing-copy.json
│   └── agent-02/
│       └── ...
└── aggregate/
    └── comparison-report.json  # Phase 2 聚合报告
```

## 阶段详解

### Phase 0：Baseline（Coordinator 独占）

1. 创建 `optimization-campaign/` 目录
2. 选定 families，写 `state.json`（phase=`created`）
3. 推进 phase → `baseline`，确定 benchmark iterations（默认 `100000`，所有 family 和后续 Phase 1 必须一致）
4. 逐 family 获取 method count，循环跑每个 method 的 baseline：
   ```bash
   # method_count 从 verification_dispatch.generated.cpp 或已有的 native-aot-benchmark.json 获取
   for method_index in $(seq 0 $((method_count - 1))); do
       entry.exe --benchmark $method_index 100000
   done
   ```
5. 每个 family 的 baseline 写入 `baseline/<family>.json`
6. 评估每个 family 的健康等级：
   ```
   healthy:  >=80% methods completed（可优化）
   partial:  1%-79% methods completed（先修再优化）
   broken:   0% methods completed（标为 excluded，不参与战役）
   ```
   写入 `state.json.families[X].health`
7. 如果有共享层优化要做，在 Phase 0 一并完成（因为会影响所有 family 的 baseline）
8. 推进 phase → `baseline_done`，开放 claim

### Phase 1：并行优化（Worker + Coordinator 监控）

**Coordinator Phase 1 监控循环（每 5 分钟）：**

```
1. 读 state.json
2. 扫 proposals/ → 如果有新提案：
   - 评审必要性
   - 如果接受：应用到共享代码 → rebuild SDK → 更新 state.json.sdk_version
   - 如果拒绝：在 proposal 中回复理由
3. 遍历所有 status="claimed" 的 family：
   - 检查 heartbeat_at：如果超时 15 min → unclaim（置回 available）
   - 写入时增加 stamp 版本，确保被 unclaim 的 worker 后续写入会被拒绝
4. 遍历所有 status="done" 的 family：
   - 检查 result_path 是否存在、数据是否完整
5. 如果所有 family 都是 "done" → 推进 phase → "aggregation"
```

**Claim 协议（原子 check-and-claim）：**

```
Worker 启动后循环：
1. 读 state.json
2. 如果 phase != "optimizing" → 等待
3. 选一个 status="available" 且 health 为 "healthy" 或 "partial" 的 family
4. 如果 health="partial": 优化目标默认为"先修复所有 failed/throws method"，修复完成后再进入性能优化
5. 本地构造新 state：设该 family status="claimed", agent="agent-01", claimed_at=now
6. 原子写入 state.json（write temp → rename）
7. 如果写入时文件已被改过（stat mismatch）→ 重试 step 1
8. 成功 → 开始优化
```

**状态机：**

```
claimed → (开始优化) → diagnosing → (6 阶段诊断: Fact → Codegen → 
Benchmark → vs.NET8 → HotUpdate → 综合排名) → optimizing → (改代码)
→ building → (编译) → verifying → (重新跑 fact 确认) → benchmarking
→ (获取 benchmark_lock, 循环跑所有 methods) → done
```

Worker 进入 `diagnosing` 后执行标准诊断步骤（6 阶段）：

```
Phase A — 测试用例正确性（Fact Validation）
  1. 跑 entry.exe --fact-json
  2. 检查 JSON 输出：全部 passed==true
  3. 验证 exitCode 语义：
     - 预期正常完成的方法：exitCode == 0
     - 预期 throw 的方法（如 Convert.ToChar(bool)）：标记为"throws"，确认 catch 机制正确
  4. 如果任何 method 的 passed==false → 标记为 blocking issue，先修验证再继续

Phase B — 生成代码正确性（Codegen Audit）
  1. 读取 native-aot.generated.cpp，对每个 Subject_N 函数分析：
     - 桥接调用模式：kChaosExternalRuntimeFnTable bridge call vs 内联实现
     - 异常处理：CHAOS_EH_TRY/CATCH 结构是否正确
     - 类型转换：static_cast 是否合理
     - GC 分配：CHAOS_IL2CPP_NEW_GC / CHAOS_IL2CPP_NEW_GC_ARRAY 是否必要
  2. 标记异常模式：
     - chaos_raise_exception(nullptr) 等未接通的异常路径
     - 明显缺失的类型检查（如 Object 参数未做类型判断）
     - 预期内联但走了 bridge call 的方法
     - **不必要的 GC 分配**：new 出临时对象后立即使用并丢弃（应改用栈分配或已有实例）
  3. 生成 codegen 健康度摘要：记录到 status.md

Phase C — 多运行时 Benchmark + 内存分配采集
  1. 从 multi-run/per-runtime/ 目录查找已有数据：
     - chaos-aot.json（native AOT）
     - chaos-jit.json（native JIT）
     - net8-jit.json（.NET 8 JIT 基线）
     - net10-jit.json（.NET 10 JIT 基线）
     - chaos-hu-aot.json（hotupdate AOT）
     - chaos-hu-jit.json（hotupdate JIT）
  2. 如果缺少某个文件，从对应技术的 entry 重新采集：
     - AOT: entry.exe --benchmark <index> <iterations>
     - JIT: entry.exe --benchmark-jit <index> <iterations>
     - Hotupdate AOT: entry.exe --benchmark-hu-aot <index> <iterations>
     - Hotupdate JIT: entry.exe --benchmark-hu-jit <index> <iterations>
  3. 收集全部运行时的 ns/op 和 alloc/op 数据：
     - **allocatedBytes**: 单次 benchmark 的总分配字节数（来自 chaos-aot JSON 的 `allocatedBytes` 字段）
     - **allocPerOp**: `allocatedBytes / iterations`，每次调用平均分配字节数
     - 从 managed harness JSON 中同样提取 `allocatedBytes` 字段（GC.GetAllocatedBytesForCurrentThread()）
  4. 统一记录到 workers/<id>/results/<family>.json

Phase D — 与托管基线对比（vs .NET 8/10 JIT）
  1. 对每个 method，计算：
     aot_slowdown_vs_net8 = (chaos_aot_ns - net8_ns) / net8_ns * 100
     jit_slowdown_vs_net8 = (chaos_jit_ns - net8_ns) / net8_ns * 100
  2. 分类标记：
     - "faster-than-net8": AOT 比 .NET 8 快（正数）
     - "within-noise": ±50% 以内
     - "slower-than-net8": AOT 比 .NET 8 慢 50%+
     - "critically-slow": AOT 比 .NET 8 慢 200%+（需要优先优化）
  3. **分配对比**：如果 chaos-aot 和 net8-jit 都有 allocPerOp 数据：
     - 对比 native vs managed 每次调用的分配量
     - `extra_allocs = chaos_aot_alloc_per_op - net8_alloc_per_op`
     - 如果 extra_allocs > 0 → native 比 managed 多分配了字节（通常是 bridge call 导致 GC 分配）
     - 多分配 + 慢速度的组合表示"分配瓶颈"，标记为 high priority
     - 记录分配对比表到 status.md（含 allocPerOp 列）
  4. 记录对比表到 status.md

Phase E — HotUpdate 开销分析
  1. 如果 chaos-hu-aot.json 存在，对每个 method 计算：
     hu_aot_overhead = (hu_aot_ns - aot_ns) / aot_ns * 100
  2. 同样计算 hu_jit_overhead（如果 chaos-hu-jit.json 存在）
  3. 分类标记：
     - "low-overhead": <10%
     - "moderate-overhead": 10-50%
     - "high-overhead": 50-100%
     - "critical-overhead": >100%
  4. 记录 hotupdate 开销表到 status.md

Phase F — 综合瓶颈排名
  1. 综合所有信号生成优化优先级：
     - P0: codegen 正确性问题（fact failed、异常路径未接通）
     - P1: vs .NET 8 严重退化（critically-slow）+ 非 bridge call 限制
     - P2: hotupdate 开销过高（critical-overhead）
     - P3: vs .NET 8 明显退化（slower）
     - P4: **过度分配** — native allocPerOp 显著高于 managed（extra_allocs > 0），即使速度差距不大也应优化
     - P5: 绝对值最慢（内部热点排名）
  2. 明确标注哪些瓶颈是 family 级可优化的、哪些需要共享层 proposal
  3. 写入 status.md 的 ## 优化策略 小节
```

Worker 更新 `state.json` 中的 `families[X].status` 来汇报进度。Coordinator 通过 `state.json.families[X].heartbeat_at` 检测存活。

**Claim Stamp（防孤儿 worker 写入）：**

每个 claim 分配一个唯一的 stamp（递增整数），worker 后续的所有写入必须携带该 stamp：

```
Coordinator 分配 claim 时:
  families[X].claim_stamp = state.next_claim_stamp
  state.next_claim_stamp += 1

Worker 写入结果/更新 status 时:
  先读 state.json，验证 families[X].claim_stamp 与自己持有的 stamp 一致
  如果不一致 → 说明被 unclaim 了 → 放弃写入，重新 claim

Coordinator unclaim 时:
  置 claim_stamp = null（使旧 stamp 失效）
```

这解决了：worker 被 unclaim 后继续写 state.json → 数据被后来的 claim 覆盖的问题。

**Heartbeat 机制：**

```
Worker 每 5 分钟更新 state.json:
  families[X].heartbeat_at = now

Coordinator 检测：
  if now - families[X].heartbeat_at > 15 min → unclaim（置回 available）
```

**Benchmark 锁规则（集成在 state.json）：**

```
获取锁：
1. 读 state.json
2. 如果 benchmark_lock != null → 另一个 worker 在用，稍后重试
3. 设 benchmark_lock = { holder: "agent-01", family: "array-indexing-copy", acquired_at: now }
4. 原子写入 state.json
5. 成功 → 跑 benchmark（循环跑该 family 所有 methods）:
   ```bash
   for method_index in $(seq 0 $((method_count - 1))); do
       entry.exe --benchmark $method_index $iterations
   done
   ```
   收集全部结果，写入 workers/<agent-id>/results/<family>.json
6. 跑完 → 设 benchmark_lock = null，原子写入

禁止：一次 benchmark 多个 family（数据互相干扰）
禁止：获取锁后长时间不释放（超时规则见时间预算表）
```

**Worker 允许修改的白名单：**

```
允许（worker 独占修改）:
  testing/foundation-dll/<assembly>/<family>/native/*.cpp   # 但 CMakeLists.txt 只读
  testing/foundation-dll/<assembly>/<family>/managed/patch/*.cs
  testing/foundation-dll/<assembly>/<family>/benchmark/*

只读（必须走 proposal）:
  testing/foundation-dll/<assembly>/<family>/native/CMakeLists.txt
  testing/foundation-dll/<assembly>/<family>/capability-family-contract.json
  src/native/runtime-core/**
  src/managed/Chaos.IL2CPP.Generator/**
  build/toolchains/**

构建隔离：
  每个 worker 使用独立 build 目录：
  optimization-campaign/workers/<agent-id>/build/
```

**共享层提案流程：**

```
Worker 发现需要改共享代码：
1. 写 proposals/<seq>-<title>.md
2. 继续做 family 级优化（不阻塞）
3. Coordinator 定期扫 proposals/，评审后应用
4. Coordinator 更新 sdk_version，rebuild SDK
5. Worker 检测到 state.json.sdk_version 变化：
   a. 重新链接 entry（使用新 SDK）
   b. 重新跑 baseline benchmark（因为共享层改变可能影响所有方法的性能基数）
   c. 将新 baseline 写回 baseline/<family>.json（覆盖旧 baseline）
   d. 记录 baseline 变更原因到 status.md
   e. 继续优化
```

### Phase 2：聚合（Coordinator 独占）

1. 收集 `workers/*/results/*.json`
2. 与 `baseline/<family>.json` 逐 method 对比
3. 计算加速比：
   ```
   speedup = optimized_ops_per_sec / baseline_ops_per_sec
   ```
   > speedup=2.0 表示优化后快了 2 倍；speedup=0.8 表示比 baseline 慢了 25%。
4. 生成 `aggregate/comparison-report.json`（复用 `benchmark-comparison-report.json` schema）
5. 推进 phase → `done`

## 约束规则

### 1. 原子写入

`state.json` 是唯一共享写入点，必须：

```python
import os, json, time

def atomic_write_state(path, state):
    tmp = path + ".tmp." + str(os.getpid())
    with open(tmp, "w") as f:
        json.dump(state, f)
        f.flush()
        os.fsync(f.fileno())
    os.replace(tmp, path)  # 原子替换
```

### 2. 中断恢复

Worker 断连重连后：

1. 读 `state.json`
2. 查找 `families[X].agent == self.agent_id` 且 `status != "done"`
3. 如果找到 → 继续之前的工作
4. 如果没找到 → claim 新的 family

### 3. 构建缓存

- SDK 在 Phase 0 统一构建一次
- SDK 路径推导规则：
  ```
  <repo-root>/testing/foundation-dll/<assembly>/<family>/codegen/<AssemblyName>Subjects/
  ```
  例如 array-indexing-copy:
  ```
  testing/foundation-dll/System.Private.CoreLib/array-indexing-copy/codegen/ArrayIndexingCopySubjects/
  ```
- Worker 通过独立 build 目录（`workers/<agent-id>/build/`）增量编译
- CMake 配置命令：
  ```bash
  SDK_DIR=<repo-root>/testing/foundation-dll/<assembly>/<family>/codegen/${AssemblyName}Subjects/
  cmake -S testing/foundation-dll/<assembly>/<family>/native \
        -B optimization-campaign/workers/<agent-id>/build/ \
        -DCHAOS_SDK_DIR=$SDK_DIR
  cmake --build optimization-campaign/workers/<agent-id>/build/
  ```

### 4. 时间预算

| 阶段 | 时间上限 | 超时处理 |
|------|---------|---------|
| 单次优化 | 30 min | coordinator unclaim family |
| Benchmark 锁持有 | 30 min | coordinator 强制释放锁 |
| Heartbeat 间隔 | 5 min | — |
| Heartbeat 超时判定 | 15 min | coordinator unclaim family |

### 5. 数据完整性

每个 benchmark 结果必须包含：

```json
{
  "methodIndex": 0,
  "methodSubjectId": "System.Private.CoreLib/System.Array::Copy:...",
  "elapsedMilliseconds": 31.858,
  "calibratedMs": 31.858,
  "opsPerSecond": 3138889.6,
  "iterations": 100000,
  "status": "completed"
}
```

与 `native-aot-benchmark.json` schema 一致。不允许只写 status 不带数据。

### 6. 通信协议

Worker → Coordinator 通过文件系统：

| 场景 | 协议 |
|------|------|
| 分配 family | 原子 check-and-claim（state.json） |
| 进度报告 | 更新 state.json families[X].status + workers/<id>/status.md |
| 存活信号 | 更新 state.json families[X].heartbeat_at |
| 共享层提案 | 写 proposals/<seq>.md |
| 遇到阻塞 | 写 workers/<id>/blocker.md |
| 完成 | 更新 state.json families[X].status="done" |

## 验收口径

一个优化的完成标准（Phase 1 worker 必须满足）：

> **优先级说明**：本验收口径是优化战役的最小要求。如果项目级 `dev-verification-before-completion` 技能有更严格的闸门（如运行全量 unified pipeline、刷新 formal authority），以更严格的为准。

1. ✅ fact 验证通过（全部 passed）

   ```
   worker 在 verifying 阶段执行：
     cd testing/foundation-dll/<assembly>/<family>/native/
     cmake --build optimization-campaign/workers/<id>/build/ --config Release
     ./build/Release/entry.exe --fact-json
     检查 JSON 输出：全部 passed==true，无 caught==true
   ```
2. ✅ benchmark 数据完整、无 failed
3. ✅ 结果写入 `workers/<agent-id>/results/<family>.json`
4. ✅ 与 baseline 对比：不得慢于 baseline 的 10% 以上。若退化，必须注明根因并恢复 baseline
