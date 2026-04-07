# 测试框架重构与升级 Design Draft v1

Date: 2026-04-06
Status: draft-discuss

## 这份文档的定位

这不是最终实施 design。

这是一份基于当前已经确认的讨论结论，整理出的第一版 design draft。后续继续讨论时，应优先在这份文档上补全未决点，而不是重新发散回旧模型。

## 设计目标

新测试框架要服务的不是“命令统一”，而是 IL2CPP 主线的证据链、诊断链和发布链。

当前确认的四个顶层测试目标是：

1. 开发环境正确性验证
2. 多平台正确性验证
3. 开发环境性能测试
4. 发布产物性能报告

补充约束：

- 平台适配性与多平台正确性必须放在同一组问题中建模
- 发布级性能报告需要覆盖所有 case 和所有平台

## 核心设计原则

### 原则 1：测试主轴改成“目标维度 + 产物链阶段”

第一主轴：

- `correctness.dev`
- `correctness.platform`
- `perf.dev`
- `perf.release`

第二主轴：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

这两层组合后，测试框架才能同时回答：

- 这次是在验证什么目标
- 坏在哪个阶段
- 该平台已经适配到什么程度

### 原则 2：`subject` 成为唯一中心模型

后续测试框架不再围绕：

- `suite`
- `module`
- `system`
- `pipeline`

这些旧对象组织。

统一改为围绕 `subject` 组织。

`subject` 表示一个长期稳定的被验证对象，可以是：

- canonical proof 输入
- 调试定位项目
- benchmark 项目

### 原则 3：顶层输入项目提升到 `subjects/`

`HelloWorldObject` 这类对象不再深埋在测试目录内部，而是提升为顶层稳定输入。

目标目录形态：

```text
subjects/
  HelloWorldObject/
    subject.manifest.json
    source/
```

对应阶段产物统一落到：

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

这样做的核心目的不是整理目录，而是为了让验证过程沿着固定产物链进行诊断。

### 原则 4：旧模型硬切换退出

当前讨论已确认：

- 现有 `suite/module/system/pipeline` 不保留为正式模型
- 旧命令不保留兼容别名
- registry 改成以 `subjects` 为唯一正式注册源
- 文档主入口也同步重写

这意味着本次重构不是平滑演化，而是显式切换。

## 新测试对象模型

## `subject` 的职责

一个 `subject` 是测试体系里的唯一主对象，负责表达：

- 这个输入对象是什么
- 它的源码或输入在哪里
- 它期望经过哪些产物阶段
- 它在哪些环境矩阵下参与哪些验证目标

## `subject.manifest.json` 第一版最小字段集合

当前已确认第一版采用平衡版结构，最小字段为：

- `subjectId`
- `displayName`
- `category`
- `source`
- `tags`
- `expectedArtifacts`
- `environmentMatrices`

### `category`

当前已确认采用粗粒度类别：

- `canonical`
- `diagnostic`
- `benchmark`

细节角色通过 `tags` 补充，例如：

- `proof`
- `smoke`
- `regression`
- `debuggable`

### `source.type`

当前已确认第一版支持：

- `dotnet-project`
- `managed-assembly`
- `native-project`
- `source-file`

### `expectedArtifacts`

当前已确认第一版采用稳定的产物阶段桶：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

这些桶不直接替代具体文件名，而是先建立稳定的产物层级。

## 环境矩阵模型

## 环境矩阵的组织方式

当前已确认：

- 单个 `subject.manifest.json` 允许声明多条 environment matrix

每条 matrix 至少应包含两部分：

- `executionContext`
- `validationIntent`

### `executionContext`

建议承载：

- `hostPlatform`
- `targetPlatform`
- `toolchainProfile`
- `runtimeProfile`

### `validationIntent`

建议承载：

- `validationMode`
- `adaptationLevel`
- `expectedOutcome`

## `validationMode`

当前已确认枚举：

- `schema`
- `compile-only`
- `build`
- `startup`
- `output`
- `trace`
- `perf`
- `report`

它描述“本次验证打算收集什么证据”。

## `adaptationLevel`

当前已确认枚举：

- `unsupported`
- `compile-only`
- `buildable`
- `startup`
- `observable-output`
- `traceable`
- `perf-reportable`

它描述“该平台当前被适配到了什么成熟度”。

## 命名重构

## `managed-closure` 的替换方向

当前讨论已确认不再继续使用 `managed-closure` 作为长期语义名。

替换方向采用双层表达：

- 模块职责名：`frontend-pipeline`
- 关键产物角色名：`proof-input-bundle`

这两个名字解决的是不同问题：

- `frontend-pipeline` 描述 Stage 3 managed 前端流水线在做什么
- `proof-input-bundle` 描述这组 Stage 3 产物对 Stage 4 native proof 的角色

对 `HelloWorldObject` 来说：

- `frontend-pipeline` 读取 host-input，例如 `.dll`
- 生成 `typed-il-ir`、`aot-manifest`、`metadata-registration`、`code-registration` 等前端产物
- 这些产物从 Stage 4 视角看，整体就是 `proof-input-bundle`

## 新 CLI 入口

当前已确认：

- `run test` 新主语法改为显式 `subject`

暂定主语法：

```text
run test subject <subject-id>
run test subject <subject-id> --matrix <matrix-id>
run test subject <subject-id> --goal <goal-id>
```

## 已确认的 matrix 默认选择规则

当前已确认：

- 当执行 `run test subject <subject-id>` 且未显式提供 `--matrix` 时
- 使用该 subject manifest 中声明的 `defaultMatrix`

这意味着：

- `subject` 默认行为不是“全矩阵批跑”，而是“按 subject 自己定义的默认矩阵执行”
- 多平台全量验证应由显式选择或更高层批量入口触发

这条规则的直接影响是：

- `subject` 既可以保留默认快速入口，也可以保留显式单矩阵切换能力
- `defaultMatrix` 将成为 subject 设计的一部分，而不是运行时推断行为

## 已确认的 goal 默认选择规则

当前已确认：

- 当执行 `run test subject <subject-id>` 且未显式提供 `--goal` 时
- 使用该 subject manifest 中声明的 `defaultGoal`

这意味着：

- `subject` 默认行为由 `defaultMatrix + defaultGoal` 一起决定
- 不再使用“默认跑全部 goal”的重量行为
- 日常执行语义将更稳定，也更适合开发态调试

## 已确认的默认值缺失错误语义

当前已确认：

- 当一个 `subject` 存在多个 `matrix` 或多个 `goal`
- 但 `subject.manifest.json` 未声明 `defaultMatrix` 或 `defaultGoal`
- 则直接视为配置错误

这意味着：

- 框架不会做隐式猜测
- 也不会退回到“自动跑全部”
- subject 的默认执行语义必须显式声明

## 已确认的 `expectedOutcome` 枚举

当前已确认采用简化结果枚举：

- `pass`
- `fail`
- `known-fail`
- `not-applicable`

含义：

- `pass`：该 matrix/goal 当前应通过
- `fail`：该 matrix/goal 当前应失败，用于负向断言
- `known-fail`：当前已知未完成或已知缺口，被正式记录，但不应伪装成意外回归
- `not-applicable`：该 subject 在该 matrix/goal 下不适用

这套枚举与运行结果的关系应保持分层：

- `expectedOutcome` 是设计期预期
- 实际运行结果仍由执行态报告输出
- 框架负责比较“预期”和“实际”，再决定是否标记为失败、known-fail 命中或配置问题

后续所有公开命令、TUI、帮助和文档都围绕这套入口收口。

## registry 重构

当前已确认：

- `subjects` 成为唯一正式注册源

新的 registry 应直接扫描：

```text
subjects/**/subject.manifest.json
```

不再保留现有：

- `tests/registry/modules`
- `tests/registry/system`
- `tests/registry/pipelines`

作为正式注册源。

新的 registry 需要提供的不是旧对象类型列表，而是至少这些视图：

- 可用 `subject`
- 可用 `matrix`
- 可用 `goal`
- subject 与 artifact bucket 的关系
- subject 与 host/target/toolchain 的关系

## 文档体系重构

当前已确认：

- 文档主入口也同步硬切换

旧的项目级测试文档入口：

- 模块
- 整体验证场景
- 测试管线

后续应重写为新的三层入口：

- `subjects`
- `goals`
- `artifact-chain`

建议含义：

- `subjects`：有哪些稳定被验证对象
- `goals`：四个顶层测试目标
- `artifact-chain`：统一产物链与诊断方法

## 迁移边界

由于当前路线已经确认是硬切换，迁移时必须成批处理，而不是半替换状态长期存在。

至少需要一起替换：

- CLI 解析与帮助
- TUI 测试菜单
- registry 扫描与展示
- 项目级测试文档导航
- 基于旧对象名的测试用例
- `wiki/06-测试验证/` 的主结构

## 当前建议的 `subject.manifest.json` 草案

下面是一版当前讨论所允许的最小形态草案：

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
    }
  ]
}
```

## 仍待确认的问题

虽然当前 design draft 已经有明确骨架，但还有几项没有正式收口：

1. 旧目录如何批量迁移到 `subjects/`
2. `subject` 级别的 report 汇总模型
3. 发布级 perf report 的产物布局
4. 新 wiki 入口页和迁移策略

## 已确认的 goal 枚举

当前已确认 `goal` 的正式命名固定为：

- `correctness.dev`
- `correctness.platform`
- `perf.dev`
- `perf.release`

## 建议的下一轮讨论顺序

为了尽快把 draft 收口成正式 design，建议后面按这个顺序继续：

1. 固定新文档入口结构
2. 再落具体迁移计划
