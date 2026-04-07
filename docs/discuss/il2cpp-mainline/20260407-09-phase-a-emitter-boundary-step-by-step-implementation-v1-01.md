# Phase A Emitter Boundary Step-By-Step Implementation v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

我们已经在原则上确定了目标：

- `LoweringFamily` 作为 emitter 的正式输入
- `TemplateKind` 暂时不升成正式 contract
- emitter 只允许保留：
  - plan 完整性校验
  - backend invariant 校验
- emitter 不再负责：
  - lowering 识别
  - shape 推理
  - 语义发现

现在真正的问题变成：

- 这件事在仓库里到底怎么一步一步改

## 2. 问题的具体描述

当前真实调用链是这样的：

1. `PipelinePlan.Execute(...)`
2. `CodeGenStage.Generate(...)`
   - 产出：
   - `typed-il-ir.json`
   - `aot-manifest.json`
   - `metadata-registration.json`
   - `code-registration.json`
   - `closure.manifest.json`
3. `DriverEntry.Run(emit-native-reference ...)`
4. `NativeReferenceProofEmitter.Generate(...)`
   - 重新读取：
   - `typed-il-ir.json`
   - `metadata-registration.json`
   - `code-registration.json`
   - `closure.manifest.json`
   - 然后在 emitter 内部：
   - 识别 lowering family
   - 组装 lowering plan
   - 选择模板
   - 渲染 generated cpp

也就是说，当前 emitter 还是：

- `planner + validator + renderer`

而目标状态应该是：

1. `PipelinePlan.Execute(...)`
2. `CodeGenStage.Generate(...)`
   - 已经产出 lowering plan
3. `DriverEntry.Run(emit-native-reference ...)`
4. `NativeReferenceProofEmitter.Generate(...)`
   - 只读取 lowering plan
   - 校验 plan 完整性 / backend invariant
   - 选模板
   - 渲染 generated cpp

目标状态下，emitter 应该是：

- `validator + renderer`

不是：

- `planner + validator + renderer`

## 3. 有哪些可解决的路径

### 路径 A：一次性大搬迁

做法：

- 直接把 `CreateLoweringPlan(...)` 整体从 emitter 挪到 `CodeGenStage`
- 直接让 emitter 改成只读 lowering plan
- 一次性改完 tests、artifact contract、driver 写盘逻辑

优点：

- 结果最干净
- 中间过渡最短

缺点：

- 改动面大
- 容易同时打断：
  - Stage 4
  - Stage 5A
  - Stage 5B
  - Stage 5C

判断：

- 不推荐作为第一步

### 路径 B：两段式兼容迁移

做法：

第一段：

- 先把“lowering 规划能力”从 emitter 内部抽成独立 planner
- 让 `CodeGen` 和 `Emitter` 都能调用它

第二段：

- 让 `CodeGen` 成为唯一 lowering plan 生产者
- 让 emitter 改成只消费 lowering plan
- 再逐步删除 emitter 里的 shape detection / lowering discovery

优点：

- 风险最可控
- 便于逐步收缩 emitter 职责
- 更适合当前 proof spine 已经存在、且测试较多的仓库状态

缺点：

- 过渡期会存在一点“职责双轨”
- 短期会多出一次中间适配

判断：

- 推荐

### 路径 C：最小包装迁移

做法：

- 不挪逻辑
- 只是给 emitter 现有逻辑套一个“planner”名字
- 文义上说 emitter 消费 lowering plan
- 实际还是 emitter 自己现场生产

优点：

- 改动最小

缺点：

- 基本没有真正解决问题
- 只是换名，不是改边界

判断：

- 明确不建议

## 4. 我的方案和理由

我推荐：

- **路径 B：两段式兼容迁移**

因为它最符合当前仓库的成熟度，也最容易让每一步都可验证。

下面我按真正可施工的顺序展开。

### 第 0 步：先冻结目标状态，不急着写代码

先明确最终职责：

- `CodeGen`
  - 负责选择 `LoweringFamily`
  - 负责组装 `LoweringPlan`
- `Emitter`
  - 负责读取 `LoweringPlan`
  - 负责 plan 完整性校验
  - 负责 backend invariant 校验
  - 负责模板选择与渲染

这一条不先冻结，后面的迁移很容易一边搬一边变形。

### 第 1 步：把 lowering 规划逻辑从 emitter 里抽成独立 planner

目标：

- 先不改变功能
- 只改变“逻辑放在哪”

建议新增一个独立类，例如：

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`

把当前 emitter 里这部分逻辑先搬过去：

- `CreateLoweringPlan(...)`
- `TryCreatePInvokeDllImportMinimalLoweringPlan(...)`
- `TryCreateReflectionQueryMinimalLoweringPlan(...)`
- `CreateStaticCallCtorGetterLoweringPlan(...)`
- `CreateConstructorThenInstanceCallLoweringPlan(...)`
- 与 lowering 识别直接相关的 `Is...Shape(...)`
- 与 lowering 规划直接相关的 `Validate...Shape(...)`

这一步的关键不是“优化逻辑”，而是先把 planner 和 renderer 物理分开。

受影响文件：

- [NativeReferenceProofEmitter.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs)
- 新增 `NativeReferenceLoweringPlanner.cs`

验收标准：

- 现有 Stage 4 / 5A / 5B / 5C 输出不变
- 只是 emitter 不再自己内联定义 planner 逻辑

### 第 2 步：让 `CodeGenStage` 也能生产 lowering plan

当前 `CodeGenStage.Generate(...)` 已经拿到了足够多的输入：

- `linkedWorld`
- `metadataWriterOutput`
- `codeRegistration`

所以它理论上已经具备生产 lowering plan 的条件。

建议在这一步：

- `CodeGenStage.Generate(...)`
  - 调用 `NativeReferenceLoweringPlanner`
  - 直接生成 `NativeReferenceLoweringPlanArtifact`

同时扩展 contract：

- 在 [ManagedClosureContracts.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs)
  - 给 `ManagedClosureArtifactNames` 增加一个 lowering plan artifact 名
  - 建议单独命名，例如：
  - `native-reference.lowering-plan.json`
- 在 `ManagedClosureResult`
  - 增加 lowering plan 产物
- 在 `ManagedClosureManifestArtifact.Artifacts`
  - 增加 `nativeReferenceLoweringPlan` 引用

为什么建议不要直接复用 `native-proof.plan.json` 这个名字：

- 因为那是 native proof 输出目录里的名字
- 而现在我们要把“plan 的真源”前移到 managed closure 侧
- 用新名字更利于表达职责变化

受影响文件：

- [CodeGenStage.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs)
- [ManagedClosureContracts.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs)

验收标准：

- managed closure root 内出现 lowering plan artifact
- `closure.manifest.json` 正式引用它

### 第 3 步：让 `DriverEntry.Run(ManagedClosureRequest)` 把 lowering plan 写盘

当 `ManagedClosureResult` 已经带 lowering plan 后，需要把 driver 写盘逻辑补上。

建议修改：

- [DriverEntry.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs)

在 `Run(ManagedClosureRequest request)` 里增加：

- 写出 lowering plan 到 managed closure root

这样，后续 `emit-native-reference` 就有了真正应该消费的输入。

验收标准：

- 执行 managed closure 命令后
- managed closure 输出目录里能看到 lowering plan 文件

### 第 4 步：把 emitter 改成“只读取 lowering plan”

这是最关键的一步。

当前 `NativeReferenceProofEmitter.Generate(...)` 会读：

- `typed-il-ir.json`
- `metadata-registration.json`
- `code-registration.json`
- `closure.manifest.json`

然后自己生成 lowering plan。

这一步改成：

- 只读：
  - `closure.manifest.json`
  - managed closure root 里的 lowering plan artifact
- 不再从 `typed-il` 识别 family
- 不再调用 planner

也就是说：

- 删掉 emitter 中对 `CreateLoweringPlan(...)` 的依赖
- `Generate(...)` 直接变成：
  1. 读取 lowering plan
  2. 做白名单校验
  3. 选择模板
  4. 渲染代码
  5. 输出 native proof manifest 和 generated sources

受影响文件：

- [NativeReferenceProofEmitter.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs)

验收标准：

- emitter 即使拿不到 `typed-il`，也能工作
- 只要 lowering plan 完整，它就能渲染

### 第 5 步：把 emitter 的 validation 收缩成白名单

这一步不是“删光所有校验”，而是“改校验性质”。

保留的校验：

- lowering plan 是否存在
- `LoweringFamily` 是否受支持
- 必要的 roles / tokens / symbols / literals 是否齐全
- 当前 backend/template 是否支持该 family
- 当前 host 是否满足硬条件
  - 例如 `pinvoke` 当前要求 Windows host

删除的校验：

- `IsStaticCallCtorGetterEntryPointShape(...)`
- `ValidateConstructorThenInstanceCallEntryPointShape(...)`
- `ValidateFieldGetterShape(...)`
- 任何基于 instruction 序列重新推导 family/role 的逻辑

这一步的实质是：

- emitter 只校验“plan 能不能渲染”
- 不再校验“这个 plan 是不是从语义上推导正确”

后者应属于上游。

### 第 6 步：保留 native 输出目录中的 `native-proof.plan.json`，但把它降级为镜像产物

这是一个非常实用的兼容步骤。

因为当前 tests 和 proof host wiring 都默认在 native 输出目录下找：

- `native-proof.plan.json`

所以在 `Phase A` 迁移期间，不建议立刻把这个文件拿掉。

建议改成：

- managed closure root 里的 lowering plan 是真源
- native output root 里的 `native-proof.plan.json` 是镜像/复制产物

这样既不打断现有 proof/tests，又完成了“plan 真源前移”。

受影响文件：

- [DriverEntry.cs](D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs)
- 相关 tests

### 第 7 步：把测试分成三层补齐

这一轮要补的不是一个测试，而是一组测试。

第一层：managed closure 层

- 验证 lowering plan 已经在 managed closure root 生成
- 验证 `closure.manifest.json` 引用了 lowering plan

第二层：native proof 层

- 验证 emitter 只靠 lowering plan 就能生成 `generated/native-reference.generated.cpp`
- 验证 native output root 仍然保留 `native-proof.plan.json`

第三层：负向测试

- plan 缺少必要 role 时失败
- family 不受当前 backend 支持时失败
- host invariant 不满足时失败

建议重点关注这些测试：

- [test_stage4_native_reference_proof.py](D:/agent/booming-il2cpp/tests/unit/run/test_stage4_native_reference_proof.py)
- [test_stage5a_generic_echo_windows_reference.py](D:/agent/booming-il2cpp/tests/unit/run/test_stage5a_generic_echo_windows_reference.py)
- `Stage5B/Stage5C` 对应测试

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

这个问题本身是合理的，而且必须现在问。  
因为如果只停留在“原则上 emitter 应该退回后端”，却不把实现顺序讲清楚，那最后很容易变成：

- 大家都同意边界
- 但没人敢真正动迁移

不过这个问题也要避免一个误区：

- 不要把它理解成“只是在重构一个类”

它其实是在改三件事：

1. artifact 真源位置
2. stage 职责边界
3. 验证责任的归属

所以更合理的整体解法，不是只改 emitter，而是同步遵守三条原则：

### A. 先改“真源归属”，再改“调用归属”

也就是先让 lowering plan 在 managed closure 侧成为真源，再让 emitter 改成只消费它。  
如果顺序反过来，迁移会很乱。

### B. 过渡期保留镜像产物，不要急着清 compatibility surface

例如：

- native output root 下的 `native-proof.plan.json`

这类文件在迁移期应保留为兼容镜像，而不是立刻消失。

### C. 让测试跟着职责边界重排，而不是只修断言

也就是：

- managed closure 测试负责证明“plan 已生成”
- emitter 测试负责证明“plan 可被消费”
- host/backend 测试负责证明“平台硬条件校验仍然正确”

这比只改几处 JSON 断言更重要。

这份文档真正想说明的是：

- 这不是一句抽象原则
- 而是一条可以按 7 个步骤渐进落地的 Phase A 改造路径
