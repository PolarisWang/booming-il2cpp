# CoreLib Supplemental Full Closure Audit Integration 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成时必须先归档当前 child 任务目录，再回写父 roadmap。

**目标：** 把真实 `System.Private.CoreLib.dll` 的 supplemental full-assembly-closure artifacts 接入 `SolutionCorePack` 当前 canonical proof/audit 链路，扩大可审计的 CoreLib DLL boundary translated coverage。

**架构：** 在现有 `run_frontend_pipeline_worker -> emit-native-reference / emit-native-aot -> subject_reporting` 主链上增加 supplemental full-closure sidecar。主 proof 仍保持当前 `GoldenCoreLibReference.NativeProofApp` 窄 packet，不改动其 formal object；新增的是同 run 下的真实 CoreLib supplemental closure evidence，并把它暴露到 generated/reporting 层。

**技术栈：** Python subject worker/reporting、`Chaos.IL2CPP.Driver` 现有 `--full-assembly-closure` / `emit-native-reference` / `emit-native-aot` 入口、pytest、canonical `run.py test subject`

**架构审核模式：** critical

**结构告警重点：** 避免在 `subject_workers.py` 复制第二套 generated emitter 链路；新增 sidecar 应复用现有 driver/manifest 结构。避免把 reporting 判断写成多个平行 helper，优先统一为 supplemental closure manifest 数据模型。

**权责图审校主题：** `SolutionCorePack` analysis/generated/reporting authority 仍归 `build/toolchains/run/testing/*`；`Chaos.IL2CPP.Driver` 只负责现有 full-closure 产物生成，不在本轮扩张新的 CLI authority。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: foundation-dll-translation-audit
- capabilityItem: corelib-supplemental-full-closure
- ownerSubjectId: subject/SolutionCorePack
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Proof
- formalVerificationObjects: [`subject/SolutionCorePack::windows-corelib-reference-native-hotupdate-proof`]
- requiredGates: collector -> registry -> workspace

**设计文档：** `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

- [ ] 修正并补齐 task 文档载体
  - 文件：`docs/dev/in-progress/20260419-20-corelib-supplemental-full-closure-audit-integration/STATUS.md`
  - 文件：`docs/dev/in-progress/20260419-20-corelib-supplemental-full-closure-audit-integration/plan-v1-01.md`
  - 文件：父 roadmap / `ACTIVE.md` / 索引
  - 目标：确保 child 合法接管执行并修正父 roadmap 子任务映射漂移

- [ ] 先写 RED 测试覆盖 supplemental closure 主链
  - 文件：`tests/unit/execution/test_subject_workers_frontend_codegen_analysis.py`
  - 文件：`tests/unit/reporting/test_subject_reporting_artifacts.py`
  - 文件：必要时 `tests/unit/run/test_test_command_workspace.py`
  - 目标：先证明当前 analysis/generated/reporting 尚未暴露真实 CoreLib supplemental closure artifacts

- [ ] 实现 supplemental CoreLib full-closure analysis/generated 接线
  - 文件：`build/toolchains/run/testing/subject_workers.py`
  - 文件：必要时 `build/toolchains/run/testing/subjects.py` 或相关 manifest helper
  - 目标：在 canonical CoreLib matrix/source 上生成 sidecar CoreLib closure、native-reference coverage 与 native-aot audit artifacts，并把路径写入 generated manifest

- [ ] 将 supplemental closure 暴露到 reporting truth boundary
  - 文件：`build/toolchains/run/testing/subject_reporting.py`
  - 目标：在 matrix report / subject summary / native-hotupdate-audit 中暴露真实 CoreLib supplemental closure 证据，但保持 `fullCoreLibTranslated = false`

- [ ] 验证并完成归档回写
  - 测试：
    - `python -m pytest tests/unit/execution/test_subject_workers_frontend_codegen_analysis.py -q`
    - `python -m pytest tests/unit/reporting/test_subject_reporting_artifacts.py -q`
    - `python -m pytest tests/unit/run/test_test_command_workspace.py -q`
    - `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject_matrices.py -q`
  - formal：
    - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - 目标：留下新的 canonical evidence，然后归档 child 并回写父 roadmap / `20260419-03`
