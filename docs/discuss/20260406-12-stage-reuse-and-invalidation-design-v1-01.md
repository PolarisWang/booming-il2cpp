# 阶段复用与失效规则设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

既然阶段产物落点已经确定，下一步必须固定：

- 哪些阶段可以复用
- 什么时候必须判定失效
- 复用判断依赖哪些 fingerprint 维度
- 第一版到底做“单工作目录重建”，还是做“多 fingerprint 历史缓存”

## 2. 结论

推荐第一版采用：

- 显式 stage dependency graph
- bucket 级 fingerprint
- 稳定工作目录 + fingerprint 比对 + 按需重建

不推荐第一版就做：

- 多版本 fingerprint 历史缓存仓库
- 自动 GC
- 跨 subject 的共享缓存

也就是：

- `shared/` 和 `matrices/<matrix-id>/` 继续是稳定工作目录
- 每个 bucket 的 `*.manifest.json` 内记录自己的 fingerprint
- 执行前先比对 fingerprint
- 一旦不匹配，就清理该 bucket 并重建

## 3. 为什么先选这个方案

这里有两个方案。

### 方案 A：稳定工作目录 + 指纹比对 + 按需重建

特点：

- 路径稳定
- 调试简单
- 失败定位最直观
- 不需要额外设计缓存淘汰

代价：

- 不能同时保留多个历史版本的 bucket
- 重跑时会覆盖旧产物

### 方案 B：按 fingerprint 建历史缓存目录

特点：

- 可以保留多个历史版本
- 理论上更利于重复切换分支或配置

代价：

- 目录模型会复杂很多
- GC / current pointer / storage 膨胀都会变成新问题
- 第一版 subject 管线还没稳定，不适合先上这一层

推荐结论：

- 采用方案 A

原因：

- 现在最重要的是把 stage 边界与 subject 调试链打通
- 不应让缓存系统比主流程更复杂

## 4. 正式复用边界

## 4.1 可复用的 bucket

首批 Windows 四个 matrix 下，可复用：

- `shared/source`
- `shared/host-input`
- `shared/analysis`
- `shared/generated`

复用前提：

- 同一个 `subjectId`
- 同一个 host profile
- 对应 fingerprint 完全一致

## 4.2 不可跨 matrix 复用的 bucket

首批不复用：

- `matrices/<matrix-id>/build`
- `matrices/<matrix-id>/runtime`
- `matrices/<matrix-id>/report.json`

原因：

- targetPlatform 不同
- runtimeProfile 不同
- 证据边界不同
- matrix 结果必须彼此隔离

## 4.3 `windows-dev-output` 与 `windows-reference-trace` 的边界

即使这两个 matrix 都是 Windows reference host，也建议：

- 共享到 `generated`
- 不共享 `build` 与 `runtime`

原因：

- `runtimeProfile` 不同
- 一个看 stdout/output
- 一个看 trace/export/compare
- 第一版保持 matrix 隔离更稳

## 5. 每个阶段的 fingerprint 维度

## 5.1 `source-resolve`

最小 fingerprint 输入建议：

- `subjectId`
- `subject.manifest.json` 中与 source 相关字段
- `subjects/<subject-id>/source/**` 文件内容摘要

说明：

- 只要 source tree 或 source 配置变了，就应失效

## 5.2 `host-input-build`

最小 fingerprint 输入建议：

- `source` fingerprint
- host platform
- .NET SDK version
- build configuration，例如 `Release`
- 与宿主编译相关的关键环境变量

说明：

- 这是 managed 宿主输入包的 fingerprint

## 5.3 `analysis-frontend`

最小 fingerprint 输入建议：

- `host-input` fingerprint
- `Chaos.IL2CPP.Driver` 相关源码摘要
- `frontend-pipeline` 相关源码摘要
- canonical analysis contract 摘要

说明：

- 只要前端逻辑、driver、contract source 变化，就应失效

## 5.4 `generated-native-proof`

最小 fingerprint 输入建议：

- `analysis` fingerprint
- native emitter / codegen 相关源码摘要
- Stage 4 proof 配置摘要

说明：

- 只要 emission 规则变了，就应失效

## 5.5 `build-target`

最小 fingerprint 输入建议：

- `generated` fingerprint
- `matrixId`
- `targetPlatform`
- `toolchainProfile`
- CMake / toolchain / preset 相关关键文件摘要
- 当前 host platform

说明：

- `build` 必须被 matrix 自身绑定，不能只看 generated 输入

## 5.6 `runtime-observe`

最小 fingerprint 输入建议：

- `build` fingerprint
- `runtimeProfile`
- 运行参数
- 关键运行环境变量

说明：

- 只要 runtime profile 或输入参数变了，就应失效

## 5.7 `runtime-trace-compare`

最小 fingerprint 输入建议：

- `runtime` fingerprint
- trace schema 摘要
- expected snapshot 摘要

说明：

- 即使 runtime 产物没变，只要 baseline 变了，也必须重跑 compare

## 6. 推荐的失效传播规则

采用单向失效传播：

- `source` 失效 -> `host-input/analysis/generated/build/runtime/report` 全失效
- `host-input` 失效 -> `analysis/generated/build/runtime/report` 全失效
- `analysis` 失效 -> `generated/build/runtime/report` 全失效
- `generated` 失效 -> `build/runtime/report` 全失效
- `build` 失效 -> `runtime/report` 失效
- `runtime` 失效 -> `trace-compare/report` 失效

这个规则应当是严格的，不做乐观跳过。

## 7. fingerprint 放在哪里

推荐第一版直接放进各 bucket 的 manifest：

- `source.manifest.json`
- `host-input.manifest.json`
- `analysis.manifest.json`
- `generated.manifest.json`
- `build.manifest.json`
- `runtime.manifest.json`

每个 manifest 至少补三类字段：

- `fingerprint`
- `upstreamFingerprints`
- `producer`

例如：

```json
{
  "fingerprint": "ab12cd34ef56",
  "upstreamFingerprints": {
    "source": "f1",
    "host-input": "f2"
  },
  "producer": {
    "stageId": "analysis-frontend",
    "toolVersion": "local-worktree"
  }
}
```

## 8. 第一版不建议额外新增 `current.json`

当前旧框架在部分地方使用：

- `current.json`
- `last.json`

但在新 subject bucket 体系里，第一版不建议也引入一层同构指针。

原因：

- `shared/source`、`shared/host-input` 等本身就是当前工作目录
- 再加 `current.json` 只是重复表达“当前”

推荐做法：

- run/session 层继续保留 `current.json` / `last.json`
- subject/bucket 层不引入新 pointer 文件
- 直接以 bucket manifest 为唯一当前状态入口

## 9. 执行器的判断顺序

执行某个 stage 时，推荐固定以下顺序：

1. 检查上游 bucket manifest 是否存在
2. 检查上游 fingerprint 是否满足当前请求
3. 计算本 stage 的请求 fingerprint
4. 若当前 bucket manifest 不存在：
   - 直接执行
5. 若 manifest 存在但 fingerprint 不一致：
   - 清理当前 bucket
   - 重新执行
6. 若 manifest 存在且 fingerprint 一致：
   - 标记为 `reused`
   - 跳过执行

这一步应在事件里明确记录：

- `executed`
- `reused`
- `invalidated`

## 10. 对 `prepare` 的影响

现有旧框架里有 `prepare` 和可复用 prepared state。

在新 subject 管线下，`prepare` 不应再等同于“某个测试对象的 build cache”。

更合理的分工是：

- `prepare`
  - 只负责全局环境准备
  - 例如 toolchain/runtime/bootstrap
- stage reuse
  - 由 subject bucket fingerprint 负责

也就是说：

- `prepare` 是环境级缓存
- `shared/` / `matrices/` 是 subject 执行级缓存

这两层不要混在一起。

## 11. 对报告层的影响

推荐在 `stageResults[]` 里补最少两个字段：

- `executionMode`
  - `executed`
  - `reused`
  - `invalidated`
- `fingerprint`

例如：

```json
{
  "stageId": "analysis-frontend",
  "status": "ok",
  "executionMode": "reused",
  "fingerprint": "ab12cd34ef56"
}
```

这样用户看报告时能立刻知道：

- 这一步到底真的跑了
- 还是沿用了上次产物

## 12. 当前可以正式固定的规则

这次可以正式固定为：

- 第一版采用“稳定工作目录 + fingerprint 比对 + 按需重建”
- 不做第一版历史 fingerprint cache 仓库
- `shared/` 复用前四段
- `matrices/<matrix-id>/build/runtime/report` 不复用
- fingerprint 写进各 bucket manifest
- subject bucket 不新增 `current.json`
- `prepare` 与 subject stage reuse 明确分层

## 13. 这一步之后最自然的下一个问题

如果继续往下讨论，最自然的下一个问题就是：

- `subject.manifest.json` 是否需要显式声明 stage dependency graph

因为一旦复用/失效规则定下来，执行器就需要知道：

- 某个 matrix 到底依赖哪些 stage
- 哪些 stage 是共享前置
- 哪些 stage 是 matrix 独有终段
