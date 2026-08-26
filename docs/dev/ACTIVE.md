# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / 批次2+3 持续推进
active_child: GC-N8/N9/N10/N11 已提交；GC-N7 残余 + GC-N6 + GC-N12 被沙箱/真机依赖阻塞
latest_stop_point: 批次1 N1/N3/N4（3f85087ed）+ N5（10013c0e5）+ N6（ef0012d49）+ GC-N7 双真bug（904114c3d）。批次2/3 已提交 GC-N8（bf1b83503+048b9f15c）、GC-N10（202c62f22）、GC-N11（d35d78dcd）、GC-N9（a77aff4dd）。**GC-N7 残余根因 ASAN 定位（96a61897b）**：GcScanAllThreadRoots 保守栈扫读越界晋升垃圾；干净修复需重设计 self 线程保守/精确扫边界（方案 B=live RSP+页提交探针）。真机 + ASAN 完成方案 B
```

## 下一步

1. **GC-N7-专项（P0，阻塞性）**：残余根因已 ASAN 定位 = `GcScanAllThreadRoots` 保守栈扫越界晋升垃圾（见 notes/gc-n7-release-benchmark-crash-2026-08-25.md 〇节）。**修复方案 B**（self 线程 live RSP + 页提交探针）需真机 + ASAN 完成并回量 Release 崩溃率。阻塞 GC-N12、GC-N8<5%量化、GC-N9 runtime。
2. **GC-N9/runtime（after GC-N3）**：Server harness 健全后跑 AdjustHeapCount。
3. **GC-N6-专项（P0）**：gen1 region ↔ old-gen 页虚拟重叠。
