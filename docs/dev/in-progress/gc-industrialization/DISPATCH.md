# DISPATCH.md — GC 工业化极致计划 调度总纲

## 调度模型

**`dispatch_model: hybrid`**

- **batch-1**: 并行（11 子任务，两轨互不依赖）
- **batch-2**: 串行（G-P1..G-P5，强依赖链）

**`parallel_merge_policy: independent-completed`**

## 并行能力

- 当前可用终端数：1（单人执行）
- batch-1 并行策略：单终端执行时，**每半天切换 Track**（上午 Track A / 下午 Track B），以周为周期轮换主导方向（如本周以 A 为主，下周以 B 为主）。STATUS.md 与 design-v1-01.md 的风险表均按此节奏描述。
- 策略说明：batch-1 设计为并行就绪，但单人执行时实际串行。如需加速，可分配更多终端给 batch-1 的独立子任务。

## Batches

### batch-1: 验证基建 + A3 设计（并行组）

| task_id | mode | description | conflict_check | wait_for |
|---------|------|-------------|:--------------:|:--------:|
| T-A1 | parallel | Stress 测试进 CI 门禁 | no-shared-output | - |
| T-A2 | parallel | ASAN per-PR 门禁 | no-shared-output | - |
| T-A3 | parallel | TSAN 并发测试 CI | no-shared-output | - |
| T-A4 | parallel | Release 性能基线 | no-shared-output | - |
| T-A5 | parallel | known-fail 数据库带 expiry | no-shared-output | - |
| T-A6 | parallel | Cross-test 全域态污染检测 | no-shared-output | - |
| T-B1 | parallel | 读 CoreCLR WKS safepoint 源码 | no-shared-output | - |
| T-B2 | parallel | 设计单 region 分配器 | no-shared-output | - |
| T-B3 | parallel | 设计 LEAF 汇编写屏障 | no-shared-output | - |
| T-B4 | parallel | 设计 codegen/JIT 接口契约 | no-shared-output | - |
| T-B5 | parallel | 设计 forbid_suspend 废弃方案 | no-shared-output | - |

**wait_for**: 无
**merge_after**: all-completed
**冲突说明**: T-A 系列改 `ci/*.yml` / `tests/`，T-B 系列写 `docs/`，无文件冲突

### batch-2: 实施（串行组）

| task_id | mode | description | conflict_check | wait_for |
|---------|------|-------------|:--------------:|:--------:|
| G-P1 | sequential | A2b A3 实现 | no-shared-output | T-A1..T-B5 |
| G-P2 | sequential | 残余并发 bug 修复 | no-shared-output | G-P1 |
| G-P3 | sequential | 诊断/配置完备 | no-shared-output | G-P2 |
| G-P4 | sequential | 性能/平台对齐 | no-shared-output | G-P3 |
| G-P5 | sequential | 差异化优势 | no-shared-output | G-P4 |

**wait_for**: batch-1
**merge_after**: all-completed