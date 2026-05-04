# P1 — Generator 脚本改造

## parent_task_id
foundation-dll-restructure

## source_task_id
gen-scripts

## source_relation
roadmap-child

## clearance_source
parent-roadmap

## lifecycle_status
completed

## phase
P1

## 范围

改造 4 个 Python generator 脚本，使其输出到新目录结构：
- `solution/` 目录下生成所有 managed .csproj + .sln
- `src/`、`src/patch/`、`managed_test/tests/`、`managed_test/benchmarks/` 下生成 .cs 源码
- `solution_native/` 下生成 cmake 项目
- `il2cpp_dist/` 下生成 il2cpp 产出

## 输入

- 当前 `build/toolchains/run/testing/foundation_dll/` 下的 4 个 generator 脚本
- Brainstorm 已确认的目录结构设计

## 预期产出

修改后的 4 个脚本：
- `test_code_generator.py`
- `execution_project_generator.py`
- `execution_solution_generator.py`
- `native_codegen_generator.py`

## 退出条件

- 跑 generator 后新目录结构正确
- `dotnet build solution/FoundationDllTranslationSolution.sln` 0 error 0 warning
- 只验证 1 个 family（convert-char）的全流程

## 完成摘要

### 改动文件

| 文件 | 改动内容 |
|------|---------|
| `test_code_generator.py` | 输出路径改为 src/, src/patch/, managed_test/；_generated_source 合并 benchmark/host/test 方法；新增 _patch_handwritten_source、_test_exe_source、_benchmark_exe_source |
| `execution_project_generator.py` | 输出 csproj 到 solution/；4 个类型 (src/patch/tests/benchmarks)；Compile Include 引用 verification 目录源码 |
| `execution_solution_generator.py` | 输出 sln 到 solution/；添加 solution folders (NestedProjects) |
| `native_codegen_generator.py` | generate_native_skeleton 输出到 il2cpp_dist/；generate_benchmark_native_entry 输出到 native_test/benchmark/；generate_hotupdate_native_skeleton 输出到 native_test/hotupdate/；generate_benchmark_managed_bodies 输出到 managed_test/benchmarks/ |
| `gap_analyzer.py` | sln 路径更新为 solution/FoundationDllTranslationSolution.sln |
| `.gitignore` | 添加 solution/ solution_native/ il2cpp_dist/ reports/ |

### 验证结果

- `dotnet build solution/FoundationDllTranslationSolution.sln` → **0 error, 0 warning**
- 4 个 project 全部编译通过：ConvertChar (Library), ConvertChar.Patch (Library), ConvertChar.Tests (Exe), ConvertChar.Benchmarks (Exe)
- 生成文件分布在: src/, src/patch/, managed_test/tests/, managed_test/benchmarks/, il2cpp_dist/, native_test/, solution/, solution/*/bin/obj/

### 下一步

P2 migrate-files: 迁移现有文件到新位置，删除旧目录
