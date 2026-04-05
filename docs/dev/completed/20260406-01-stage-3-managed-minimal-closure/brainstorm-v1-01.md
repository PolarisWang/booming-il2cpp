# Stage 3 Managed Minimal Closure Brainstorm

Date: 2026-04-06 00:13:36 +08:00
Status: draft

## 任务意图

这个子任务负责把现有 managed skeleton 从“只有项目和占位类名”推进到真正可流动的最小闭环，但闭环只到 managed 侧 generated-code input bundle，不提前踏进 Stage 4 的 native runtime 证明。

## 为什么先做 Stage 3

- Stage 2 已经把 `HelloWorldObject` 的 canonical truth、artifact 真源和 direct-call landing 约束写死。
- Stage 4 的 native bootstrap 需要一个稳定的 managed 上游输入。
- 如果现在继续把 Stage 3 / Stage 4 混在一起，会把 managed boundary、native boundary 和 proof gate 再次揉成一团。

## 本任务聚焦

- `Chaos.IL2CPP.Driver`
- `Chaos.IL2CPP.Loader`
- `Chaos.IL2CPP.SemanticWorld`
- `Chaos.IL2CPP.Linker`
- `Chaos.IL2CPP.MetadataWriter`
- `Chaos.IL2CPP.CodeGen`
- `Chaos.IL2CPP.Pipeline`
- 仅面向 `HelloWorldObject` 的最小 closure 输出

## 明确不做

- native `runtime-core -> bootstrap -> support`
- reference platform 真跑 `Hello, World!`
- follow-on sample
- 大而全的泛化 world / linker / codegen 能力

## 备选方案

### 方案 A：Stage 3 直接做到 generated C++ + native bootstrap

- 优点：看起来离最终 proof 更近
- 缺点：会和 Stage 4 直接重叠，重新失去阶段边界

### 方案 B：Stage 3 结束在 managed 侧 generated-code input bundle

- 优点：与 roadmap 当前定义一致，能给 Stage 4 一个稳定输入面
- 缺点：需要为“bundle 长什么样”补一份明确 spec

### 方案 C：把 Stage 3 再拆成 Loader/World 与 Writer/CodeGen 两个子任务

- 优点：每段更短
- 缺点：现在 skeleton 太薄，再拆会制造更多状态管理成本

## 当前推荐

- 采用 **方案 B**

## 补充判断

Stage 3 不应完全硬编码 Stage 2 artifact 文件，而应从编译后的 `HelloWorldObject.dll` 真实读取 metadata，再在 proof-bound 范围内收窄成最小 world / retained set / writer output。

也就是说：

- 不做完全通用 pipeline
- 但也不接受“直接把 Stage 2 JSON 原样拷贝成输出”这种伪闭环
