# 测试框架重构与升级 Design v1.02

Date: 2026-04-06
Status: design-discuss

## 1. 目标

本次重构不是继续优化现有 `run test` 外壳，而是把测试框架重心从旧的：

- `suite`
- `module`
- `system`
- `pipeline`

切换到新的：

- `subject`
- `goal`
- `artifact-chain`
- `environment-matrix`

目标是让测试体系直接服务 IL2CPP 主线的：

1. 开发环境正确性验证
2. 多平台正确性与平台适配性验证
3. 开发环境性能测试
4. 发布产物性能报告

## 2. 核心原则

### 2.1 双主轴

第一主轴固定为测试目标：

- `correctness.dev`
- `correctness.platform`
- `perf.dev`
- `perf.release`

第二主轴固定为产物链阶段：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

### 2.2 `subject` 是唯一中心模型

后续所有正式测试对象统一由 `subject` 表达。

`subject` 可以是：

- canonical proof 输入
- 调试定位项目
- benchmark 项目

旧 `suite/module/system/pipeline` 不再作为正式主模型保留。

### 2.3 硬切换

当前已确认采用硬切换路线：

- CLI 主语法切到 `subject`
- 旧 `suite/module/system/pipeline` 公开命令直接移除
- registry 改为 `subjects` 为唯一正式注册源
- 文档主入口同步重写

本次重构不是平滑过渡，不设计长期双轨期。

## 3. 目录模型

### 3.1 顶层输入对象

```text
subjects/
  <subject-id>/
    subject.manifest.json
    source/
```

### 3.2 统一产物目录

```text
artifacts/subjects/<subject-id>/
  source/
  host-input/
  analysis/
  generated/
  build/
  runtime/
  report/
```

这条产物链对应用户提出的目标分层：

- 原项目 -> `source`
- dotnet/mono 环境产物 -> `host-input`
- IL2CPP 分析产物 -> `analysis`
- 生成产物 -> `generated`
- build 产物 -> `build`
- 运行结果 / trace / stdout / exit code -> `runtime`
- 发布与性能汇总 -> `report`

## 4. `subject.manifest.json` 第一版模型

## 4.1 最小字段集合

第一版最小字段集合固定为：

- `subjectId`
- `displayName`
- `category`
- `defaultGoal`
- `defaultMatrix`
- `source`
- `tags`
- `expectedArtifacts`
- `environmentMatrices`

## 4.2 `category`

第一版固定为：

- `canonical`
- `diagnostic`
- `benchmark`

辅助语义通过 `tags` 表达，例如：

- `proof`
- `smoke`
- `regression`
- `debuggable`
- `interop`
- `trace`

## 4.3 `source`

第一版 `source.type` 支持：

- `dotnet-project`
- `managed-assembly`
- `native-project`
- `source-file`

## 4.4 `expectedArtifacts`

第一版固定为阶段桶，而不是具体文件名：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

## 4.5 `environmentMatrices`

单个 `subject.manifest.json` 允许声明多条 matrix。

每条 matrix 至少包含：

- `matrixId`
- `executionContext`
- `validationIntent`

### `executionContext`

- `hostPlatform`
- `targetPlatform`
- `toolchainProfile`
- `runtimeProfile`

### `validationIntent`

- `validationMode`
- `adaptationLevel`
- `expectedOutcome`

## 5. 环境矩阵语义

## 5.1 `validationMode`

第一版固定为：

- `schema`
- `compile-only`
- `build`
- `startup`
- `output`
- `trace`
- `perf`
- `report`

它表示“本次验证要收集什么证据”。

## 5.2 `adaptationLevel`

第一版固定为：

- `unsupported`
- `compile-only`
- `buildable`
- `startup`
- `observable-output`
- `traceable`
- `perf-reportable`

它表示“目标平台当前被适配到什么成熟度”。

## 5.3 `expectedOutcome`

第一版固定为：

- `pass`
- `fail`
- `known-fail`
- `not-applicable`

它表示设计期预期，不直接等同运行态结果。

## 6. 命名重构

## 6.1 `managed-closure` 退役

后续不再继续使用 `managed-closure` 作为长期语义名。

替换为双层表达：

- 模块责任：`frontend-pipeline`
- 关键产物角色：`proof-input-bundle`

### 这两个名称分别表示什么

- `frontend-pipeline`
  - 表示从 host-input 出发，经 Loader、SemanticWorld、Linker、MetadataWriter、CodeGen、Driver 生成前端产物的职责链
- `proof-input-bundle`
  - 表示这组前端产物是后续 native proof 的唯一正式输入包

两者分别解决“谁负责生成”和“下游如何消费”两个问题。

## 7. CLI 模型

正式主语法固定为：

```text
run test subject <subject-id>
run test subject <subject-id> --matrix <matrix-id>
run test subject <subject-id> --goal <goal-id>
```

## 7.1 默认选择规则

当未显式指定 `--matrix` 时：

- 使用 `subject.manifest.json` 中的 `defaultMatrix`

当未显式指定 `--goal` 时：

- 使用 `subject.manifest.json` 中的 `defaultGoal`

## 7.2 默认值缺失规则

当 subject 存在多个 matrix 或多个 goal，但未声明对应默认值时：

- 直接视为配置错误

框架不做隐式推断，也不自动回退到“跑全部”。

## 8. registry 模型

正式 registry 统一扫描：

```text
subjects/**/subject.manifest.json
```

不再保留现有：

- `tests/registry/modules/**`
- `tests/registry/system/**`
- `tests/registry/pipelines/**`

作为正式注册源。

新的 registry 应输出的核心视图是：

- subject 列表
- matrix 列表
- goal 列表
- subject 与 artifact bucket 的关系
- subject 与 host/target/toolchain 的关系

## 9. 文档模型

原 `wiki/06-测试验证/` 的主入口结构后续需要重写。

新的项目级测试文档入口建议改成：

- `subjects`
- `goals`
- `artifact-chain`

对应说明：

- `subjects`：稳定被验证对象
- `goals`：四个顶层测试目标
- `artifact-chain`：统一产物链与诊断方法

## 10. `HelloWorldObject` 示例

下面是一版符合当前设计的 `subject.manifest.json` 草案：

```json
{
  "subjectId": "HelloWorldObject",
  "displayName": "HelloWorldObject",
  "category": "canonical",
  "defaultGoal": "correctness.platform",
  "defaultMatrix": "windows-x64-reference-output",
  "source": {
    "type": "dotnet-project",
    "path": "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
    "entry": "HelloWorldObject/Program::Main(System.String[])"
  },
  "tags": ["proof", "object", "debuggable"],
  "expectedArtifacts": [
    "source",
    "host-input",
    "analysis",
    "generated",
    "build",
    "runtime"
  ],
  "environmentMatrices": [
    {
      "matrixId": "windows-x64-reference-output",
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "windows-x64",
        "toolchainProfile": "msvc-reference",
        "runtimeProfile": "native-reference"
      },
      "validationIntent": {
        "validationMode": "output",
        "adaptationLevel": "observable-output",
        "expectedOutcome": "pass"
      }
    },
    {
      "matrixId": "android-arm64-compile-only",
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "android-arm64",
        "toolchainProfile": "android-ndk",
        "runtimeProfile": "compile-gate"
      },
      "validationIntent": {
        "validationMode": "compile-only",
        "adaptationLevel": "compile-only",
        "expectedOutcome": "pass"
      }
    },
    {
      "matrixId": "macos-reference-trace",
      "executionContext": {
        "hostPlatform": "macos-arm64",
        "targetPlatform": "macos-arm64",
        "toolchainProfile": "clang-reference",
        "runtimeProfile": "native-reference"
      },
      "validationIntent": {
        "validationMode": "trace",
        "adaptationLevel": "traceable",
        "expectedOutcome": "known-fail"
      }
    }
  ]
}
```

## 11. 实施分期建议

虽然整体路线是硬切换，但执行上仍建议分期。

### Phase 1：建新骨架

- 新建 `subjects/`
- 定义 `subject.manifest.json` schema
- 新建 subject-based registry
- 新建 `run test subject ...` 入口

### Phase 2：迁移主线对象

- 首批只迁移 `HelloWorldObject`
- 让 Stage 3 / Stage 4 主线先跑在新模型上
- 其余旧测试对象不做兼容迁移
- 其余旧 smoke / gate / workflow / diagnostic 对象直接进入删除范围
- 后续如仍需要，必须以新的 `subject` 模型重新创建，而不是恢复旧对象

### Phase 3：重写文档与 TUI

- 重写 `wiki/06-测试验证/` 主入口
- TUI 改成 subject/goals/artifact-chain 视图
- 帮助和命令清单同步改写

### Phase 4：移除旧模型

- 删除旧 registry 正式路径
- 删除旧对象命令实现
- 删除旧知识入口中的正式语义

## 11.1 首批 subject 范围

当前已确认首批切换范围极度收敛：

- 首批正式 subject 只保留 `HelloWorldObject`

这意味着：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`
- 以及现有旧 gate / workflow / pipeline 对象

在首轮切换中都不进入新模型。

如果这些对象后续仍有保留价值，应在新框架稳定后重新评估，并以新的 `subject.manifest.json` 形式重建。

## 11.2 迁移策略

本次迁移不是“搬运旧对象”，而是“保留主线、删除旧枝杈”。

推荐执行顺序：

1. 新建 `subjects/HelloWorldObject/`
2. 迁移当前 `HelloWorldObject` 源输入与默认 matrix / goal
3. 打通 `run test subject HelloWorldObject`
4. 同步删除旧 `suite/module/system/pipeline` 正式入口
5. 删除除 `HelloWorldObject` 主线外的旧对象定义
6. 后续按需要逐个以新 subject 模型补回真正仍有价值的对象

## 12. 当前仍待补完的设计项

当前已经有较完整骨架，但仍有几项需要下一轮继续收口：

1. 发布级 perf report 的聚合格式
2. 新 wiki 入口的具体目录结构
3. subject 与具体文件级 artifact contract 的绑定方式

## 12.1 已确认的 report / summary / event 流层级

当前已确认：

- 新报告与事件流以 `subject` 为顶层组织
- `matrix` 作为 `subject` 下的第二层
- `stage` 与 `artifacts` 作为 `matrix` 下的第三层

不再沿用旧的：

- `session -> suite`

作为主要视图。

### 推荐的报告层级

```text
subject-report
  matrix-results[]
    stage-results[]
    artifact-results[]
    runtime-results
    perf-results
```

### 设计含义

- `subject-report`
  - 表示某个 subject 的整体状态
  - 是默认查看和汇总的第一入口
- `matrix-results[]`
  - 表示同一个 subject 在不同 host/target/toolchain/goal 下的执行结果
  - 这是多平台正确性与平台适配性的主观察层
- `stage-results[]`
  - 表示 `source / host-input / analysis / generated / build / runtime / report` 这些阶段桶的结果
  - 用于快速定位坏在哪个产物阶段
- `artifact-results[]`
  - 表示阶段桶内部的关键文件、contract 或派生产物
  - 用于进一步做精细诊断

### 推荐的日志目录骨架

```text
artifacts/subjects/<subject-id>/
  report/
    summary.json
    events.jsonl
    matrices/
      <matrix-id>/
        report.json
        events.jsonl
        stages/
          analysis/
          generated/
          build/
          runtime/
```

### 推荐的事件流层级

建议事件流最少区分三层标识：

- `subjectId`
- `matrixId`
- `stageId`

这样事件既能做全局聚合，也能回落到单个 matrix 的单个阶段。

### 直接收益

- 看 `HelloWorldObject` 的多平台状态时，不会被拆散成多个独立 session
- 发布级 perf report 也可以天然按 subject 聚合
- 调试时先看 subject，再看 matrix，再看 stage，路径清晰

## 12.2 已确认的首轮保留资产范围

当前已确认：

- 首轮硬切换只保留主线必需资产

保留范围应限制在：

- `HelloWorldObject` 作为唯一首批 subject
- canonical contracts / examples / snapshots
- Stage 3 `frontend-pipeline` 直接必需代码
- Stage 4 native / reference proof 直接必需代码
- 最小文档入口与最小 registry / CLI / report 骨架

明确不保留为首轮正式资产的范围：

- 旧 smoke subject
- 旧 gate / workflow / pipeline 对象
- 非主线 diagnostic subject
- 非主线平台 gate 保留物

这意味着：

- 首轮目标不是“保住旧测试覆盖率”
- 而是“把唯一主线在新模型上重新建立并打通”

## 12.3 首轮切换后的资产处置原则

对非首轮保留对象，默认策略不是迁移，而是删除。

后续如果某个对象仍被证明有价值：

- 必须以新的 `subject.manifest.json` 模型重建
- 不允许直接恢复旧 `suite/module/system/pipeline` 形式

## 13. 当前结论

当前已经可以把这次重构概括为一句话：

测试框架从“围绕旧测试对象和执行命令组织”，切到“围绕 subject、goal、artifact-chain 和 environment-matrix 组织”，并用硬切换方式完成模型替换。
