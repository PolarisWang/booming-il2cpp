# DISPATCH.md — 调度总纲

dispatch_model: hybrid
parallel_merge_policy: independent-completed

batches:
  - batch_id: batch-1
    mode: sequential
    description: "核心管线补全（编码器抽象 → GC → Liveness → OSR → Unwind → 测试 → CI）"
    tasks:
      - id: wf1-arch
        conflict_check: no-shared-output
        wait_for: []
      - id: wf1-gc
        conflict_check: no-shared-output
        wait_for: [wf1-arch]
      - id: wf1-liveness
        conflict_check: no-shared-output
        wait_for: [wf1-gc]
      - id: wf1-osr
        conflict_check: no-shared-output
        wait_for: [wf1-liveness]
      - id: wf1-unwind
        conflict_check: no-shared-output
        wait_for: [wf1-osr]
      - id: wf1-tests
        conflict_check: no-shared-output
        wait_for: [wf1-unwind]
      - id: wf1-ci
        conflict_check: no-shared-output
        wait_for: [wf1-tests]
    wait_for: []
    merge_after: all-completed

  - batch_id: batch-2
    mode: parallel
    description: "Linux 平台支持和调试工具"
    tasks:
      - id: wf2-linux
        conflict_check: no-shared-output
        wait_for: []
      - id: wf4-debug
        conflict_check: no-shared-output
        wait_for: []
    wait_for: [batch-1]
    merge_after: all-completed

  - batch_id: batch-3
    mode: sequential
    description: "TLAB 内联分配"
    tasks:
      - id: wf3-tlab
        conflict_check: no-shared-output
        wait_for: [wf1-arch, wf1-tests]
    wait_for: [batch-1]
    merge_after: all-completed

  - batch_id: batch-4
    mode: sequential
    description: "ARM64 完整支持"
    tasks:
      - id: wf5-arm64
        conflict_check: no-shared-output
        wait_for: [wf1-arch]
    wait_for: [batch-2, batch-3]
    merge_after: all-completed
