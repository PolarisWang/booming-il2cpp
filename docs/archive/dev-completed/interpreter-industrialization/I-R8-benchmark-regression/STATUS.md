# I-R8: Benchmark 回归管线 — 完成

> **task_id**: I-R8
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

为 Interpreter 子系统的层级性能基准测试创建了完整的回归检测管线，包括 Python pipeline 脚本和 CTest 集成。

### 产出物

| 文件 | 说明 |
|------|------|
| `build/toolchains/run/testing/interpreter_benchmark_pipeline.py` | Python pipeline 脚本，运行原生基准测试、解析 SUMMARY 输出、记录 JSONL、比较基线 |
| `testing/src/native/runtime-core/interpreter/CMakeLists.txt` | 新增 `test_interpreter_benchmark_pipeline` CTest 测试 |

### Pipeline 功能

`interpreter_benchmark_pipeline.py` 支持：

- `--record` — 将每个 scenario 的 T1/T2/T3 ns/op 记录为 JSONL 基准记录，复用现有 `benchmark_records.py` 基础设施
- `--compare` — 对每个 scenario 独立查询最新基线记录，按可配置阈值检测回归（默认 20%）
- `--threshold N` — 回归判定阈值百分比
- `--bin-dir <path>` — 指定基准测试可执行文件目录（支持自动发现）

### 覆盖场景

从 18 个基准测试 scenario 中解析 10 个层级（T1/T2/T3）时间数据：

- bench_arithmetic
- bench_register_10
- bench_native（含 T4）
- bench_multi_alu_t4（含 T4）
- bench_loc_storm_t4（含 T4）
- bench_branches_t4（含 T4）
- bench_ldstr
- bench_newobj_fields
- bench_newarr_elem
- bench_branch_combo

其余 8 个 scenario（callvirt_pic、direct_arithmetic_t4、direct_args_t4、reg_execute_t4、mixed_il_t4、mul_t4、div_rem_t4、static_fields）不产生层级 SUMMARY 数据，由 gtest 断言保证正确性。

### 技术细节

- **解析器**：基于正则表达式的输出扫描，匹配 `--- bench_<name> ---` 标题和 `SUMMARY: T1=Xns T2=Yns T3=Zns` 行，将 scenario 名称与层级数据配对
- **存储**：写入 `artifact/verification/benchmark-records/InterpreterTiering/records.jsonl`，JSONL 格式，每条记录包含设备信息、git commit、各层级 ns/op
- **回归检测**：按 scenario 别名查询最新的基线记录，逐层比较当前值与基线值，超过阈值时报告回归
- **CTest 集成**：注册为 `test_interpreter_benchmark_pipeline`，带 `benchmark;interpreter` 标签，依赖 `test_interpreter_tiering_benchmark`

### 测试结果

- 基准测试可执行文件：18/18 全部 PASS
- Pipeline 解析：10/10 scenario 正确解析并记录
- 回归比较：基线一致时无假阳性报警

### Exit Criteria

| 标准 | 状态 |
|------|------|
| Pipeline 脚本可运行并解析 SUMMARY 输出 | ✅ |
| JSONL 记录写入 benchmark store | ✅ |
| 回归检测逻辑按 per-scenario 基线比较 | ✅ |
| CTest 集成 | ✅ |
| 验证通过（18/18 基准测试 PASS，10/10 解析） | ✅ |
