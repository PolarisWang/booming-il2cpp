# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / Phase 10→11（批次 1 收尾 + 批次 2 启动）
active_child: GC-N5
latest_stop_point: GC-N1/N3/N4 已落地（本地验证通过，待 CI 实跑确认）；GC-N2 由并行线承接；下一步 GC-N5（L1 卡表并发读稳定化）
```

## 下一步

1. 提交批次 1 改动（仅本人文件：gc-ci.yml、contracts CMakeLists、docs）。
2. GC-N5：L1 卡表 realloc 并发读稳定化（`gc_card_table.cpp` rebase/swap 悬垂窗口 + seg_idx 重索引核对）。
3. GC-N6：世代写屏障压力确定性化。
4. GC-N7：Release 构建 GC 基准。
5. GC-N8：dynamic_tuning 伺服闭环。
6. 批次 3（GC-N9..N12）。
