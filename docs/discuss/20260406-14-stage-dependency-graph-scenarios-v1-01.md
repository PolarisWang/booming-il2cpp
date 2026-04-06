# stage / dependency / graph 落地场景说明 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 结论

`stage`、`dependency`、`graph` 不是为了把测试框架抽象得更复杂，而是为了回答三个非常实际的问题：

- 这次到底要跑哪些步骤
- 某一步失败时，问题应该定位到哪一层
- 哪些已有产物可以复用，哪些必须重跑

如果没有这三层概念，新框架最后还是会退回到：

- 一支大脚本
- 一堆隐式顺序
- 一次失败只能看到“整体没过”

这和你现在想要的“按产物链定位 IL2CPP 问题”是冲突的。

## 2. `stage` 落地时是什么

`stage` 不是抽象名词，它对应的是“一个有明确输入、输出、责任边界的执行步骤”。

例如 `HelloWorldObject` 的 Windows 主线里：

- `source-resolve`
  - 读取 `subjects/HelloWorldObject/source/`
  - 产出 `source.manifest.json`
- `host-input-build`
  - 做 .NET build
  - 产出 `host-input/HelloWorldObject.dll`
- `analysis-frontend`
  - 跑 `frontend-pipeline`
  - 产出 `typed-il-ir.json`、`aot-manifest.json`
- `generated-native-proof`
  - 产出 `HelloWorldObject.generated.cpp`
- `build-target`
  - 产出当前 matrix 的 native build 输出
- `runtime-observe`
  - 产出 stdout / exit code / trace

所以 `stage` 落地时，实际就是：

- 一个执行动作
- 一个产物桶
- 一个失败定位边界

## 3. `dependency` 落地时是什么

`dependency` 表示“这个 stage 依赖哪些上游 stage 的结果”。

例如：

- `analysis-frontend` 依赖 `host-input-build`
- `generated-native-proof` 依赖 `analysis-frontend`
- `build-target` 依赖 `generated-native-proof`
- `runtime-trace-compare` 依赖 `runtime-observe`

它落地后主要解决两类场景。

### 场景 1：失效传播

如果 `analysis-frontend` 的输入变了，例如：

- `Chaos.IL2CPP.Driver` 变了
- analysis contract 变了
- host-input 变了

那么：

- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`

都必须失效重跑。

这就是 dependency 的第一价值：

- 不靠人猜，框架自己知道该从哪一层往后重跑

### 场景 2：失败定位

如果 `windows-linux-buildable` 失败在 `build-target`，那说明：

- `source`
- `host-input`
- `analysis`
- `generated`

已经过了。

问题更可能在：

- Linux toolchain 路由
- generated source 和 Linux 壳层的适配关系

而不是去怀疑 `HelloWorldObject.dll` 没编出来。

这就是 dependency 的第二价值：

- 通过上游已成功的链路，快速缩小问题范围

## 4. `graph` 落地时是什么

`graph` 不是图论装饰，它本质上是：

- 某条 matrix 的正式执行计划

也就是：

- 先跑什么
- 后跑什么
- 跑到哪里结束
- 哪些是 shared stage
- 哪些是 matrix stage

例如：

### `windows-dev-output`

```text
source-resolve
  -> host-input-build
  -> analysis-frontend
  -> generated-native-proof
  -> build-target
  -> runtime-observe
  -> report-assemble
```

### `windows-reference-trace`

```text
source-resolve
  -> host-input-build
  -> analysis-frontend
  -> generated-native-proof
  -> build-target
  -> runtime-observe
  -> runtime-trace-compare
  -> report-assemble
```

### `windows-android-buildable`

```text
source-resolve
  -> host-input-build
  -> analysis-frontend
  -> generated-native-proof
  -> build-target
  -> report-assemble
```

注意这里最关键的区别：

- `windows-reference-trace` 要跑到 `runtime-trace-compare`
- `windows-android-buildable` 到 `build-target` 就结束

这就是 graph 的落地含义。

## 5. 为什么只靠 `validationMode` 不够

如果只有：

- `validationMode=trace`
- `validationMode=build`

你只能知道“这条 matrix 想验证什么”，但不知道：

- 中间具体经过哪些 stage
- 这些 stage 怎么复用
- 哪些阶段在 `shared/`，哪些在 `matrices/<matrix-id>/`

所以：

- `validationMode` 只表达目标
- `graph` 才表达执行路径

两者不是重复关系。

## 6. 这些概念在实际执行里怎么用

### 场景 1：执行器决定要跑什么

命令：

```text
run test subject HelloWorldObject --matrix windows-reference-trace
```

执行器看到 `pipelineId=proof-runtime-trace` 后，就知道要依次跑：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`
- `report-assemble`

如果没有 graph，这个顺序只能硬编码在执行器里。

### 场景 2：决定哪些阶段可以复用

命令先跑：

```text
run test subject HelloWorldObject --matrix windows-dev-output
```

然后再跑：

```text
run test subject HelloWorldObject --matrix windows-reference-trace
```

这时 graph + scope 会告诉执行器：

- `source-resolve` 可复用
- `host-input-build` 可复用
- `analysis-frontend` 可复用
- `generated-native-proof` 可复用
- `build-target` 不复用
- `runtime-observe` 不复用

所以第二次不会从头全跑。

### 场景 3：决定是否需要重跑

如果你改了：

- `Program.cs`

那 `source` fingerprint 变化，整条链后面都要重跑。

如果你只改了：

- trace snapshot

那只需要：

- `runtime-trace-compare`

重跑，不需要重新 build。

这就是 graph + dependency + fingerprint 一起工作的场景。

### 场景 4：支持不同类型 subject

以后如果有一个 diagnostic subject，只想验证：

- `source`
- `host-input`
- `analysis`

那它可以声明一条更短的 pipeline。

如果有 benchmark subject，未来可以声明：

- `build-target`
- `runtime-observe`
- `perf-report`

这时执行器不用改核心分支，只要换 graph。

## 7. 对你这个项目最实际的收益

放在 booming-il2cpp 这个项目里，最实际的收益不是“概念更漂亮”，而是：

- `HelloWorldObject` 出问题时，能知道是 `host-input`、`analysis`、`generated` 还是 `build/runtime`
- Android/Linux 的 buildable matrix 不会再混成一个历史 gate
- `windows-reference-trace` 和 `windows-dev-output` 能共享前半段，但保留后半段隔离
- 后续扩更多 `subjects/` 时，不需要继续堆脚本和分支判断

## 8. 一句话总结

如果只用一句话说：

- `stage` 是可定位的执行边界
- `dependency` 是失效传播和复用判断的依据
- `graph` 是某条 matrix 的正式执行计划

三者合在一起，才能让新框架真正按产物链工作，而不是换个名字继续跑“大一统脚本”。
