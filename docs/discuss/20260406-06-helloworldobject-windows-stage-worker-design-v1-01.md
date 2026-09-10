# HelloWorldObject Windows Stage/Worker 设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

这份设计把上一轮已经确认的四个 Windows matrix，继续收敛到“可执行的 stage/worker 模型”：

- `windows-dev-output`
- `windows-reference-trace`
- `windows-android-buildable`
- `windows-linux-buildable`

本轮要解决的不是 matrix 名称，而是：

- 每个 matrix 由哪些 stage 组成
- 每个 stage 由谁执行
- stage 的输入输出契约是什么
- 哪些 stage 可以跨 matrix 复用
- 现有代码里哪些地方会破坏新的 stage 边界

## 2. 总体结论

四个 Windows matrix 应共享同一条前半段主线：

- `source`
- `host-input`
- `analysis`
- `generated`

然后在 `build` 之后按 matrix 分叉：

- `windows-dev-output` 分叉到 `runtime-output`
- `windows-reference-trace` 分叉到 `runtime-trace`
- `windows-android-buildable` 停在 `build-android`
- `windows-linux-buildable` 停在 `build-linux`

因此，推荐的执行图不是四条完全独立流水线，而是：

```text
source
  -> host-input
  -> analysis
  -> generated
  -> build/windows-reference-host
      -> runtime/output
      -> runtime/trace
  -> build/android-buildable
  -> build/linux-buildable
```

## 3. 推荐的 stage 集合

### 3.1 统一 stage Id

推荐首批固定以下 stage：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`
- `report-assemble`

这里有两个关键点：

- `build-target` 是同一个 stage 类别，但 worker 可按 target 不同而变化
- `runtime-trace-compare` 独立于 `runtime-observe`
  - 因为 trace 导出与 trace 对比不是一回事

### 3.2 stage 与 artifact bucket 的关系

- `source-resolve` -> `source`
- `host-input-build` -> `host-input`
- `analysis-frontend` -> `analysis`
- `generated-native-proof` -> `generated`
- `build-target` -> `build`
- `runtime-observe` -> `runtime`
- `runtime-trace-compare` -> `runtime`
- `report-assemble` -> `report`

## 4. 每个 stage 的职责与契约

## 4.1 `source-resolve`

职责：

- 解析 `subject.manifest.json`
- 固定当前 subject 的 source root、entry、canonical shape
- 为后续 stage 写出 source 侧清单

推荐 worker：

- `subject-source-resolver`

输入：

- `subjects/HelloWorldObject/subject.manifest.json`
- `subjects/HelloWorldObject/source/**`

输出：

- `matrices/<matrix-id>/source/source.manifest.json`
- `matrices/<matrix-id>/source/source.inputs.json`

失败归因：

- subject 配置错误
- source 缺失
- canonical 入口不匹配

## 4.2 `host-input-build`

职责：

- 在宿主 .NET 环境下构建 `HelloWorldObject`
- 产出后续 `frontend-pipeline` 的正式 host 输入

推荐 worker：

- `dotnet-host-input-builder`

输入：

- `source` bucket

输出：

- `matrices/<matrix-id>/host-input/HelloWorldObject.dll`
- `matrices/<matrix-id>/host-input/HelloWorldObject.deps.json`
- `matrices/<matrix-id>/host-input/HelloWorldObject.runtimeconfig.json`
- `matrices/<matrix-id>/host-input/host-input.manifest.json`

失败归因：

- .NET 编译失败
- 宿主 SDK/依赖问题

## 4.3 `analysis-frontend`

职责：

- 运行 `frontend-pipeline`
- 把 host-input 转成 Stage 4 消费的 `proof-input-bundle`

推荐 worker：

- `frontend-pipeline-worker`

输入：

- `host-input` bucket
- `Chaos.IL2CPP.Driver`

输出：

- `typed-il-ir.json`
- `aot-manifest.json`
- `metadata-registration.json`
- `code-registration.json`
- `closure.manifest.json`
- `analysis.manifest.json`

逻辑命名：

- 这一组产物在逻辑上应称为 `proof-input-bundle`

兼容说明：

- 现有磁盘路径仍可能暂时落在 `managed-closure` 目录
- 但在新测试模型中，不再把 `managed-closure` 当成正式长期语义

失败归因：

- Loader / SemanticWorld / Linker / MetadataWriter / CodeGen / Driver 前端链路错误

## 4.4 `generated-native-proof`

职责：

- 消费 `proof-input-bundle`
- 产出目标 proof 所需 generated source 与 native proof 清单

推荐 worker：

- `native-proof-emitter`

输入：

- `analysis` bucket

输出：

- `generated/HelloWorldObject.generated.cpp`
- `generated/native-proof.manifest.json`
- `generated/generated.manifest.json`

失败归因：

- native emission 规则错误
- Stage 4 proof 输入协议不兼容

## 4.5 `build-target`

职责：

- 消费 `generated` bucket
- 为当前 matrix 的 target 构建对应 native shell / proof host

推荐 worker：

- `build-target.windows-reference`
- `build-target.android-buildable`
- `build-target.linux-buildable`

输入：

- `generated` bucket
- target-specific toolchain profile

输出：

- `build/build.manifest.json`
- target-specific configure/build 日志
- target-specific shell / binary / packaging 中间产物

失败归因：

- toolchain profile 错误
- target 路由错误
- subject 生成产物不满足目标平台壳层要求

## 4.6 `runtime-observe`

职责：

- 执行当前 matrix 的 runtime 观察动作
- 收集 stdout / exit code / trace 文件落点

推荐 worker：

- `runtime-observer.output`
- `runtime-observer.trace-export`

输入：

- `build` bucket

输出：

- `runtime/stdout.txt`
- `runtime/stderr.txt`
- `runtime/exit-code.json`
- 可选 `runtime/*.runtime.json`

失败归因：

- 可执行物无法启动
- runtime/bootstrap/support 接线错误
- 目标输出不符合预期

## 4.7 `runtime-trace-compare`

职责：

- 对 `runtime-observe` 产生的 trace 做 schema / snapshot / contract compare

推荐 worker：

- `trace-compare-worker`

输入：

- `runtime/*.runtime.json`
- trace schema
- trace snapshot

输出：

- `runtime/trace-compare.report.json`

失败归因：

- trace contract 漂移
- 导出的 trace 非法
- snapshot 对比失败

## 4.8 `report-assemble`

职责：

- 汇总 matrix 级 `stage-results`
- 汇总 artifact 清单
- 形成 subject / matrix 最终报告

推荐 worker：

- `matrix-report-assembler`

输入：

- 全部 stage 结果

输出：

- `report.json`
- `events.jsonl`

## 5. matrix 到 stage 的映射

## 5.1 `windows-dev-output`

执行链：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target.windows-reference`
- `runtime-observer.output`
- `report-assemble`

终止桶：

- `runtime`

## 5.2 `windows-reference-trace`

执行链：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target.windows-reference`
- `runtime-observer.trace-export`
- `runtime-trace-compare`
- `report-assemble`

终止桶：

- `runtime`

## 5.3 `windows-android-buildable`

执行链：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target.android-buildable`
- `report-assemble`

终止桶：

- `build`

## 5.4 `windows-linux-buildable`

执行链：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target.linux-buildable`
- `report-assemble`

终止桶：

- `build`

## 6. 推荐的复用策略

## 6.1 哪些 stage 可以共享

推荐共享：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`

共享前提：

- 同一个 `subject`
- 同一个 source revision
- 同一个 host profile
- 同一版 driver / frontend 配置

## 6.2 哪些 stage 不应共享

首批不建议共享：

- `build-target`
- `runtime-observe`
- `runtime-trace-compare`

原因：

- target 不同
- runtimeProfile 不同
- 证据类型不同
- 失败定位必须保持 matrix 隔离

## 6.3 关于 `windows-dev-output` 与 `windows-reference-trace` 是否共享 build

这里有两个方案。

### 方案 A：共享同一份 Windows reference build

优点：

- 速度更快
- 物理构建次数更少

问题：

- output 与 trace 两个 matrix 的 build 证据会缠在一起
- 一旦后续 runtime profile 分叉，缓存策略会变脆
- matrix 隔离性变差

### 方案 B：共享到 `generated` 为止，`build-target` 仍按 matrix 独立

优点：

- 报告边界清晰
- 日后对 runtime profile 做差异化时不需要重构缓存键
- 更符合“matrix 是一条独立验证语义”的设计

代价：

- 首批多一次 Windows 构建

推荐结论：

- 采用方案 B

这是更稳的默认值。

## 7. 当前代码中必须拆开的实现点

## 7.1 Stage 4 CMake 现在把多个 stage 糊在一起

当前 `tests/proof/native-reference/HelloWorldObject/CMakeLists.txt` 的 `add_custom_command` 里直接执行了：

- `dotnet build` subject project
- `dotnet build` driver project
- `dotnet run` 生成前端 bundle
- `dotnet run emit-native-reference`

见：
[CMakeLists.txt](/E:/agent/booming-il2cpp/tests/proof/native-reference/HelloWorldObject/CMakeLists.txt#L15)

这会把下面四个 stage 压扁进一个 CMake build 动作里：

- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`

这与新的 stage 报告模型直接冲突。

### 结论

首批实现必须把这些动作从 CMake `custom_command` 中外提：

- CMake `build-target` 只消费已经准备好的 `generated` bucket
- `host-input-build` / `analysis-frontend` / `generated-native-proof` 由独立 worker 显式执行

否则 `stage-results[]` 只是表面存在，实际无法定位问题。

## 7.2 旧 `verify-roadmap-0.py` 可以保留逻辑，但不能保留 orchestrator 角色

现有脚本里这些低层动作可复用：

- dotnet build helper
- frontend bundle 生成
- native reference emission
- routing build smoke
- trace compare

但它们在新模型里必须分别挂到独立 worker，而不是继续由一支脚本串行控制整条流程。

## 8. 平台 buildable matrix 的 worker 约束

`windows-android-buildable` 与 `windows-linux-buildable` 的 worker 必须满足两个约束：

- 必须消费 `generated` bucket，而不是绕过 subject
- 必须把目标平台 configure/build 输出写回当前 matrix 的 `build/`

因此它们应该被设计成：

- `subject-aware platform build worker`

而不是：

- `legacy preset gate wrapper`

这两者的区别非常关键。

前者失败时，你可以判断：

- 是 `analysis` 产物不完整
- 是 `generated` source 不满足目标壳层
- 还是目标 toolchain profile 本身有问题

后者只能说明：

- preset 还活着

## 9. 推荐的目录骨架

推荐把 matrix 级目录细化为：

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

这里选择 `shared/ + matrices/` 双层结构，而不是每个 matrix 都完整复制一遍前四个 bucket。

原因：

- 这四个 matrix 的前半段主线在语义上相同
- 共享前四段能减少重复构建
- 又不会破坏 matrix 级 `build/runtime/report` 隔离

## 10. 下一步实施顺序

如果按最小可落地路线推进，建议顺序固定为：

1. 提炼独立 worker：
   - `dotnet-host-input-builder`
   - `frontend-pipeline-worker`
   - `native-proof-emitter`
2. 修改 Stage 4 CMake：
   - 让 `build-target` 只消费 `generated/HelloWorldObject.generated.cpp`
3. 先打通 `windows-dev-output`
4. 在同一套 worker 上补 `windows-reference-trace`
5. 最后接入 `windows-android-buildable` 与 `windows-linux-buildable`

这个顺序的理由是：

- 先把 Windows 主线正确性链打通
- 再补 reference trace
- 最后处理平台 buildable 的 subject-aware 路由

## 11. 本轮设计结论

本轮可以正式固定的结论是：

- 四个 Windows matrix 共享前半段主线
- `build-target` 之后按 matrix 分叉
- `host-input-build` / `analysis-frontend` / `generated-native-proof` 必须成为显式 stage
- Stage 4 现有 CMake `custom_command` 必须拆开
- `windows-dev-output` 与 `windows-reference-trace` 共享到 `generated` 为止
- Android/Linux 平台验证必须做成 subject-aware build worker

这意味着下一次如果进入正式改动，首批代码切入点应该不是 CLI，而是：

- worker 抽取
- artifact 目录重排
- Stage 4 build 接线去隐式化
