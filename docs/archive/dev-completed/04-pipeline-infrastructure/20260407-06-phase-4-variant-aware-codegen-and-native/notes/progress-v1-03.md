# Progress v1.03

Date: 2026-04-07 19:50:00 +08:00

## Summary

- 已把活动 Windows native reference proof host 切到 `subjects/HelloWorldObject/validation/proof/native-reference/`，并让 root `CMakeLists.txt` 只引用 subject-owned 宿主。
- 已让 subject-owned native host 正式消费 `CHAOS_SUBJECT_VARIANT`，把 `CHECK|PROFILE|SHIP` 落成 `CHAOS_VARIANT_*` 与 `CHAOS_VARIANT_NAME="..."` 编译宏。
- 已把活动 worker/tooling 测试切到 run-scoped `artifacts/subjects/<id>/runs/<run-id>/...` 路径，并补齐对应 wiki。

## Verification

- `python -B -m unittest tests.unit.run.test_repo_layout tests.tooling.run.test_test_watch_summary tests.tooling.run.test_subject_command tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf`
- `python -B build/toolchains/run/run.py test subject --id subject/HelloWorldObject --json`
  - `source-resolve -> host-input-build -> analysis-frontend -> generated-native-proof` 通过
  - xUnit validation 通过，`Passed: 13`
  - `build-target` 仍因本机缺少可用 C++ 编译器失败：`No CMAKE_CXX_COMPILER could be found`

## Next

- 继续收缩 `tests/proof/**` 兼容壳与其历史引用。
- 继续清理 `tests/` 与 `wiki/` 里剩余的旧 subject 样例名和非 run-scoped 路径。
