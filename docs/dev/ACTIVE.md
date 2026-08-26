# ACTIVE — 当前活动任务

```yaml
task_id: gc-align-coreclr
task_dir: docs/dev/in-progress/gc-align-coreclr
status_file: docs/dev/in-progress/gc-align-coreclr/STATUS.md
lifecycle_status: in-progress
phase: roadmap-v3 / 批次2+3 持续推进
active_child: GC-N8/N9/N10/N11 已提交；GC-N7 残余 + GC-N6 + GC-N12 被沙箱/真机依赖阻塞
latest_stop_point: 批次1 N1/N3/N4（3f85087ed）+ N5（10013c0e5）+ N6（ef0012d49）+ GC-N7 双真bug（904114c3d）。批次2/3 新增落地：GC-N8（bf1b83503+048b9f15c）、GC-N10（202c62f22）、GC-N11（d35d78dcd）、GC-N9（a77aff4dd，Server-compile 通过、runtime 待 GC-N3）。余：GC-N7 残余堆破坏（真机 page-heap）阻塞 GC-N12/GC-N6/GC-N8-量化
```

## 下一步

1. **GC-N7-专项（P0，阻塞性）**：残余非确定性堆破坏——需真实机器 `gflags /p /full` page-heap 定字节。**阻塞 GC-N12（profile 调参）、GC-N8 <5% 量化、GC-N9 runtime 验证（连同 GC-N3 Server harness）**。
2. **GC-N9/runtime（after GC-N3）**：Server harness 健全后在 Server 构建跑 AdjustHeapCount 测试。
3. **GC-N6-专项（P0）**：gen1 region ↔ old-gen 页虚拟重叠。
4. 批次 3 后续：GC-N12（依赖 GC-N7）。
