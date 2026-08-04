# DISPATCH.md — 调度总纲

dispatch_model: parallel-batch
parallel_merge_policy: independent-completed

batches:
  - batch_id: batch-1
    mode: parallel
    description: "核心修复 + EventPipe 桥接（3 终端并行）"
    tasks:
      - id: M1
        conflict_check: no-shared-output
        wait_for: []
      - id: M5
        conflict_check: no-shared-output
        wait_for: []
      - id: M6
        conflict_check: no-shared-output
        wait_for: []
    wait_for: []
    merge_after: all-completed

  - batch_id: batch-2
    mode: parallel
    description: "测试套件（2 终端并行）"
    tasks:
      - id: M2
        conflict_check: no-shared-output
        wait_for: []
      - id: M3
        conflict_check: no-shared-output
        wait_for: []
      - id: M4
        conflict_check: no-shared-output
        wait_for: []
    wait_for: [batch-1]
    merge_after: all-completed

## 终端分配

batch-1:
  - terminal-1: M1 (EventPipe)
  - terminal-3: M5 + M6 (修复类，同一终端串行执行)

batch-2:
  - terminal-2: M2 + M3 (stress + benchmark，同一终端串行)
  - terminal-2: M4 (proof，与 M2/M3 共享终端)

注意: M5 和 M6 共享 conflict_scope（涉及 interpreter 和 runtime-core 交叉），建议同一终端串行执行 M5 → M6。
M2/M3/M4 均为 testing/ 下新文件，无冲突，可同一终端串行执行。
