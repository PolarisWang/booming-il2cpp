# Design — GC-D1：OOM 逐级降级链（对齐 CoreCLR handle_oom）

> 版本：design-d1-v1-01
> 创建：2026-08-10
> 上游：`docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md`（Phase 2, GC-D1）
> 对标基准：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\allocation.cpp:2055-2091 handle_oom`

---

## 1. CRAG 现状（gc_api.cpp:47 HandleOomCondition）

已有阶梯：
```
Step 0 硬限 guard（超硬限跳过 full GC）
Step 1 阻塞 full GC
Step 2 retry alloc
Step 3 emergency reserve
Step 4 OOM 事件 → nullptr
```

## 2. CoreCLR handle_oom 要点（对齐目标）

CoreCLR（allocation.cpp:2055-2091）在终态 OOM 处做两件 CRAG 没有的事：
1. **半量预算 clamp**（:2058-2061）：`oom_budget` 时把上报的失败 size 压成
   `dd_min_size(gen0)/2`，规整"预算耗尽误差"的可预期量级。
2. **归因升级**（:2063-2070）：若最近一次 GC 的 `fgm_result` 显示曾
   reserve/commit 失败（`fgm != fgm_no_failure`），才判定为真 OOM
   （`oom_low_mem`）；否则视"预算误报"。**区分真内存耗尽 vs 误报**是关键。
3. **复位**（:2083）：记录后立即复位 fgm，下次分配重走状态机，OS 恢复即恢复。

## 3. GC-D1 增量 1（本次实现）：对齐终态 OOM 路径

安全、有界、可验证，对齐 CoreCLR 的 handle_oom 终态语义：

### 3a. 半量预算 clamp（对齐 :2058-2061）
在 Step 4（emergency reserve 也失败）上报 OOM 事件前，把 `size` 钳为
`min_alloc_budget/2`。CRAG 的可用"预算下界"取 `kMinGen1Size`（4MB）/2 = 2MB
（更贴 Nursery min 下界 32KB；用 min(+4MB, nursery_min/2) 保守取 32KB 量级更稳）。
这统一 OOM 异常信息的失败量级，对齐 CoreCLR 语义。**不改分配行为**，只改上报。

### 3b. OOM 归因升级（对齐 :2063-2070）
加一个进程级原子 flag `s_recent_gc_mem_failure`：
- 置位：`HandleOomCondition` 里 emergency reserve 也失败（Step 3 耗尽）时 → 说明真尝试过 reserve/commit 且失败。
- 复位：某次分配最终成功（Step 2 retry 成功 或 emergency reserve 成功）时 → 说明已恢复。
Step 4 判定：`s_recent_gc_mem_failure` 为 true → 记录真实 OOM（含 reason 标志）；
否则记录为"预算/瞬态失败"（仍返回 nullptr 但 reason 区分）。对齐 CoreCLR
"真 OOM vs 误报"。

### 3c. 恢复（对齐 :2083）
不新增显式恢复标志：每次 `HandleOomCondition` 结束时复位归因 flag，下次分配
重走阶梯，OS 恢复即正常。与 CoreCLR 一致。

## 4. 验收（exit criteria）

- [ ] `chaos_runtime_core` Debug 编译通过
- [ ] 内存耗尽时走完四级降级（full GC → retry → emergency reserve → OOM），OOM 事件上报半量 size
- [ ] `s_recent_gc_mem_failure` 归因：真 OOM 与瞬态失败 reason 可区分
- [ ] `emergency reserve` 非耗尽场景下无回归（GC 单测批 0 失败）

## 5. 后续增量（本次不做，记录）

CoreCLR 的 **provisional mode**（高记忆+高碎片→禁 expand + gen1 强制 compact
+ NGC2 而非全GC）与 **记忆压力 decommit**（常态回收而非仅 OOM 兜底）涉及
调度器全局，是更深的 GC-D1 后续 / 独立任务。本次只对齐终态 OOM 归因与半量预算。
