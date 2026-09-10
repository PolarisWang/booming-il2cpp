# 阶段产物落点设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 结论

在新框架下，每个阶段的产物落点应按两层分开：

- `shared/`
  - 放同一 `subject` 可跨 matrix 复用的前半段产物
- `matrices/<matrix-id>/`
  - 放当前 matrix 独有的后半段产物与结果

正式根路径：

```text
artifacts/subjects/<subject-id>/
```

对首批 `HelloWorldObject` Windows 四个 matrix，推荐布局为：

```text
artifacts/subjects/HelloWorldObject/
  shared/
    source/
    host-input/
    analysis/
    generated/
  matrices/
    windows-dev-output/
      build/
      runtime/
      report.json
      events.jsonl
    windows-reference-trace/
      build/
      runtime/
      report.json
      events.jsonl
    windows-android-buildable/
      build/
      report.json
      events.jsonl
    windows-linux-buildable/
      build/
      report.json
      events.jsonl
  subject-report/
    summary.json
    events.jsonl
```

## 2. 最重要的规则

## 2.1 bucket 按需创建

不是所有 bucket 都预先创建，而是：

- 某个 stage 真正执行了
- 才创建对应 bucket

例如：

- `windows-dev-output` 会创建 `build/` 和 `runtime/`
- `windows-reference-trace` 会创建 `build/` 和 `runtime/`
- `windows-android-buildable` 默认不会创建 `runtime/`
- `windows-linux-buildable` 默认不会创建 `runtime/`

所以“没有这个目录”本身也代表一条语义：

- 该 matrix 没跑到这个阶段

## 2.2 `source` 本体不复制，`source` 阶段只落索引

`subject` 的源码权威位置仍然是：

```text
subjects/<subject-id>/source/
```

因此 `source-resolve` 阶段默认只在：

```text
artifacts/subjects/<subject-id>/shared/source/
```

写：

- `source.manifest.json`
- `source.inputs.json`

而不是把整个 source tree 再复制一份到 `artifacts/`。

## 2.3 contract baseline 不进入阶段桶

下列内容继续保留在源码树：

- `contracts/`
- `tests/contracts/`

阶段桶里只放：

- contract validate report
- trace compare report
- resolved manifest

不放 canonical schema/example/header/snapshot 的副本。

## 3. 每个阶段的正式落点

## 3.1 `source-resolve`

阶段角色：

- 解析 subject source
- 固定 entry / source inputs

落点：

```text
subjects/<subject-id>/source/**              # canonical source
artifacts/subjects/<subject-id>/shared/source/
  source.manifest.json
  source.inputs.json
```

说明：

- `subjects/.../source/` 是输入
- `artifacts/.../shared/source/` 是本次执行的 source 侧索引

## 3.2 `host-input-build`

阶段角色：

- 用宿主 .NET 构建 managed 输入包

落点：

```text
artifacts/subjects/<subject-id>/shared/host-input/
  HelloWorldObject.dll
  HelloWorldObject.deps.json
  HelloWorldObject.runtimeconfig.json
  host-input.manifest.json
```

说明：

- 这是后续 `analysis-frontend` 的直接输入
- 同一 subject、同一 host profile 下可跨 matrix 复用

## 3.3 `analysis-frontend`

阶段角色：

- 执行 `frontend-pipeline`
- 产出 `proof-input-bundle`

落点：

```text
artifacts/subjects/<subject-id>/shared/analysis/
  typed-il-ir.json
  aot-manifest.json
  metadata-registration.json
  code-registration.json
  closure.manifest.json
  analysis.manifest.json
  contract-validate.report.json
```

说明：

- 这里是 Stage 3/前端分析产物的正式落点
- 逻辑角色名是 `proof-input-bundle`
- 物理目录名是 `analysis/`

## 3.4 `generated-native-proof`

阶段角色：

- 消费 `proof-input-bundle`
- 产出 Stage 4 generated source 与 native proof 清单

落点：

```text
artifacts/subjects/<subject-id>/shared/generated/
  HelloWorldObject.generated.cpp
  native-proof.manifest.json
  generated.manifest.json
```

说明：

- 这是 `build-target` 的正式输入
- 同一 subject 的多个 Windows matrix 可共享

## 3.5 `build-target`

阶段角色：

- 针对当前 matrix 的 target 执行 configure/build

落点：

```text
artifacts/subjects/<subject-id>/matrices/<matrix-id>/build/
  build.manifest.json
  stdout.log
  stderr.log
  out/
    ...
```

其中：

- `windows-dev-output` 与 `windows-reference-trace`
  - `out/` 下是 Windows reference host build 产物
- `windows-android-buildable`
  - `out/` 下是 Android buildable shell / packaging 中间产物
- `windows-linux-buildable`
  - `out/` 下是 Linux buildable shell / packaging 中间产物

说明：

- `build/` 必须是 matrix 独占
- 不能和 `shared/` 混放

## 3.6 `runtime-observe`

阶段角色：

- 执行 runtime 观察
- 收集 stdout / stderr / exit code / 可选 trace

落点：

```text
artifacts/subjects/<subject-id>/matrices/<matrix-id>/runtime/
  runtime.manifest.json
  stdout.log
  stderr.log
  exit-code.json
```

如果是普通输出型 matrix，例如 `windows-dev-output`，一般还会有：

```text
artifacts/subjects/<subject-id>/matrices/windows-dev-output/runtime/
  output.assert.json
```

如果是 trace 型 matrix，例如 `windows-reference-trace`，一般还会有：

```text
artifacts/subjects/<subject-id>/matrices/windows-reference-trace/runtime/
  trace.runtime.json
```

## 3.7 `runtime-trace-compare`

阶段角色：

- 对 runtime trace 做 schema / snapshot / contract compare

落点：

```text
artifacts/subjects/<subject-id>/matrices/windows-reference-trace/runtime/
  trace-compare.report.json
```

说明：

- 这个结果应放在 `runtime/`
- 因为它是在 runtime evidence 上做的验证
- 不应放回 `shared/analysis/`

## 3.8 `report-assemble`

阶段角色：

- 汇总当前 matrix 的 stage 结果与产物索引

落点：

```text
artifacts/subjects/<subject-id>/matrices/<matrix-id>/
  report.json
  events.jsonl
```

说明：

- 这是 matrix 级最终报告
- 不再单独放一个 `report/` 目录作为第一版默认布局
- 如果后续 `perf.release` 需要大量附件，再补 `report/` 子目录

## 3.9 `subject` 聚合汇总

阶段角色：

- 汇总当前 subject 的全部 matrix 结果

落点：

```text
artifacts/subjects/<subject-id>/subject-report/
  summary.json
  events.jsonl
```

说明：

- 这是 subject 级聚合视图
- 不承载具体 build/runtime 证据
- 只引用各 matrix 的 `report.json`

## 3.10 session / run 聚合汇总

这个不是 subject 内部阶段，但需要一起说明清楚。

落点继续保留：

```text
artifacts/logs/tests/<run-id>/
  session.json
  summary.json
  events.jsonl
  console.log
  telemetry.json
```

说明：

- 这是 run/session 层汇总
- 不是某个 subject 的内部产物桶
- 当前 TUI / watch / summary 先继续依赖这层

## 4. 用 `HelloWorldObject` 四个 matrix 看一遍

## 4.1 `windows-dev-output`

会用到：

```text
artifacts/subjects/HelloWorldObject/shared/source/
artifacts/subjects/HelloWorldObject/shared/host-input/
artifacts/subjects/HelloWorldObject/shared/analysis/
artifacts/subjects/HelloWorldObject/shared/generated/
artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/
artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime/
artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/report.json
```

## 4.2 `windows-reference-trace`

会用到：

```text
artifacts/subjects/HelloWorldObject/shared/source/
artifacts/subjects/HelloWorldObject/shared/host-input/
artifacts/subjects/HelloWorldObject/shared/analysis/
artifacts/subjects/HelloWorldObject/shared/generated/
artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/build/
artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/
artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace.runtime.json
artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace-compare.report.json
artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json
```

## 4.3 `windows-android-buildable`

会用到：

```text
artifacts/subjects/HelloWorldObject/shared/source/
artifacts/subjects/HelloWorldObject/shared/host-input/
artifacts/subjects/HelloWorldObject/shared/analysis/
artifacts/subjects/HelloWorldObject/shared/generated/
artifacts/subjects/HelloWorldObject/matrices/windows-android-buildable/build/
artifacts/subjects/HelloWorldObject/matrices/windows-android-buildable/report.json
```

不会默认创建：

```text
artifacts/subjects/HelloWorldObject/matrices/windows-android-buildable/runtime/
```

## 4.4 `windows-linux-buildable`

会用到：

```text
artifacts/subjects/HelloWorldObject/shared/source/
artifacts/subjects/HelloWorldObject/shared/host-input/
artifacts/subjects/HelloWorldObject/shared/analysis/
artifacts/subjects/HelloWorldObject/shared/generated/
artifacts/subjects/HelloWorldObject/matrices/windows-linux-buildable/build/
artifacts/subjects/HelloWorldObject/matrices/windows-linux-buildable/report.json
```

不会默认创建：

```text
artifacts/subjects/HelloWorldObject/matrices/windows-linux-buildable/runtime/
```

## 5. 为什么前四段放 `shared/`

首批 Windows 四个 matrix 共享：

- 同一个 subject source
- 同一个 Windows host profile
- 同一个 host-input
- 同一个 `frontend-pipeline`
- 同一个 generated proof 基础

所以：

- `source`
- `host-input`
- `analysis`
- `generated`

放进 `shared/` 是合理的。

而：

- `build`
- `runtime`
- `report`

必须按 matrix 隔离，因为：

- target 不同
- runtimeProfile 不同
- 验证边界不同

## 6. 对 `report` bucket 的当前解释

当前第一版不单独展开成：

```text
matrices/<matrix-id>/report/
```

而是采用轻量形式：

- `matrices/<matrix-id>/report.json`
- `matrices/<matrix-id>/events.jsonl`
- `subject-report/summary.json`

原因：

- 第一版更容易落地
- 现有 `summary/events` 体系可以平滑接上
- 后续如果 `perf.release` 需要大量附件，再扩成 `report/` 子目录

## 7. 当前可以正式固定的规则

这次可以正式固定为：

- 正式根路径：`artifacts/subjects/<subject-id>/`
- `source/host-input/analysis/generated` 放 `shared/`
- `build/runtime` 放 `matrices/<matrix-id>/`
- matrix 最终结果放 `matrices/<matrix-id>/report.json`
- subject 聚合放 `subject-report/summary.json`
- session 聚合继续放 `artifacts/logs/tests/<run-id>/summary.json`
- bucket 按需创建，不跑到该阶段就不创建对应目录
- `source` 阶段默认不复制源码树
- contract baseline 不进入阶段桶
