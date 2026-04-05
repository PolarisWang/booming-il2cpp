# Stage 3 Managed Minimal Closure Design

Date: 2026-04-06 00:13:36 +08:00
Status: ready-for-plan

## 设计目标

打通 `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 的最小 managed 闭环，使其能从 `HelloWorldObject.dll` 产出 Stage 4 可消费的 generated-code input bundle。

## 设计边界

### 纳入

- `Chaos.IL2CPP.Driver`
- `Chaos.IL2CPP.Loader`
- `Chaos.IL2CPP.SemanticWorld`
- `Chaos.IL2CPP.Pipeline`
- `Chaos.IL2CPP.Linker`
- `Chaos.IL2CPP.MetadataWriter`
- `Chaos.IL2CPP.CodeGen`
- `tests/proof/input/HelloWorldObject/HelloWorldObject.csproj`
- Stage 3 自己的 closure spec / task doc / tests

### 不纳入

- native runtime ABI / codegen bridge 的真实执行
- reference platform 运行
- 非 `HelloWorldObject` 的第二条 sample

## 关键设计结论

1. Stage 3 的输出边界固定为 managed 侧 generated-code input bundle，而不是 generated C++ 可执行结果。
2. bundle 必须能够追溯并对齐到 Stage 2 的 canonical truth：
   - `typed-il-ir`
   - `aot-manifest`
   - `metadata-registration`
   - `code-registration`
3. Stage 3 要求“真实 load 真实 assembly”，但允许在 proof-bound 范围内窄化语义：
   - 只支持 `HelloWorldObject`
   - 只支持 `Program / Greeter / _name / 3 methods`
   - 只支持 Stage 2 已冻结的 helper / dependency set
4. `Driver` 只承担 orchestration，不承担额外 CLI 协议扩张；当前阶段更适合做可测试的 managed API 入口。
5. `Loader` 至少负责读取：
   - assembly path
   - assembly identity
   - proof 所需 type / field / method metadata
6. `SemanticWorld` 负责把 loader 结果转成 proof-bound world model，而不是直接把 metadata token 流裸传给后续阶段。
7. `Linker` 负责基于 entry subject 和 Stage 2 canonical dependency set 得到 retained world。
8. `MetadataWriter` 与 `CodeGen` 共同负责形成 closure bundle：
   - `MetadataWriter`: `aot-manifest`、`metadata-registration`
   - `CodeGen`: `typed-il-ir`、`code-registration`、bundle manifest

## 输出形态

Stage 3 的推荐输出根目录：

- `artifacts/proof/managed-closure/HelloWorldObject/`

推荐最小文件集合：

- `typed-il-ir.json`
- `aot-manifest.json`
- `metadata-registration.json`
- `code-registration.json`
- `closure.manifest.json`

其中 `closure.manifest.json` 只承担 Stage 3 自己的聚合入口职责，不重定义 Stage 2 已冻结的字段语义。

## 测试策略

1. Python 单测先锁 Stage 3 入口与输出约束。
2. managed 项目 build 必须全部通过。
3. proof input 项目 build 必须通过。
4. Stage 3 产出的 bundle 至少要能和 Stage 2 canonical truth 做关键字段比对。

## 设计结果

Stage 3 默认以单一子任务推进，不再继续拆成更细 roadmap。它的职责是把“真实 assembly 输入”变成“真实 managed closure 输出”，而把“真实 native proof”明确留给 Stage 4。
