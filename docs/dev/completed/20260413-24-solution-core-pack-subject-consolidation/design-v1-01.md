# Subject Solution Core-Pack 收敛设计 v1.01

Date: 2026-04-13 08:43:27 +08:00

## 1. 设计背景

仓库已经完成过一轮 `solution-first + attribute-declared hybrid` 重构，但当前 `subjects/` 仍然保留了明显的历史分裂形态：

- 顶层仍有大量 `*Proof`、`Bench*`、`*Lite`、`HelloWorld*` 等 legacy subject root
- pack 化已经存在，但还没有收敛成极少数 retained subject
- “solution 模式” 已经是主线认知，但仓库里实际上还没有显式 `.sln`
- 用户当前的新目标不是继续做中度清理，而是要把 `subjects/` **尽可能压缩到极少数 solution-mode subject**

因此，这次设计不是对现有 hybrid 模型的小修，而是对 retained subject 粒度的进一步收紧。

## 2. 本次设计目标

本次设计只服务一个核心目标：

- 把 `subjects/` 收敛成方案 A 对应的极少数 retained solution-mode subject，其中绝大多数现有 subject 并入单一核心 subject

配套目标如下：

- 统一 retained subject 内部入口
- 统一 retained subject 的 `proof` / `benchmark` 发现与运行方式
- 保证所有 retained solution-mode subject 都能跑通 `proof` 和 `benchmark`
- 让“新增一个测试点”默认不再新增顶层 subject，而是在 retained subject 中新增：
  - 工程级验证条目
  - `ChaosUnitTest`
  - `ChaosBenchmark`

## 3. 非目标

本次设计不追求：

- 一次性支持所有真机/农场自动化
- 一次性把所有工程级 workload 变成源码内 benchmark
- 一次性把所有 retained subject 压成单一 subject

这里最后一条很重要。用户选择的是 “方案 A：极限大合并”，但不是 “把所有 executable plan 都硬塞进一个 subject”。只要 executable plan 明显不同，就允许保留独立 retained subject。

## 4. 当前状态判断

### 4.1 现有 retained 基础

现有最接近 retained subject 的对象包括：

- `MainlineFeaturePack`
- `PerformanceFeaturePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`
- `SolutionSimpleLib`
- `SolutionMultiProject`
- `SolutionPackageReference`

### 4.2 现有 legacy roots

现有明显仍属于 legacy root 的对象包括：

- `AutoBridgeProof`
- `BenchDelegate`
- `BenchException`
- `BenchGameLoop`
- `BridgeRoundtripProof`
- `CompatibilityMatrixProof`
- `EngineHostProof`
- `EngineLifecycleCallbackLite`
- `EngineLogWriteLite`
- `EngineObjectHandleLite`
- `GenericEcho`
- `GenericSupplementProof`
- `HelloWorld`
- `HelloWorldObject`
- `HostEmbeddingLite`
- `MethodReplacementProof`
- `MobileHelloWorldProof`
- `PInvokeLite`
- `ReflectionLite`
- `SharedContractProof`
- `VersionRollbackProof`

### 4.3 当前结构的主要问题

- 顶层 root 与测试目标不一一对应
- 很多 root 只是单个 proof / benchmark 切片，不应继续长期存在
- 外层统一入口已经有了，但 retained subject 内部入口仍风格不一致
- 现有 “solution 模式” 还停留在 project-set，未升级为显式 `.sln`

## 5. 方案 A 的最终结构

### 5.1 Retained subject 清单

本设计把长期保留的 canonical subject 收敛为以下三类：

1. `SolutionCorePack`
2. `HotUpdateHostPack`
3. `MixedExecutionFeaturePack`

说明：

- `SolutionCorePack` 是这次重构的主角，承担极大多数原有 subject 的归宿
- `HotUpdateHostPack` 单独保留，因为它的 executable plan 本质上是 `host-plus-hot-update-patch`
- `MixedExecutionFeaturePack` 单独保留，因为它的 executable plan 本质上是 `mixed runtime / interpreter + native`

除这三类外，不再默认长期保留新的顶层 subject。

### 5.2 为什么不是只保留一个 subject

原因只有一个：executable plan 不同。

- `SolutionCorePack` 主要承担 managed/native solution ingestion、mainline correctness、interop、engine-facing contract、archetype solution、core benchmark
- `HotUpdateHostPack` 需要 host、patch、load、rollback、replacement 等独立链路
- `MixedExecutionFeaturePack` 需要解释执行与混合执行链路

如果把后两类也强行塞进 `SolutionCorePack`，subject 会失去清晰度，入口和 artifact 语义也会变乱。

因此，方案 A 的实际含义是：

- **一个超大的核心 subject**
- **极少数因 executable plan 差异而保留的辅助 subject**

## 6. 迁移映射

### 6.1 迁入 `SolutionCorePack`

以下现有 subject 迁入 `SolutionCorePack`：

| 当前 subject | 迁移后的归属形式 |
| --- | --- |
| `SolutionSimpleLib` | solution slice + engineering validation/workload |
| `SolutionMultiProject` | solution slice + engineering validation/workload |
| `SolutionPackageReference` | solution slice + engineering validation/workload |
| `MainlineFeaturePack` | 核心 feature project + `ChaosUnitTest` |
| `PerformanceFeaturePack` | benchmark project + `ChaosBenchmark` |
| `BenchDelegate` | `ChaosBenchmark` |
| `BenchException` | `ChaosBenchmark` |
| `BenchGameLoop` | `ChaosBenchmark` |
| `GenericEcho` | `ChaosBenchmark` 或 engineering workload，按条目重判 |
| `HelloWorld` | onboarding / smoke slice + `ChaosUnitTest` |
| `HelloWorldObject` | onboarding / object slice + `ChaosUnitTest` |
| `PInvokeLite` | interop slice + `ChaosUnitTest` |
| `ReflectionLite` | interop slice + `ChaosUnitTest` |
| `HostEmbeddingLite` | host/engine slice + `ChaosUnitTest` |
| `EngineHostProof` | host/engine slice + engineering validation 或 `ChaosUnitTest` |
| `EngineLifecycleCallbackLite` | host/engine slice + `ChaosUnitTest` |
| `EngineLogWriteLite` | host/engine slice + `ChaosUnitTest` |
| `EngineObjectHandleLite` | host/engine slice + `ChaosUnitTest` |
| `BridgeRoundtripProof` | bridge/inter-op slice + `ChaosUnitTest` |
| `AutoBridgeProof` | bridge/inter-op slice + `ChaosUnitTest` |
| `GenericSupplementProof` | metadata/generic slice + `ChaosUnitTest` |
| `CompatibilityMatrixProof` | engineering validation 为主，必要时保留 `ChaosUnitTest` |
| `MobileHelloWorldProof` | device slice + engineering validation，host 上按 availability 受控 |

### 6.2 迁入 `HotUpdateHostPack`

以下现有 subject 迁入 `HotUpdateHostPack`：

| 当前 subject | 迁移后的归属形式 |
| --- | --- |
| `HotUpdateHostPack` | retained subject 原位升级 |
| `MethodReplacementProof` | `ChaosUnitTest` |
| `SharedContractProof` | `ChaosUnitTest` |
| `VersionRollbackProof` | `ChaosUnitTest` |

### 6.3 迁入 `MixedExecutionFeaturePack`

以下现有 subject 迁入 `MixedExecutionFeaturePack`：

| 当前 subject | 迁移后的归属形式 |
| --- | --- |
| `MixedExecutionFeaturePack` | retained subject 原位升级 |

说明：

- 现阶段不再额外为 mixed execution 再造新顶层 root
- 若未来出现新的 interpreter-only 单独 executable plan，再单独评估是否需要新 subject

## 7. retained subject 的工程结构

### 7.1 强制显式 `.sln`

所有 retained subject 都升级为显式 solution：

- `subjects/SolutionCorePack/source/SolutionCorePack.sln`
- `subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln`
- `subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln`

这条约束是新的硬边界：

- subject source 不再只是“目录下若干 `.csproj`”
- 而是“一个清晰、可直接被工程级验证消费的显式 solution”

### 7.2 `SolutionCorePack` 推荐布局

`SolutionCorePack` 采用一个 solution、多 project、多 slice 的布局：

```text
subjects/SolutionCorePack/
  subject.manifest.json
  source/
    SolutionCorePack.sln
    Launcher/
    Archetypes/
      SimpleLib/
      MultiProject/
      PackageReference/
    Mainline/
    Interop/
    EngineHost/
    Benchmarks/
    Mobile/
  assets/   # optional
```

设计原则：

- `Launcher/` 提供统一 `Program.cs`
- 不再让每个历史 root 保留自己的顶层 `Program.cs`
- 各 slice 可以有自己的 assembly，但属于同一个 retained subject
- 工程级验证按 solution graph 组织
- 源码内 `proof` / `benchmark` 通过 attribute 挂在对应 project 中

### 7.3 `HotUpdateHostPack` 推荐布局

```text
subjects/HotUpdateHostPack/
  subject.manifest.json
  source/
    HotUpdateHostPack.sln
    Launcher/
    Host/
    Patch/
    SharedContracts/
    Benchmarks/
```

### 7.4 `MixedExecutionFeaturePack` 推荐布局

```text
subjects/MixedExecutionFeaturePack/
  subject.manifest.json
  source/
    MixedExecutionFeaturePack.sln
    Launcher/
    Managed/
    Interpreter/
    Benchmarks/
```

## 8. 统一入口设计

### 8.1 subject 内统一入口

每个 retained subject 只保留一个统一 launcher entry：

- `Program::Main(string[] args)`

`Main` 内不再直接写历史 proof / benchmark 选择字符串，而是只做如下工作：

- 初始化 subject bootstrap
- 按 typed command / typed selector 分发
- 调用 discovered `ChaosUnitTest` / `ChaosBenchmark`
- 或调用 engineering validation / workload entry

### 8.2 类型优先，少字符串

入口协议坚持以下规则：

- 外部 CLI 可以继续接收字符串 subject id
- 进入 retained subject 内部后，尽量转成 enum / compact typed model
- 不在 subject 内部长期传播 “`proof-dev`”“`bench-delegate`”“`lowering-proof`” 这种自由字符串

建议补充的内部枚举：

- `ChaosSubjectEntryKind : byte`
  - `EngineeringValidation`
  - `EngineeringWorkload`
  - `DeclaredUnitTest`
  - `DeclaredBenchmark`
- `ChaosSubjectSlice : byte`
  - `Archetype`
  - `Mainline`
  - `Interop`
  - `EngineHost`
  - `Mobile`
  - `HotUpdate`
  - `MixedExecution`

所有公共枚举都必须带注释。

### 8.3 外层统一入口

外层 `run test ...` 保持统一，但默认用户路径收敛为：

- `run test subject SolutionCorePack ...`
- `run test subject HotUpdateHostPack ...`
- `run test subject MixedExecutionFeaturePack ...`

保留更细粒度入口：

- `run test declared-unit-test ...`
- `run test declared-benchmark ...`
- `run test engineering-validation ...`
- `run test engineering-workload ...`

但这些属于高级入口，不再鼓励普通使用者从 legacy root 心智出发。

## 9. `proof` / `benchmark` 设计

### 9.1 retained subject 全部必须拥有 proof 与 benchmark

这是本次设计新增的强约束：

- 每个 retained solution-mode subject 都必须能跑 `proof`
- 每个 retained solution-mode subject 都必须能跑 `benchmark`

因此：

- `SolutionCorePack` 需要至少一个最小 proof 与一个最小 benchmark
- `HotUpdateHostPack` 需要至少一个 hot-update proof 与一个 hot-update benchmark
- `MixedExecutionFeaturePack` 需要至少一个 mixed proof 与一个 mixed benchmark

如果某 subject 原本只有 proof 没有 benchmark，则新增最小 benchmark 用来验证 benchmark 流程本身，而不是追求业务性能完整性。

### 9.2 工程级 workload 仍然保留在 system-owned 层

以下对象不改成 `ChaosBenchmark`：

- convert
- codegen
- generated native AOT
- native link
- package
- patch generation

它们继续是 engineering workload。

`ChaosBenchmark` 只负责 solution 内部 runtime benchmark。

### 9.3 `GenericEcho` 的处理

`GenericEcho` 是这次迁移的边界样例：

- 若其核心诉求是 runtime perf，则迁成 `ChaosBenchmark`
- 若其核心诉求是 AOT backend / pipeline completeness，则迁成 engineering workload

本设计要求在实施阶段逐条拆解，而不是整包照搬旧 root。

## 10. manifest 与 discovery 约束

### 10.1 继续使用薄 manifest

本次并不回退到 heavy manifest。retained subject 仍然使用薄 manifest，只保留：

- `subjectId`
- `displayName`
- `source`
- `sourceModel`
- `dependencyModel`
- `executablePlan`
- `engineeringProfile`
- `testDeclarationMode`
- `orchestration`
- `availability`
- `compatibility`

### 10.2 `sourceModel` 必须升级为显式 solution

retained subject 的 `sourceModel` 默认应为：

- `dotnet-solution`
- `host-plus-patch`
- `mixed-solution`

不再把 retained subject 长期定义为 project-set。

### 10.3 discovery 继续走 metadata 模型

`ChaosUnitTest` / `ChaosBenchmark` 的发现继续：

- 读 metadata
- 不加载程序集执行
- 不把 discovery 建在 runtime side effect 上

## 11. 验证设计

### 11.1 验证对象

本次 roadmap 收敛后，“所有 solution 模式 subject” 指以下 retained subject：

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

### 11.2 验证要求

每个 retained subject 都要完成：

- `proof` 流程通过
- `benchmark` 流程通过

并输出：

- registry 条目可见
- summary/report 可定位
- rerun command 可复现

### 11.3 平台边界

当前主机环境下的第一验收面优先是：

- `windows-x64`

移动端/device-package slice 可以先作为 `availability != ready` 的 engineering 条目存在，不阻断 retained subject 的 host proof / benchmark 闭环。

## 12. 与挂起任务的关系

以下挂起任务不会直接恢复原路线，而是要被本 roadmap 重挂接：

- `20260412-05-phase-b-generic-native-aot-benchmark-backend`
- `20260411-10-post-phase7-development-roadmap`

处理原则：

- 旧 `Bench*` 心智不再作为恢复入口
- generic native AOT benchmark backend 的成果要拆分到：
  - `SolutionCorePack` engineering workload
  - `SolutionCorePack` declared benchmark

## 13. 验收标准

本设计完成并实施后，必须满足：

- `subjects/` 长期只保留极少数 retained solution-mode subject
- `SolutionCorePack` 成为绝大多数现有 subject 的唯一归宿
- retained subject 全部具备显式 `.sln`
- retained subject 全部具备统一 launcher
- retained subject 全部跑通 `proof` 与 `benchmark`
- 最终阶段一次性删除全部 compatibility alias，不保留长期兼容层
- 新增测试点时，默认不再新增顶层 `*Proof` / `Bench*` root
- 新增一个独立顶层 subject 时，必须证明它拥有新的 executable plan，而不是只是新测试点

## 14. 风险与缓解

### 风险 1：`SolutionCorePack` 过大

缓解：

- 用 solution slice 明确边界
- 用 `ChaosSubjectSlice` 和 assembly 划分内部责任
- 只把顶层 root 合并，不把所有逻辑强行合成单 project

### 风险 2：迁移时 benchmark 边界再次混淆

缓解：

- engineering workload 与 `ChaosBenchmark` 在设计上继续分层
- 旧 `Bench*` 逐条重判，不整包照搬

### 风险 3：统一入口回到字符串分发

缓解：

- retained subject 内部协议强制 typed enum
- 公共枚举全部带注释

### 风险 4：移动端 slice 阻塞 host 验收

缓解：

- device-only 工程条目通过 availability 管理
- retained subject 的 host proof / benchmark 验收优先单独闭环

## 15. 结论

方案 A 在本仓库中的正确落地方式不是 “只保留一个 subject”，而是：

- 建立一个超大的 `SolutionCorePack`
- 保留极少数因为 executable plan 差异无法并入的 retained subject
- 让 retained subject 全部显式 solution 化、入口统一化、proof/benchmark 流程化

这条路线比此前的分域 pack 方案更激进，但与用户当前的整理目标更一致，也更容易让仓库形成稳定心智：

- 默认只往 retained subject 里加内容
- 只有证明自己是新 executable plan，才允许新增顶层 subject
