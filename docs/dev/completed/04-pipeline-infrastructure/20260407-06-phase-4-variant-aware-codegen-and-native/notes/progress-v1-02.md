# Progress v1.02

Date: 2026-04-07 18:33:00 +08:00

## Summary

- 已把 `build/toolchains/run/testing/subject_workers.py` 中残留的 `CHAOS_HELLOWORLD_*` 与 `chaos_stage4_hello_world_object_proof*` 改成通用 `CHAOS_SUBJECT_*` 与 `chaos_subject_reference_proof(_run)`。
- 已同步更新 `tests/proof/native-reference/HelloWorldObject/CMakeLists.txt`，避免 native reference 入口继续把具体样例名暴露成 worker 约定。
- 已把 `tests.unit.run.test_subject_workers` 与 `tests.unit.run.test_subject_workers_perf` 改成通用 fixture，断言只验证链路语义与目录约束，不再依赖真实 subject 工程名。

## Verification

- `python -m unittest tests.unit.run.test_subject_workers`
- `python -m unittest tests.unit.run.test_subject_workers_perf`

## Next

- 继续清理 `tests/` 中其他新引入的真实样例名，优先处理 `test_path_resolver`、`test_subject_planner`、`test_subject_executor`、`test_subject_reporting`。
- 继续复核 tooling 入口测试，把 registry/command/tui 中仅用于样例展示的真实 subject 名替换成通用 fixture 或查询链路。
