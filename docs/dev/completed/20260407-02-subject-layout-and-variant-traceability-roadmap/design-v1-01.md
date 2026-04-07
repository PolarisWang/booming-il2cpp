# Subject Structure And Variant Traceability Design v1.01

Date: 2026-04-07
Status: design-frozen-for-roadmap

## 1. 设计目标

本设计把仓库结构、subject 执行脊柱、validation 模型和 native variant 语义收敛成一套可迁移的目标状态。核心原则是：

- `subject` 是唯一正式测试对象
- `artifacts/subjects/<id>` 是唯一 subject 产物真源
- `variant` 从 `codegen` 开始成为一等执行轴
- `tests/` 与 `subjects/` 责任彻底分离

## 2. 目标仓库结构

### 2.1 顶层职责

- `src/`
  - 仅保留 il2cpp 核心代码
- `tests/`
  - 仅保留通用测试脚本、公共 fixture、共享 harness、仓库级校验逻辑
- `subjects/`
  - 作为正式测试工程根，承载 subject 自身源码与验证资产
- `contracts/`
  - 保留 canonical schema / headers / docs / global samples
- `artifacts/subjects/`
  - 保留 subject 运行过程中的全部中间产物、日志和 run ledger

### 2.2 subject 目录模型

```text
subjects/<subject-id>/
  subject.manifest.json
  source/
  validation/
    unit/
      <Subject>.UnitTests/
    perf/
      <Subject>.PerfHarness/
    proof/
  expected/
    analysis/
    codegen/
    runtime/
  baselines/
    perf/
```

说明：

- `validation/unit` 使用 `xUnit`
- `validation/perf` 使用 C# perf harness
- `validation/proof` 保留与 pipeline proof 相关的 subject 侧资产
- `expected/` 与 `baselines/` 保留 subject 自己的期望输出和 perf 基线，不再放入 `tests/`

## 3. manifest 模型

`subject.manifest.json` 不再只靠 `defaultGoal + defaultMatrix` 表达全部语义，而是显式表达 validation 与 profile：

```json
{
  "source": {
    "type": "dotnet-project",
    "path": "subjects/<id>/source/<Subject>.csproj"
  },
  "defaultValidationProfile": "dev-fast",
  "validationProfiles": {
    "dev-fast": ["proof", "unit"],
    "perf-gate": ["proof", "unit", "perf"]
  },
  "validation": {
    "proof": {
      "defaultVariant": "CHECK"
    },
    "unit": {
      "project": "subjects/<id>/validation/unit/<Subject>.UnitTests/<Subject>.UnitTests.csproj",
      "framework": "xunit",
      "defaultVariant": "CHECK"
    },
    "perf": {
      "project": "subjects/<id>/validation/perf/<Subject>.PerfHarness/<Subject>.PerfHarness.csproj",
      "driver": "csharp-perf-harness",
      "defaultVariant": "PROFILE"
    }
  },
  "expected": {
    "analysis": "subjects/<id>/expected/analysis/",
    "codegen": "subjects/<id>/expected/codegen/",
    "runtime": "subjects/<id>/expected/runtime/"
  },
  "baselines": {
    "perf": "subjects/<id>/baselines/perf/"
  }
}
```

设计约束：

- `validation` 采用 typed + minimal，不提前塞未来配置
- `validationProfiles` 用于声明 bundle，不借 `goal` 承载
- `goal` 仅保留兼容层职责

## 4. CLI 与选择模型

正式主轴改为：

- `--validation-profile`
- `--validation`
- `--variant`
- `--matrix`

统一入口保持为：

```text
run test subject --id subject/<subject-id>
```

默认行为：

- 默认执行 manifest 中的 `defaultValidationProfile`
- `validation` 决定默认 `variant`
- 允许专家模式显式覆盖 `--variant`

兼容策略：

- `--goal` 暂时作为旧接口兼容层保留
- 但不再作为公开设计主轴

## 5. artifact 与 run ledger 模型

subject canonical 产物全部按 run 组织：

```text
artifacts/subjects/<subject-id>/
  runs/
    current.json
    last.json
    <run-id>/
      analysis/
      variants/
        CHECK/
          codegen/
          matrices/
            windows-x64/
              native/
              pipeline-report/
              validations/
                proof/
                unit/
        PROFILE/
          codegen/
          matrices/
            windows-x64/
              native/
              pipeline-report/
              validations/
                perf/
```

设计含义：

- `analysis/` 是单次 run 的共享层
- `variant` 从 `codegen` 开始分叉
- `native / pipeline-report / validations` 继续按 `matrix` 分层
- `pipeline-report/` 只描述主流程产物
- `validations/<kind>/` 分别保存 proof、unit、perf 的结果、日志、samples、baseline compare

## 6. variant 语义

`CHECK / PROFILE / SHIP` 是正式构建变体，而不是单纯的输出标签：

- `CHECK`
  - 默认服务于 `proof` 与 `unit`
  - 保留调试与可追踪能力
- `PROFILE`
  - 默认服务于 `perf`
  - 关闭多余调试输出，保留性能分析所需能力
- `SHIP`
  - 默认服务于发布用 native 产物
  - 追求最高优化，去除 debug 信息

设计要求：

- codegen 阶段必须生成对应变体的宏定义
- native 阶段必须承接同一组变体宏定义与编译参数
- 变体宏清单最终需要写入 `wiki/`

## 7. contracts 与 samples 语义

- `contracts/` 继续保存 canonical schema / docs / headers / global samples
- `contracts/examples` 命名统一改为 `samples`
- `analysis/contracts/examples` 不再作为长期真源
- 运行期真实产物一律进入 `artifacts/subjects/<id>/runs/<run-id>/...`

## 8. 非目标

本设计当前不直接定义：

- 具体代码实现细节
- 每个 stage worker 的完整 I/O schema
- 所有旧命令的最终删除时间点
- wiki 页面最终拆分目录

这些内容应在 roadmap 派生出的 child plan 中逐步落地。
