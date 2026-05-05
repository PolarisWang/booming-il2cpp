---
name: dev-foundation-dll-verification-pipeline
description: Master orchestrator for foundation DLL verification — codegen → data integrity → three-gate → aggregation → dashboard refresh. TRIGGER when running batch_native_aot_runner, verifying capability families, or refreshing foundation-dll dashboard.
---

# Foundation DLL Verification Pipeline

## 概述

完整的 foundation DLL 验证管线编排。从 codegen 管线开始，经过数据校验、三闸门验证、结果聚合到 dashboard 刷新。

**入口信号**：`dev:foundation-dll-verification-pipeline`

## 前置条件

- 目标 family 的 managed 代码已实现（翻译路径已写入 codegen）
- 当前目录为仓库根目录

## 管线步骤

```
Step 0: Codegen Pipeline（如未运行）
  │  运行 batch_native_aot_runner.py
  │  ├── 为每个 family 生成 entrypoint C#
  │  ├── chaos-il2cpp convert → aot-core-ir.json
  │  ├── chaos-il2cpp emit-native-aot → native-aot.generated.cpp
  │  └── native-codegen → native-reference.runtime-skeleton.coverage.json
  │
  │  ⚠ 必须确认 coverage JSON 已生成，否则 Step 0 的 integrity check 会失败
  ▼
Step 1: Data Integrity
  │  dev:foundation-dll-verify-data-integrity
  │  ├── Claims 完整性（methodSubjectIds）
  │  ├── Claims vs Ledger 一致性
  │  ├── Coverage Evidence 存在性
  │  └── Dashboard Gate 状态一致性
  │
  │  ⚠ 如果 coverage 缺失 → 回退 Step 0
  │  ⚠ 如果 claims 为空 → regenerate → 重试
  ▼
Step 2: Select Families
  │  根据验证目的选择 family 列表：
  │  - 基准回归: convert-char, math-numerics
  │  - 完整验证: 所有 33 families
  │  - 定点验证: <按需指定>
  ▼
Step 3: Three-Gate Verification (per family)
  │  dev:foundation-dll-family-verification
  │  ├── Fact L1:    Codegen success (AOT lowering compiled)
  │  ├── Fact L2:    Semantic correctness (native exec checksum match)
  │  ├── Benchmark:  native exe + managed baseline
  │  └── HotUpdate:  native hotupdate exe
  │
  │  ⚠ 任何闸门失败 → 先查看 trace 定位根因，再决定继续还是停止
  │  ⚠ `dev:systematic-debugging` + `run trace` 是调试闸门失败的首选手段
  ▼
Step 4: Aggregate & Report
  │  dev:foundation-dll-verify-aggregate
  │  ├── 收集各 family 结果
  │  ├── 检测回归信号
  │  ├── 生成汇总表
  │  └── 刷新 Dashboard
  ▼
Step 5: Final Verification
  │  dev:verification-before-completion
  │  确认 Dashboard 数据正确反映验证结果
  │  确认 ledger 已更新（如适用）
  │  输出最终状态报告
```

## 前置准备：Codegen Pipeline

如果目标 family 尚未运行 codegen 管线，先补跑：

```bash
cd <REPO_ROOT>
python build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py \
  --assembly-name System.Private.CoreLib \
  --families convert-char array-indexing-copy ... \
  --trace
```

然后确认 coverage JSON 已生成：

```bash
ls verification/foundation-dll/System.Private.CoreLib/<family>/il2cpp_dist/native-reference.runtime-skeleton.coverage.json
```

## 参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `assembly` | 目标 assembly | `System.Private.CoreLib` |
| `families` | family 列表（逗号分隔） | 全部（自动发现） |
| `skip_codegen` | 跳过 codegen 步骤 | `false` |
| `skip_integrity` | 跳过 Step 1 | `false` |
| `skip_aggregate` | 跳过 Step 4-5 | `false` |

## 选择 Family 原则

| 目的 | 推荐组合 | 数量 |
|------|---------|------|
| 快速回归检查 | `convert-char, math-numerics, string-char-text-core` | 3 |
| 完整验证 | 所有 33 families | 33 |
| 定点调试 | 按需指定 | 1-N |

## 失败处理

| 阶段 | 失败模式 | 处理方式 |
|------|---------|----------|
| Step 0 | codegen 失败 | 先 `run trace --exception` 查看失败原因，修复后重跑 |
| Step 1 | claims 为空 | `write_foundation_dll_audit_outputs()` regenerate → 重试 |
| Step 1 | coverage 缺失 | Step 0 未完成 → 回退 codegen |
| Step 3 | Fact L1 失败（无 AOT lowering） | 标记为 "not-translated"，该 family AOT 翻译未完成，不阻塞其他 family |
| Step 3 | Fact L2 失败（native exec checksum 不匹配） | **阻塞** → **查看 trace** → `dev:systematic-debugging` → 确认翻译语义错误 |
| Step 3 | Benchmark 退化 | **查看 trace** → 排查翻译质量 → 标记 regression |
| Step 3 | HotUpdate 失败 | **查看 trace** → 检查 stub 返回值 → 标记失败 |
| Step 4 | Dashboard 不一致 | `write_foundation_dll_audit_outputs()` 重新生成 |

### 闸门失败标准流程

任何闸门失败时：

```bash
# 1. 先查 trace
python -m build.toolchains.run.commands.trace_viewer --exception

# 2. 分析根因（使用 dev:systematic-debugging 方法论）
# 3. 修复后重跑
# 4. 确认修复后再继续下一 family
```

## 职责边界

| 技能 | 负责 | 不负责 |
|------|------|--------|
| `codegen pipeline` | 运行 batch_native_aot_runner | 验证、数据完整性 |
| `verify-data-integrity` | 验证前数据检查 | 修复数据（仅提供修复命令） |
| `family-verification` | 单 family 三闸门 | 数据完整性、跨 family 聚合 |
| `verify-aggregate` | 结果汇总、回归检测、dashboard 刷新 | 单 family 验证执行 |
| `verification-pipeline` | 编排、参数传递、流程控制 | 具体验证执行逻辑 |

## 关联

- 前置: 翻译路径实现完成
- Step 0: `batch_native_aot_runner.py` + native codegen
- Step 1: `dev:foundation-dll-verify-data-integrity`
- Step 3: `dev:foundation-dll-family-verification`
- Step 4: `dev:foundation-dll-verify-aggregate`
- Step 5: `dev:verification-before-completion`
