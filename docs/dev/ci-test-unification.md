# CI 测试统一方案（ci_test.py）— 执行清单

> 目的：消除 CI 双轨（workflow 用 raw `dotnet test`/`ctest`，而统一的
> `scripts/ci_test.py`）—— 把每个 workflow 的 raw 测试步骤替换为
> `ci_test.py --preset <x>` 一步。改动须在 CI 环境逐个替换 + push 验证；本文件是
> turn-key 执行清单。

## 统一入口 presets（已实测 OK）

```bash
python scripts/ci_test.py --preset managed-codegen    # unit: snapshot + codegen
python scripts/ci_test.py --preset managed-full        # unit: 全部 3 个 dotnet 项目
python scripts/ci_test.py --preset native-contracts    # integration: ctest -LE benchmark|stress|soak
python scripts/ci_test.py --preset jit-native          # JIT T4 子集（test_jit_*，reference build）
python scripts/ci_test.py --preset foundation-smoke    # e2e: foundation-dll ci_smoke
python scripts/ci_test.py --preset all                 # 全部
```

> 注：`jit-native` preset（a645efc3b）已收编散落的 JIT ctest 子集。注意它如实上报
> 预存 JIT 失败（rc≠0，因 test_jit_native/il_smoke/unwind/mode/pgo 有预存 codegen bug，
> 已录入 `known-failures.integration.yaml`），接入 hard-gate 前需确认这些 JIT bug 已建模
> 或被接受。

## 逐 workflow 替换清单（按建议顺序）

> 改动后 push 触发一次 CI 验证再进下一个。已完成的清理（本 loop 先行交付）见末尾。

### 1. `aot-regression.yml`（最简，先行）
- raw: `dotnet test tests/unit/managed/snapshot/`（1 处）
- 换: `python scripts/ci_test.py --preset managed-codegen`（含 snapshot+codegen）

### 2. `foundation-dll-regression.yml`
- raw: `dotnet test snapshot` + `python -m verification.ci_smoke`（带 env）
- 换: `--preset managed-codegen`（snapshot）+ `--preset foundation-smoke`（ci_smoke）

### 3. `ci-framework.yml`（被复用 base）
- 现: `Run Snapshot Tests`(raw dotnet test) + `Run Codegen Native Tests`/`Run Common Native Tests`(ctest `-R test_jit_*`/`-R test_common`)
- 换: `--preset managed-codegen` + `--preset native-contracts` + `--preset jit-native`。保持 `inputs.run_jit_tests`/`run_fd_smoke` 控制位语义。

### 4. `codegen-regression.yml`
- 现: snapshot/codegen dotnet test + `Run Codegen Native Tests`(ctest jit 子集)
- 换: `--preset managed-full`（覆盖 snapshot+codegen+driver）+ `--preset jit-native`

### 5. `jit-baseline.yml`
- 现: jit/bench ctest 子集
- 换: `--preset managed-codegen` + `--preset jit-native`

### 6. `ci.yml`
- 现: run-foundation-dll batch runner（已迁 `tests/e2e/translation`）
- 换: `--preset foundation-smoke`

### 7. `hybrid-regression.yml` / `arm64-jit-ci.yml`
- **保留**：jit 专属 ctest 子集 / linux-arm64 交叉编译 —— 非 ci_test 范围，不需换。
  （`hybrid-regression` 的死 paths filter 与死 Configure 步骤已在 L11 清理。）

## 注意事项
1. **MetadataWriter 缺口项目**：已被移除（ci-framework/codegen-regression 的
   `dotnet test *MetadataWriter*` 死步骤 61e10cec1）。`managed-codegen` 不含它，无需再改。
2. **native Configure 死步骤**：`cd testing; cmake -S src/native`（L11 后已删 testing/）已在
   hybrid/codegen-regression/jit-baseline 修正为 `cmake -S tests/unit/runtime-native`
   （df707af94、7e3586bef）——统一时勿复用旧路径。
3. **jit-native rc≠0**：因预存 JIT codegen bug（integration baseline 已建模）；若想 hard-gate，
   需先修 JIT bug（runtime 专项），或接受其 FAIL 直至修复。

## 执行建议
在 CI 环境逐个 workflow 替换，每个 push 一次 CI 验证。从 `aot-regression` 开始（最简）。
