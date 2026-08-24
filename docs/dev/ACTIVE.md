# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / Phase 11（批次 2：P1 工程闭环）
active_child: GC-N6-专项（gen1↔old-gen 重叠）优先；其次 GC-N7
latest_stop_point: 批次1 N1/N3/N4 已提交（3f85087ed）；GC-N5 已提交（10013c0e5）；GC-N6 调查暴露 2 缺陷——屏障碰撞已修（ef0012d49），gen1↔old-gen 重叠升级 P0 专项（36662a9cc 固化）
```

## 下一步

1. **GC-N6-专项（P0）**：gen1 region ↔ old-gen 页虚拟重叠 → gen1 collection 破坏 old-gen 对象（见 notes/gc-n6-liveness-findings-2026-08-25.md）。修复后重新启用内容存活校验测试。
2. GC-N7：Release 构建 GC 基准。
3. GC-N8：dynamic_tuning 伺服闭环。
4. 批次 3（GC-N9..N12）。
