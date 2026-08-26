# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / 批次2+3 持续推进
active_child: 已提交项全部在基线绿；GC-N7 残余转真机（仅 Windows）专项
latest_stop_point: 已提交 GC-N8（bf1b83503+048b9f15c）/ N9（a77aff4dd）/ N10（202c62f22）/ N11（d35d78dcd）+ GC-N7 双真bug（904114c3d）。基线 GC 单测全绿（old_gen 6/6, region 18/18, card_table 5/5, scheduler 11/11, events 7/7, heap_manager 3/3）。**GC-N7 残余：沙箱内推断性补丁(A/B/X/Y/CoreCLR版)全证净负已回滚；真机路径仅验证 Windows（用户决策）**——page-heap 归因确切坏字节。
```

## 下一步

1. **GC-N7-专项（P0，真机·仅 Windows）**：残余非确定性堆破坏——根因已 ASAN 定位（`GcScanAllThreadRoots` 保守栈扫越界晋升垃圾），沙箱推断性补丁全证净负（见 notes/gc-n7...md）。**真机 Windows 开 `gflags /p /full` page-heap → 归因确切坏字节 → 据实定点修**。阻塞 GC-N12、GC-N8<5%量化、GC-N9 runtime。
2. **GC-N6-发现3 专项**：typed young-GC 无限循环挂起（内容校验原型暴露），需重载测试 harness 后用 cdb 定位循环点。
3. **GC-N9/runtime（after GC-N3）**：Server harness 健全后跑 AdjustHeapCount。
```
