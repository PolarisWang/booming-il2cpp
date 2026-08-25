# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / Phase 11（批次 2：P1 工程闭环）
active_child: GC-N7（Release 基准）→ 暴露 2 真缺陷已修（析构 UAF + ResizeGen1Region 清空 gen1）；残余偶发 young-GC scan_ptr 破坏升级专项
latest_stop_point: 批次1 N1/N3/N4（3f85087ed）+ N5（10013c0e5）+ N6（ef0012d49）。GC-N7 Release 基准中断于 throughput YoungGcPauseUnderLoad SEH：已修 2 缺陷（crash 率 80%→~5-10%）；残余【缺陷2】跨域（young-GC scan + old-gen PageArray + gen1）升级 dev-systematic-debugging。详见 notes/gc-n7-release-benchmark-crash-2026-08-25.md
```

## 下一步

1. **GC-N7-专项（P0，新增）**：残余 young-GC Phase 2 `scan_ptr` 越界（`gc_young_collector.cpp:537`）——`reclaimed` unsigned 下溢 + old-gen `retired_arrays_` teardown AV，root 横跨 young-GC/gen1/old-gen PageArray；走 dev-systematic-debugging。crash 率 ~5-10%（两个已修缺陷后）。
2. GC-N6-专项（P0）：gen1 region ↔ old-gen 页虚拟重叠（notes/gc-n6-liveness-findings）。
3. GC-N8：dynamic_tuning 伺服闭环。
4. 批次 3（GC-N9..N12）。
