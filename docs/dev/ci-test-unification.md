# CI 测试统一方案（ci_test.py）— 映射参考

> 目的：消除 CI 双轨（7 个 workflow 用 raw `dotnet test`/`ctest`，而统一的
> `scripts/ci_test.py` 已被引用）。本表把每个 workflow 的 raw 测试步骤映射到
> `ci_test.py --preset <x>`，供 CI 环境执行时直接套用。
>
> 验证状态：`ci_test.py` 已实测 OK（`--preset managed-codegen`、`managed-full` 均
> delegate test_driver、exit 0、known 基线正确 gate）。workflow YAML 改动须在 CI
> 环境确认（本地无 CI runner）。

## 入口

```bash
python scripts/ci_test.py --preset managed-codegen   # unit: snapshot + codegen
python scripts/ci_test.py --preset managed-full       # unit: 全部 3 个 dotnet 项目
python scripts/ci_test.py --preset native-contracts   # integration: ctest -LE benchmark|stress|soak
python scripts/ci_test.py --preset foundation-smoke   # e2e: foundation-dll ci_smoke
python scripts/ci_test.py --preset all                # 全部
```

## Workflow → preset 映射

| workflow | 当前 raw 测试步骤 | 应换成的 preset（或保留原因） |
|----------|------------------|------------------------------|
| `ci.yml` | run-foundation-dll batch（已迁 `tests/e2e/translation`） | `foundation-smoke` |
| `ci-framework.yml` | 2× `dotnet test`(snapshot, MetadataWriter·缺口) + native ctest | `managed-codegen` + `native-contracts` |
| `codegen-regression.yml` | `dotnet test`(snapshot, codegen, MetadataWriter·缺口) + jit ctest | `managed-full` + jit 专属 ctest 子集 |
| `foundation-dll-regression.yml` | snapshot + `python -m verification.ci_smoke` | `managed-codegen` + `foundation-smoke` |
| `aot-regression.yml` | `dotnet test snapshot` | `managed-codegen` |
| `jit-baseline.yml` | jit/bench ctest 子集 | `managed-codegen` + jit 专属 ctest 子集 |
| `hybrid-regression.yml` | ctest jit 子集 | 保留 ctest 子集（jit 专属） |
| `arm64-jit-ci.yml` | linux-arm64 ctest | 保留（交叉编译专属,非 ci_test 范围） |

## 注意事项
1. **MetadataWriter 缺口项目**：部分 workflow 对它 `dotnet test`,但 suite_contract
   已注明它缺 Test.Sdk 无法真正运行（known gap）——统一到 `managed-codegen`（不含它）后，
   应删除这些 raw 步骤,避免 CI 跑一个必然失败的缺口项目。
2. **jit/arm64 子集**：`test_jit_*` ctest 子集是专门的（T4 JIT 验证）,`ci_test.py`
   目前无对应 preset；可新增 `jit-native` preset 或保留这些 raw ctest 步骤。
3. **cfi framework 复用**：`ci-framework.yml` 是被复用的 base workflow,统一时需保持其
   `inputs.run_jit_tests`/`inputs.run_fd_smoke` 控制位语义。

## 执行建议
在 CI 环境逐个 workflow 替换,每个替换后 push 触发一次 CI 验证,再进下一个。
先从受影响最小的 `aot-regression.yml` / `foundation-dll-regression.yml` 开始。
