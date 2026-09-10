# T0-1: string_stubs 单元测试

> **parent_task_id**: 20260523-runtimesupport-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **entry_skill**: writing-plans
> **lifecycle_status**: in-progress
> **phase**: Phase 0
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true
> **preflight_review**: pass
> **auto_execution_decision**: continue

## Scope

为 `runtime_stubs/string_stubs.cpp` 所有 15 个 public API 函数编写单元测试：
- ChaosStringContains, ChaosStringStartsWith, ChaosStringEndsWith
- ChaosStringIndexOf, ChaosStringCompare
- ChaosStringJoinSs, ChaosStringSplit
- ChaosStringSubstring, ChaosStringRemove
- ChaosStringReplace, ChaosStringToLower, ChaosStringToUpper
- ChaosStringTrim, ChaosStringFastAllocate
- ChaosStringFormatArray, ChaosStringFormat1, ChaosStringFormat2
- ChaosStringAppend, ChaosFormattablestringFactoryCreate

## Inputs

- `src/native/runtime-core/runtime_stubs/string_stubs.cpp` — 待测实现
- `src/native/runtime-core/runtime_stubs/stub_common.h` — StubStringHeader/StubArrayHeader
- `src/native/runtime-core/runtime_stubs/string_stubs.h` — 声明
- `testing/src/native/runtime-core/CMakeLists.txt` — 注册测试目标
- `testing/src/native/runtime-core/metadata/metadata_test_stubs.cpp` — 参考测试 stub 模式

## Expected Outputs

- `testing/src/native/runtime-core/runtime_stubs/string_stubs_test.cpp` — 测试文件
- `testing/src/native/runtime-core/runtime_stubs/CMakeLists.txt` — 测试构建
- `testing/src/native/runtime-core/CMakeLists.txt` — 注册子目录

## Exit Criteria

- string_stubs_test 在 CHECK 配置下全部通过
- 每个 API 至少覆盖：正常输入、边界值（空字符串/超大输入）、错误输入（nullptr）
- 测试验证返回值的语义正确性（不仅是"不崩溃"）
