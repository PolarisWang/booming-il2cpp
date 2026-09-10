# 进度记录 v1-03

## 时间

- 2026-04-05 23:32:04 +08:00

## 已完成

- 按 TDD 先补齐失败测试，锁定了 `tests/contracts` canonical 路径、`src/managed/Chaos.IL2CPP.*` skeleton、`tests/proof` 与 `tests/smoke` 的角色分离。
- 建立 `src/managed/Chaos.IL2CPP.Contracts / Driver / Loader / SemanticWorld / Pipeline / Linker / MetadataWriter / CodeGen` 8 个 managed 项目骨架，并落下最小 project reference 规则。
- 在 `Chaos.IL2CPP.Contracts` 中放入 shared contract model 入口 `SubjectId / ContractSubject`。
- 新建 `tests/proof/input/HelloWorldObject/HelloWorldObject.csproj` 与 `tests/proof/hello-world-object/suite.manifest.json`，把 object-based proof 输入从 smoke 树独立出来。
- 把 `CMakeLists.txt`、`build/toolchains/run/commands/build.py`、`build/toolchains/run/commands/test.py`、`build/toolchains/run/run_manifest.json`、`build/toolchains/run/testing/contracts.py`、`build/scripts/verify-roadmap-0.py` 等入口切到 `tests/contracts` canonical 路径。
- 更新 wiki，明确 `tests/contracts` 是 canonical contract 根目录，`tests/contract` 仅作为 compatibility 层保留。

## 验证

- `python -m unittest tests.tooling.run.test_build tests.tooling.run.test_build_batch tests.tooling.run.test_trace_compare tests.tooling.run.test_platform_gate_validate_only tests.tooling.run.test_command_manifest tests.tooling.run.test_contract_paths tests.unit.run.test_stage1b_layout`
- `python -m unittest tests.tooling.run.test_verify_roadmap0 tests.tooling.run.test_contract_check`
- `dotnet build` 全量 `src/managed/**/*.csproj`
- `dotnet build tests/proof/input/HelloWorldObject/HelloWorldObject.csproj -c Release`

## 当前状态

- 任务状态：`completed`
- 当前阶段：`archived`

## 结论

- Stage 1B 的 managed skeleton、tests family 角色分离和 canonical path 迁移已完成。
- `tests/contracts` 现在是 run/build/verify/tooling 的权威 contract 路径。
- `tests/proof` 已作为独立 proof 角色落地，后续 Stage 2 可以直接在 `HelloWorldObject` 上继续冻结 proof spec。
