# Design — GC-C1：并行标记声明式终止（对齐 CoreCLR）

> 版本：design-c1-v1-01
> 创建：2026-08-10
> 上游：`docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md`（Phase 1, GC-C1）
> 优先级：P1（正确性硬化）

---

## 1. 现状协议（gc_parallel_mark.cpp:315-378）

当前终止依赖全局 `active_workers` 计数器 + `parallel_done` 标志：

- 每个 worker 进入外层循环时 `active_workers.fetch_add(1)`，无工作时 `fetch_sub(1)`。
- **最后一个把 `prev <= 1` 的 worker** `parallel_done.store(true)` 并 break。
- 非最后 worker 进入内层 wait 循环，定期 pop/steal，找到工作则 re-increment 回外层。

每个 worker 通过 `FlushPending`（loop 顶，:334）把新标记对象 flush 到**自己** deque，next 迭代 `PopChunk`（:338）能找到。

## 2. 架构优先核验结论（2026-08-10）

**当前协议对"自发 work"（own-deque）是自洽正确的**：
1. worker 处理 chunk 时新标记对象累积到 `worker->pending`（本 worker）。
2. loop 顶 `FlushPending` 写到**自己** deque → 下一迭代 `PopChunk` 能取。
3. 一个 worker 只有当自己 deque 空时才 fetch_sub（不会带着自己 work 变 idle）。

**唯一理论竞态（对齐 CoreCLR 声明式要堵的）**：
worker A 在 `ProcessChunk` 中，即将 `FlushPending` 新 work 到自己 deque；
worker B（idle）`fetch_sub(1)` 见 `prev<=1` → `parallel_done=true`。
A 处理完 flush 新 work，但外层 `while(!parallel_done)` 已退出 → **A 刚产生的新 work 未被处理**。

因为 work 只写 own deque，且所属 worker 仍 active，现实中此窗口极窄；但对齐 CoreCLR 声明式应**保证收敛可证明**而非依赖时序。

## 3. 对齐 CoreCLR 的方法（mark_phase.cpp:3107 声明式 + gc_t_join）

CoreCLR 用每 worker 独立完成声明（`card_mark_done` flag）+ `gc_t_join` 屏障，且依赖"重扫至稳定"终止。我们把 **最后设置 done 的 worker 做一次全 deque 收敛复查**：若发现仍有余存 work，清除 done 标志让其余 worker 恢复，直到真正无 work。

### 收敛复查（最小改动，对齐声明式）

```cpp
// 在原 "设置 done 并 break" 处（:354-358）改为：
int prev = ctx->active_workers.fetch_sub(1, std::memory_order_acq_rel);
if (prev <= 1) {
    // 我们是最后一个 idle 的 worker。按声明式协议做一次收敛复查：
    // 若任何 worker 的 deque 仍有余存（可能是并发在途的 flush），
    // 清除 done 让所有人恢复，否则才真正终止。保证 "重扫至稳定"。
    if (AnyWorkRemaining(ctx)) {
        ctx->parallel_done.store(false, std::memory_order_release);
        // re-increment 回到外层继续
        ctx->active_workers.fetch_add(1, std::memory_order_relaxed);
        work_found = true;
        continue;  // 回到外层循环
    }
    ctx->parallel_done.store(true, std::memory_order_release);
    break;
}
```

辅助函数 `AnyWorkRemaining`：遍历所有 worker 的 deque，任一非空（用 `steal_mutex` 保护读）→ true。

### 为什么要 fetch_add 回来

last worker 复查发现余存 work 时，它虽已 fetch_sub 归零，但该 work 属于 deque 中某个 worker（可能已被标记为 idle 在 inner wait）。简单做法是 last worker **自己** re-increment 回外层，继续 pop/steal 那批余存 work。这保证不依赖其他 worker 必然醒来。

---

## 4. 对齐的终态语义

- **正确性**：任何可能在途 flush 的新 work，都会被 last-worker 的收敛复查捕获（它扫所有 deque），保证收敛可证明。
- **性能**：复查是 O(worker_count × deque_size)，仅在最后 idle 时跑一次，开销可忽略。
- **与 CoreCLR 一致性**：等同 mark_phase.cpp:3107 的声明式 + 重扫至稳定终止。

## 5. 验收标准（exit criteria）

- [ ] `chaos_gc_parallel_mark_test` 通过（含 MultiRoundWorkStealing / StealFromBusy / FlushPending）
- [ ] 高并发 ParallelMark（大页集）运行稳定，无 hang/漏标记
- [ ] `chaos_runtime_core` Debug 编译通过
- [ ] 对齐 CoreCLR 声明式：最后 idle worker 做收敛复查

---

## 6. 备选

若复查的 `work_found=true; continue` 引入意外死循环（deque 反复被并发改动），降级为"复查 + 短暂 re-check 上限"（如复查 N 次仍有余存则强制终止并记日志）。但 own-deque 语义下正常不应出现。
