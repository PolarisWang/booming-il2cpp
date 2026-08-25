# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / Phase 11（批次 2：P1 工程闭环）
active_child: GC-N7（Release 基准）→ 2 真 bug 已提交（904114c3d）；残余非确定性堆破坏需真实机器 page-heap 专项
latest_stop_point: 批次1 N1/N3/N4（3f85087ed）+ N5（10013c0e5）+ N6（ef0012d49）。GC-N7 Release 基准：修 2 代码级真 bug 并提交（904114c3d：析构 FreePage UAF + ResizeGen1Region 清空 gen1）。残余 crash 为**非确定性堆破坏**（in-test :537 AV + teardown c0000374，同代码频率 8%~73% 波动）；A/B/C 修复已逐项复测 revert。需真实机器 page-heap 定位。详见 notes/gc-n7-release-benchmark-crash-2026-08-25.md
```

## 下一步

1. **GC-N7-专项（P0）**：残余非确定性堆破坏——同代码 `YoungGcPauseUnderLoad` 频率 8%~73%（`GcYoungCollection:537` 扫越界 + `~MarkSweepOldGen:127` teardown `c0000374`）。需真实机器 `gflags /p /full` page-heap 定字节 + systematic-debugging 查 BGC 并发/堆写越界。修复前不产出 Release 基准数值。
2. GC-N6-专项（P0）：gen1 region ↔ old-gen 页虚拟重叠（notes/gc-n6-liveness-findings）。
3. GC-N8：dynamic_tuning 伺服闭环。
4. 批次 3（GC-N9..N12）。
