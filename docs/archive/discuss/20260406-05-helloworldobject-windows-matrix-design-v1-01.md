# HelloWorldObject 首批 Windows Matrix 设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 本轮范围

本轮只收敛首批正式 `subject`：

- `HelloWorldObject`

本轮只收敛 Windows 侧 matrix：

- `windows-dev-output`
- `windows-reference-trace`
- `windows-android-buildable`
- `windows-linux-buildable`

明确暂不补：

- `macos-reference-trace`
- `macos-ios-buildable`
- `macos-linux-buildable`

这意味着首批设计先把 Windows 主线跑通，并把旧 `roadmap-0` 中真正需要继承的能力，转译成 `HelloWorldObject` 的 matrix，而不是继续保留宿主工作流对象。

## 2. 这四个 matrix 分别解决什么问题

### 2.1 `windows-dev-output`

用途：

- 作为默认开发环境正确性入口
- 验证 `HelloWorldObject` 在 Windows 开发环境下能完成主线 proof 流程并得到正确输出

对应测试目标：

- `correctness.dev`

核心证据：

- host 输入可构建
- analysis 产物可生成
- generated 产物可生成
- native proof 可构建
- runtime 输出与退出码符合预期

### 2.2 `windows-reference-trace`

用途：

- 作为 Windows 桌面参考基线
- 验证 reference desktop 路径上的 trace contract 未漂移

对应测试目标：

- `correctness.platform`

核心证据：

- 主线 proof 可构建
- runtime trace 可导出
- trace 与 snapshot / contract 对齐

### 2.3 `windows-android-buildable`

用途：

- 作为 Windows 宿主下 Android 适配性验证入口
- 不验证完整 Android runtime，先验证当前 subject 能被路由到 Android buildable 状态

对应测试目标：

- `correctness.platform`

核心证据：

- 当前 subject 的前端产物可用于 Android 目标路由
- 目标 shell / build 输出可以生成
- 失败能回落到当前 subject 的 generated / build 产物

### 2.4 `windows-linux-buildable`

用途：

- 作为 Windows 宿主下 Linux 适配性验证入口
- 不验证完整 Linux runtime，先验证当前 subject 能被路由到 Linux buildable 状态

对应测试目标：

- `correctness.platform`

核心证据：

- 当前 subject 的前端产物可用于 Linux 目标路由
- 目标 shell / build 输出可以生成
- 失败能回落到当前 subject 的 generated / build 产物

## 3. 关键判断

旧 `roadmap-0` 里的 Android / Linux gate 不能原样平移进新模型。

原因不是名字旧，而是语义不对：

- 旧 gate 更接近“宿主路由或 preset 还活着”
- 新 matrix 必须回答“这个 subject 在这个 host/target 组合下达到了什么成熟度”

所以首批 `windows-android-buildable` 和 `windows-linux-buildable` 的最低要求是：

- 它们必须消费 `HelloWorldObject` 自己的 `analysis` / `generated` 主线产物
- 而不是只验证一个与 subject 解耦的通用空壳路由

否则它们只是平台工具链自检，不是 subject 测试。

## 4. 推荐的 matrix 定义

## 4.1 推荐命名原则

matrix 命名固定表达三个东西：

- `host`
- `validation shape`
- `evidence depth`

因此首批名称定为：

- `windows-dev-output`
- `windows-reference-trace`
- `windows-android-buildable`
- `windows-linux-buildable`

不再延续：

- `roadmap-0`
- `reference-gate`
- `packaging-gate`
- `startup-gate`

这类历史工作流命名作为正式模型名。

## 4.2 推荐 manifest 草案

下面是一版更适合首批实施的 `HelloWorldObject` manifest 草案。

相对前一版设计稿，这里额外引入两个必要字段：

- `supportedGoals`
- `artifactPlan`

原因见第 6 节。

```json
{
  "subjectId": "HelloWorldObject",
  "displayName": "HelloWorldObject",
  "category": "canonical",
  "defaultGoal": "correctness.dev",
  "defaultMatrix": "windows-dev-output",
  "source": {
    "type": "dotnet-project",
    "path": "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
    "entry": "HelloWorldObject/Program::Main(System.String[])"
  },
  "tags": ["proof", "object", "debuggable", "reference"],
  "expectedArtifacts": [
    "source",
    "host-input",
    "analysis",
    "generated",
    "build",
    "runtime",
    "report"
  ],
  "environmentMatrices": [
    {
      "matrixId": "windows-dev-output",
      "supportedGoals": ["correctness.dev"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "windows-x64",
        "toolchainProfile": "msvc-reference",
        "runtimeProfile": "native-proof-output"
      },
      "validationIntent": {
        "validationMode": "output",
        "adaptationLevel": "observable-output",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime"],
        "terminalBucket": "runtime"
      }
    },
    {
      "matrixId": "windows-reference-trace",
      "supportedGoals": ["correctness.platform"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "windows-x64",
        "toolchainProfile": "msvc-reference",
        "runtimeProfile": "reference-trace"
      },
      "validationIntent": {
        "validationMode": "trace",
        "adaptationLevel": "traceable",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime"],
        "terminalBucket": "runtime"
      }
    },
    {
      "matrixId": "windows-android-buildable",
      "supportedGoals": ["correctness.platform"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "android-arm64",
        "toolchainProfile": "android-ndk",
        "runtimeProfile": "platform-build"
      },
      "validationIntent": {
        "validationMode": "build",
        "adaptationLevel": "buildable",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build"],
        "terminalBucket": "build"
      }
    },
    {
      "matrixId": "windows-linux-buildable",
      "supportedGoals": ["correctness.platform"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "linux-x64",
        "toolchainProfile": "linux-cross",
        "runtimeProfile": "platform-build"
      },
      "validationIntent": {
        "validationMode": "build",
        "adaptationLevel": "buildable",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build"],
        "terminalBucket": "build"
      }
    }
  ]
}
```

## 5. 每个 matrix 的验证边界

## 5.1 `windows-dev-output`

建议边界：

- `source`
  - `HelloWorldObject.csproj` 与 canonical shape 固定
- `host-input`
  - dotnet build 成功
  - 可得到主程序集与基础 host 产物
- `analysis`
  - `frontend-pipeline` 成功产出 `proof-input-bundle`
- `generated`
  - native proof/reference 所需 generated source 成功落盘
- `build`
  - Windows reference/native proof 可构建
- `runtime`
  - 执行结果输出 `Hello, World!`
  - exit code 为 0

它是首批默认 matrix，因为它最符合“开发环境正确性验证”的日常入口语义。

## 5.2 `windows-reference-trace`

建议边界：

- 复用 `windows-dev-output` 的前置主线
- `runtime` 阶段不再只看 stdout
- 改为看：
  - trace 文件落盘
  - trace schema 合法
  - trace snapshot / contract compare 通过

它不是默认 matrix，因为它更像平台参考基线，而不是最快的日常开发检查。

## 5.3 `windows-android-buildable`

建议边界：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`

这里的 `build` 不等于完整 Android 运行时闭环，而是：

- Android 目标路由已选中
- 当前 subject 的 generated/native shell 已被纳入构建输入
- configure/build 或 packaging shell 可完成

明确不要求：

- 模拟器启动
- 真机运行
- runtime trace
- 目标输出断言

## 5.4 `windows-linux-buildable`

建议边界与 `windows-android-buildable` 一致，只是 target 改为 `linux-x64`。

这里也不等于完整 Linux runtime 正确性，而是：

- 当前 subject 的 generated/native shell 已用于 Linux 目标构建
- 可以把问题定位到 build 之前的哪一层产物出了问题

## 6. 这次设计暴露出的两个必须调整点

## 6.1 `expectedArtifacts` 不能只放在 subject 级

如果 `expectedArtifacts` 只在 subject 级存在，那么：

- `windows-dev-output` 会到 `runtime`
- `windows-reference-trace` 会到 `runtime`
- `windows-android-buildable` 只到 `build`
- `windows-linux-buildable` 只到 `build`

这四种 matrix 的边界明显不同。

所以首批实施至少要补一个 matrix 级字段。

推荐方案：

- 保留 subject 级 `expectedArtifacts`
  - 语义改为“该 subject 在全部 matrix 合并后理论可覆盖的最大产物包络”
- 增加 matrix 级 `artifactPlan`
  - `requiredBuckets`
  - `terminalBucket`

这是首批设计里必须落地的结构，不是可选优化。

## 6.2 matrix 必须能声明自己支持哪些 goal

如果没有 matrix 级 `supportedGoals`，那么下面的命令组合没有办法被正确验证：

- `run test subject HelloWorldObject --goal correctness.dev --matrix windows-linux-buildable`
- `run test subject HelloWorldObject --goal correctness.platform --matrix windows-dev-output`

框架必须知道：

- 这是合法组合
- 还是配置错误
- 还是应当提示切换 matrix

推荐方案：

- 每个 matrix 显式声明 `supportedGoals`
- CLI 在解析后先做组合合法性校验

这能避免“goal 只是标签、对执行无约束”的伪设计。

## 7. 推荐的产物目录调整

当前设计稿里，subject 级目录和 matrix 级报告目录是分开的，但首批 Windows matrix 一落地，就会发现仅 subject 级桶不够用。

推荐改成：

```text
artifacts/subjects/<subject-id>/
  subject-report/
    summary.json
    events.jsonl
  matrices/
    <matrix-id>/
      source/
      host-input/
      analysis/
      generated/
      build/
      runtime/
      report.json
      events.jsonl
```

这样做的收益是：

- 同一个 subject 的不同 matrix 不会互相覆盖产物
- 平台 buildable matrix 能自然停在 `build/`
- reference trace matrix 能自然把 trace 放到自己的 `runtime/`
- 调试路径就是 `subject -> matrix -> bucket`

## 8. 平台 buildable matrix 的两个实现方案

这里有一个重要实现分歧，需要在设计上定性。

### 方案 A：沿用旧 gate 语义，只验证 routing / preset

特点：

- 迁移最快
- 复用旧 `roadmap-0` 最容易
- 但与 `HelloWorldObject` 绑定很弱

问题：

- 定位不到当前 subject 的 generated/build 问题
- 更像平台工具链自检
- 不满足“基于生成产物定位 IL2CPP 问题”的目标

### 方案 B：以当前 subject 为输入，生成并构建目标平台 shell

特点：

- `windows-android-buildable` / `windows-linux-buildable` 都真正属于 `HelloWorldObject`
- 错误能定位到 `analysis` / `generated` / `build`
- 与新 subject 架构一致

代价：

- 首批实现成本更高
- 需要把旧 gate 的平台路由逻辑改造成 subject-aware worker

推荐结论：

- 采用方案 B

原因：

- 这是你当前测试重构的核心目标之一
- 如果首批平台 matrix 仍然不消费 subject 产物，新模型会在第一天就失真

## 9. 当前阶段的正式建议

首批 `HelloWorldObject` Windows 侧设计建议固定为：

- 默认 `goal`：`correctness.dev`
- 默认 `matrix`：`windows-dev-output`
- Windows 桌面参考基线：`windows-reference-trace`
- Windows 到 Android 的适配性验证：`windows-android-buildable`
- Windows 到 Linux 的适配性验证：`windows-linux-buildable`
- Android / Linux 的 buildable matrix 必须消费当前 subject 的主线生成产物
- 引入 matrix 级 `artifactPlan`
- 引入 matrix 级 `supportedGoals`
- 产物目录调整为 matrix 级桶目录

## 10. 对后续设计稿的影响

这份草案会推动后续正式设计稿做三处实质修订：

- `HelloWorldObject` 示例中的默认 goal / 默认 matrix 改为 Windows 开发主线
- `subject.manifest.json` 模型补充 `supportedGoals` 与 `artifactPlan`
- `artifacts/subjects/` 目录改为 matrix 隔离布局
