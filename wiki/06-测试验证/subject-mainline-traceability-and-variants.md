objectId: subject-mainline-traceability-and-variants
objectType: guide

# Subject Mainline Traceability And Variants

## 主流流程

当前 subject 主线按以下阶段展开：

1. `source-resolve`
2. `host-input-build`
3. `analysis-frontend`
4. `generated-native-proof`
5. `build-target`
6. `runtime-observe` / `runtime-perf-collect` / `runtime-trace-compare`
7. `report-assemble`

对外定位顺序：

`analysis -> codegen -> native -> report & perf`

## 产物路径

给定 `subject-id`、`run-id`、`matrix-id`：

- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/source/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/host-input/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/generated/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/build/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/runtime/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/validations/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/pipeline-report/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/subject-report/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/run-report/`

## CHECK | PROFILE | SHIP

### CHECK

- 用途：正确性验证、开发调试、问题追踪
- 宏：
  - `CHAOS_VARIANT_CHECK`
  - `CHAOS_VARIANT_NAME="CHECK"`
- native 编译特征：`MSVC=/Od /Zi`，非 MSVC=`-O0 -g`

### PROFILE

- 用途：性能分析专用
- 宏：
  - `CHAOS_VARIANT_PROFILE`
  - `CHAOS_VARIANT_NAME="PROFILE"`
- native 编译特征：`MSVC=/O2 /DNDEBUG`，非 MSVC=`-O3 -DNDEBUG`

### SHIP

- 用途：发布产物
- 宏：
  - `CHAOS_VARIANT_SHIP`
  - `CHAOS_VARIANT_NAME="SHIP"`
- native 编译特征：`MSVC=/O2 /GL /DNDEBUG /LTCG`，非 MSVC=`-O3 -DNDEBUG`

## 宏输出链路

- planner 决定 effective `variant`
- worker 把 `variant` 写入 stage manifest 与 matrix report
- `analysis-frontend` / `generated-native-proof` 记录 codegen 宏
- `build-target` 记录 `variantMacros.codegen` 与 `variantMacros.native`
- subject-owned native reference host 通过 `CHAOS_SUBJECT_VARIANT` 消费 variant，并把宏落成编译定义

## Windows Native Build 规则

- Windows subject native `build-target` 统一使用绝对 `cmake.exe`、VS developer environment、绝对 `ninja.exe` 与 `Ninja Multi-Config`
- `cmake` binary dir 在 Windows 上允许落到系统临时目录 `%TEMP%/chaos-cmake-*`；它只是 scratch 目录，不是 canonical artifact 根
- canonical 证据仍只落在 `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/build|runtime|pipeline-report|validations/`
- `subjects/<subject-id>/validation/proof/native-reference/` 中的 `native-reference.generated.cpp` 是前置生成输入；CMake 必须在缺失时立即失败，不能把它声明成 `GENERATED` 输出
- 受限沙箱中的 Windows 原生验证可能在 compiler ABI probe 阶段长时间停滞；正式端到端验证应优先在沙箱外执行，但不改变 canonical 产物目录

## 当前活动 native host

Windows reference proof 的活动宿主位于：

`subjects/HelloWorldObject/validation/proof/native-reference/`

它是唯一活动 native proof host；`tests/proof/**` 已不再保留 subject 专属 host 或 proof source。

## 最近更新

- `2026-04-07`：新增 subject 主流 traceability 与变体规则页
- `2026-04-08`：补充 Windows native `cmake + VsDevCmd + Ninja Multi-Config` 规则，以及 `CHECK|PROFILE|SHIP` 的 native 编译特征
