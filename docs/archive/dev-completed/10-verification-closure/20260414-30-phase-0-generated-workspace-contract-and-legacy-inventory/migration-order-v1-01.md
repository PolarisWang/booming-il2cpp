# Migration Order v1.01

Date: 2026-04-14 16:34:34 +08:00
Status: frozen-for-phase-1

## 1. 总体顺序

固定迁移顺序：

1. 先冻结 contract 与 cleanup inventory
2. 再把 declared catalog 接到 generated managed host
3. 再生成 workspace manifest v2 和四类工程
4. 再切 unified test entry / registry / executor
5. 再收 benchmark runner 与 dashboard
6. 最后删除旧 host、旧字段、旧 shim 与旧测试壳

禁止逆序：

- 不允许先删旧 host，再补 generated host
- 不允许先切 test entry，再补 workspace manifest v2
- 不允许 benchmark dashboard 继续围绕 workloadEntry 做新增逻辑

## 2. Phase 1: Declared Catalog To Managed Test Host

### 2.1 目标

把 attribute discovery 真正落到 managed test host，不再依赖 retained subject 手写注册。

### 2.2 checklist

- 给 declared catalog 增加 `entryIndex`
- 冻结 family 内 `stableId -> entryIndex` 编号规则
- 生成：
  - `ChaosGeneratedDeclaredTests.g.cs`
  - `ChaosGeneratedDeclaredBenchmarks.g.cs`
- 生成 managed proof host / benchmark host `.csproj`
- 建立 `stableId -> entryIndex` 与 `entryIndex -> invoke` 映射
- 补 RED 测试：
  - 无 framework reference
  - framework referenced but no declarations
  - `testDeclarationMode=require` 但无 entry
  - catalog entryIndex 稳定性

### 2.3 不做

- 不改 workspace manifest 写入逻辑
- 不切 CLI 公共入口
- 不开始删 retained subject Program host

### 2.4 阶段出口

- 任一 subject 的 declared proof / benchmark 可生成 managed host
- generated host 内部不再依赖 `sourceEntry` 文本做主分发

## 3. Phase 2: Workspace v2 And Native Test Project Generation

### 3.1 目标

把 workspace 生成从旧 native target 模型提升到四类工程模型。

### 3.2 checklist

- 在 `project_workspace.py` 写出 `workspaceVersion: 2`
- 根级写出：
  - `managedProjects`
  - `managedTestProjects`
  - `nativeProjects`
  - `nativeTestProjects`
  - `matrices`
- 从 generated managed host 翻译并生成 native test project
- 为 matrix 建立 project-id 引用，而不是 `proofNativeProjectPath`
- build selector 升级为面向四类工程
- subject solution / IDE 打开入口同步对齐

### 3.3 兼容策略

- 允许保留一次性 v1 读取兼容
- 不允许继续写入旧字段

### 3.4 阶段出口

- `run generate project subject` 可以生成四类工程
- `workspace.manifest.json` 不再写入旧 native proof 字段

## 4. Phase 3: Unified Test Entry And Registry Cutover

### 4.1 目标

让 `run test` / registry / executor 全部优先消费 workspace manifest v2 与 generated host。

### 4.2 checklist

- `commands/test.py` 优先按 workspace manifest v2 解析 host
- registry 对 declared object 不再依赖 `sourceEntry/workloadEntry` 选 matrix
- worker 不再新增 `--chaos-source-entry=` 写入路径
- planner fingerprint 从 `sourceEntry/workloadEntry` 迁到：
  - `entryIndex`
  - `stableId`
  - workspace host resolution
- engineering validation / workload 接到统一 artifact/report spine

### 4.3 保留项

- retained subject 的 `ChaosSubjectEntryArguments` 可短期兼容读取
- 但不再允许新增新的 slice/member/new selector

### 4.4 阶段出口

- `run test subject`
- `run test declared-unit-test`
- `run test declared-benchmark`

三条路径都能通过 workspace manifest v2 定位执行宿主。

## 5. Phase 4: Benchmark Native Runner And Dashboard Closure

### 5.1 目标

让 benchmark 从“能跑”升级到“结果正确、identity 稳定、页面可读”。

### 5.2 checklist

- benchmark runner 从 workloadEntry 迁到 declared benchmark identity
- dashboard 用 `stableId` 作为主 identity
- managed baseline 成为主对比基线
- 页面 drill-down 到 solution 内单个 declared benchmark 条目
- 对缺失数据输出明确原因码，而不是静默空白
- 清理 `benchmark_dashboard_generator.py` / `perf_dashboard.py` 的 workloadEntry 兼容映射

### 5.3 阶段出口

- managed/native benchmark 数据成对可见
- dashboard 不再依赖 workloadEntry 作为主匹配键

## 6. Phase 5: Subject Migration And Legacy Deletion

### 6.1 目标

删掉所有已被新主链替代的旧 surface。

### 6.2 checklist

- 删除：
  - `chaos_subject_reference_proof`
  - `proofNativeProjectPath`
  - `generatedNativeProjectPath`
  - `defaultOpenNativeProject`
  - `defaultBuildNativeProject`
  - `nativeProjectPath`
- 删除 subject 内手写 host：
  - `subjects/SolutionCorePack/source/Launcher/Program.cs`
  - `subjects/HotUpdateHostPack/source/Host/Program.cs`
  - `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
- 收缩或删除 migration shim：
  - `ChaosSourceEntryArguments`
  - `ChaosSubjectEntryArguments`
  - `ChaosSubjectEntryKind`
  - `ChaosSubjectSlice`
- 删除 run bucket shim 文件
- 更新 `tests/unit/run/test_repo_layout.py`
- 把真实 subject 测试的断言目标迁到：
  - declared catalog
  - workspace manifest v2
  - generated host contract
  - benchmark dashboard contract
- 对 archetype solution：
  - 保留真实 solution source
  - 去掉把 `Program::Main` 当验证协议的测试依赖

### 6.3 阶段出口

- 仓库内不再保留仅为历史入口服务的 host / manifest 字段 / shim tests
- retained subject 的测试目标收敛到“真实 source + attribute + generated host”

## 7. 贯穿式约束

所有阶段共同遵守：

- 不新增 compatibility alias
- 不新增 string-based dispatch surface
- 优先使用闭集枚举 / 数值索引，字符串只留在 JSON / CLI 输入边界
- subject 目录只保留必要 source，不回流生成 glue code
