# Phase 4 MainlineFeaturePack Onboarding 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划，并按 TDD 先补失败测试，再推进实现。
> **目标：** 把 planning-only `MainlineFeaturePack` 接成真实完整 subject，形成 `correctness.dev` / `correctness.platform` / `perf.profile` 三条统一入口。
> **架构：** 先补 RED 锁定 subject tree、trace export 和 subject-scoped proof build，再补 source / unit / proof / baseline，最后跑 focused verification。
> **技术栈：** C#、xUnit、Python unittest、CMake、MSBuild
> **设计文档：** `docs/dev/completed/20260409-06-phase-4-mainline-feature-pack-onboarding/design-v1-01.md`
> **预期知识沉淀：** 暂不写 wiki；待 `MainlineFeaturePack` 与 Batch A 稳定后统一沉淀完整 subject onboarding 规则。

## 目标文件与职责

- `tests/unit/run/test_phase4_mainline_feature_pack_onboarding.py`
  - 锁定 Phase 4 onboarding 的 RED/GREEN 边界
- `subjects/MainlineFeaturePack/subject.manifest.json`
  - 从 planning-only 调整为真实 onboarding surface
- `subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj`
- `subjects/MainlineFeaturePack/source/Program.cs`
  - 提供真实 source tree 与 trace export CLI
- `subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/*`
  - subject-owned xUnit 验证
- `subjects/MainlineFeaturePack/validation/proof/native-reference/*`
  - subject-owned native reference host
- `subjects/MainlineFeaturePack/baselines/perf/windows-native-profile/windows.json`
  - native perf baseline 入口
- `build/toolchains/run/testing/subject_workers.py`
  - build-target 改为传入 subject-scoped proof root
- `CMakeLists.txt`
  - `windows-x64-reference` 路径下支持 `CHAOS_SUBJECT_PROOF_ROOT`

## Chunk 1：RED

- [ ] 新增 `tests.unit.run.test_phase4_mainline_feature_pack_onboarding`
- [ ] 运行 `python -m unittest tests.unit.run.test_phase4_mainline_feature_pack_onboarding`
- [ ] 确认失败点集中在真实 source / unit / proof 缺失与 proof build 仍然硬编码 `HelloWorldObject`

## Chunk 2：MainlineFeaturePack 真实目录接入

- [ ] 新增 `source/MainlineFeaturePack.csproj`
- [ ] 新增 `source/Program.cs`
- [ ] 调整 `subject.manifest.json`，补齐 `proof-dev` / `perf-profile` 的真实 validation surface
- [ ] 新增 `validation/unit/MainlineFeaturePack.Subject.UnitTests/`
- [ ] 新增 `validation/proof/native-reference/`
- [ ] 新增 `baselines/perf/windows-native-profile/windows.json`

## Chunk 3：subject-scoped proof build

- [ ] 根 `CMakeLists.txt` 支持 `CHAOS_SUBJECT_PROOF_ROOT`
- [ ] `_windows_subject_build(...)` 传入当前 subject proof root
- [ ] 保持 `HelloWorldObject` 默认路径兼容

## Chunk 4：验证与状态回写

- [ ] 跑 `python -m unittest tests.unit.run.test_phase4_mainline_feature_pack_onboarding`
- [ ] 跑 `dotnet test subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj -c Release`
- [ ] 跑 `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --validation-profile proof-dev --variant CHECK --json`
- [ ] 跑 `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.platform --validation-profile trace-platform --variant CHECK --json`
- [ ] 跑 `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`
- [ ] 回写 child / parent task 状态

## 验证命令

- [ ] `python -m unittest tests.unit.run.test_phase4_mainline_feature_pack_onboarding`
- [ ] `dotnet test subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj -c Release`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --validation-profile proof-dev --variant CHECK --json`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.platform --validation-profile trace-platform --variant CHECK --json`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`

## 完成标准

- [ ] `subjects/MainlineFeaturePack` 不再只有 manifest，而是具备真实 `source / validation / baselines` 目录
- [ ] source 支持 warmup trace export，`windows-reference-trace` 可真实执行
- [ ] proof build 不再硬编码 `HelloWorldObject`
- [ ] `correctness.dev` / `correctness.platform` / `perf.profile` 三条入口可执行
- [ ] focused verification 通过
