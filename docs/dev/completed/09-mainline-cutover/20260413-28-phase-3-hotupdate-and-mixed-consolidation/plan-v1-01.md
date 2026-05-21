# Phase 3 HotUpdate / Mixed Consolidation 实施计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。
> 目标：让 `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 成为真正可执行的 retained solution-mode subject；compatibility alias 与 legacy root 删除继续延后到后续 phase。

## 计划任务

- [ ] Task 1: 激活 Phase 3 并冻结 retained 范围
  - 范围：
    - 盘清 `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 的 manifest、solution、declared entry 与真实执行链路。
    - 明确本阶段不处理的内容：compatibility alias 删除、统一入口 typed dispatch、`tests/` vs `subjects/` 最终解耦。
  - 验收：
    - 子任务状态、`ACTIVE.md`、父 roadmap 状态一致。
    - Phase 3 的执行边界清晰，不和 Phase 4/6 交叉。

- [ ] Task 2: 补齐 `HotUpdateHostPack` retained pack 缺口
  - 范围：
    - 吸收 `SharedContractProof` 到 `HotUpdateHostPack`。
    - 为 `HotUpdateHostPack` 增加最小 benchmark manifest 能力：
      - `workloadEntry`
      - `perf` validation
      - 至少一个可执行 perf matrix
    - 保持 `HotUpdateHostPack` 仍是 solution-mode source。
  - 验收：
    - declared metadata 能发现新增 proof / benchmark entry。
    - planner 能直接为 `HotUpdateHostPack` 生成 perf 计划。
    - 不引入新的 legacy root。

- [ ] Task 3: 固化 `MixedExecutionFeaturePack` retained 闭环
  - 范围：
    - 复核 `MixedExecutionFeaturePack.sln`、nested support project 与 manifest/workload 配置。
    - 证明 retained pack 能直接承接 mixed-execution proof / benchmark，而不是依赖旧 shell 的独立规划语义。
  - 验收：
    - mixed execution 的 proof / benchmark 相关规划与 catalog 回归稳定。
    - retained solution 结构不回退到 project-set / shell 驱动。

- [ ] Task 4: 跑定向回归与真实 proof / benchmark，收口 Phase 3
  - 范围：
    - 跑 hot-update / mixed-execution 的 manifest、planner、catalog、registry 回归。
    - 跑 `HotUpdateHostPack` 真实 proof / benchmark。
    - 跑 `MixedExecutionFeaturePack` 真实 proof / benchmark。
    - 更新 child / parent 状态、进度记录与索引。
  - 验收：
    - 两个 retained subject 都有真实运行证据。
    - 父 roadmap 可以继续进入 Phase 4，而不需要回头补 Phase 3 的 retained 能力。

## 初始验证候选

- `python -m pytest tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_subject_planner.py tests/unit/compatibility/test_subject_manifest_schema.py tests/integration/registry/test_declared_metadata_discovery.py -q`

## 执行备注

- 本阶段不删除 `MethodReplacementProof`、`SharedContractProof`、`VersionRollbackProof` 等 legacy root。
- 本阶段不处理 `tests/unit/planning/test_subject_planner.py` 等机制测试里仍存在的 subject-heavy 语义；那是后续 cleanup phase 的收尾工作。
- 本阶段允许 `HotUpdateHostPack` 仅提供最小 perf 闭环，不强行补齐 native / interpreter 的全矩阵 benchmark。
