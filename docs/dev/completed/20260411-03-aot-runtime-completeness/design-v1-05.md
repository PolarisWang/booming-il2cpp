# Design v1-05

## Batch 5 最小实现设计

### 目标

把 Batch 5 收敛为一个明确的 stripping proof slice，使主线第一次具备：

- source 级 `[Preserve]` 声明
- loader 级 preserve 识别
- linker 级 preserve-aware closure
- analysis 级 `preserve-descriptor.json` artifact

### 设计范围

#### Contracts

- `ManagedClosureArtifactNames` 新增：
  - `PreserveDescriptor = "preserve-descriptor.json"`
- managed models 增加最小布尔位：
  - `ManagedTypeModel.IsPreserved`
  - `ManagedFieldModel.IsPreserved`
  - `ManagedPropertyModel.IsPreserved`
  - `ManagedMethodModel.IsPreserved`
- 新增 artifact：
  - `PreserveDescriptorArtifact`
  - `PreserveDescriptorEntry`
- `LinkedWorldModel` 挂出 `PreserveDescriptor`
- `ManagedClosureResult` 挂出 `PreserveDescriptor`

#### Loader

- 复用现有 custom attribute 解码路径
- 新增 `HasPreserveAttribute(...)`
- 识别规则收敛为最小版：
  - 只要 attribute type name 为 `PreserveAttribute`，即视为 preserve
  - 不要求特定 namespace
- type / field / property / method 全部可携带 `IsPreserved`

#### Linker

- 在现有 reachable closure 之后追加 `IncludePreservedClosure(...)`
- preserve 规则：
  - preserved type：
    - 保留 type
    - 保留其 fields / properties
    - 把其 methods 入队，继续走已有 `ExpandReachableMethods(...)`
  - preserved field / property：
    - 保留该 member
    - 保留 declaring type
  - preserved method：
    - 保留该 method
    - 保留 declaring type
    - 方法体继续扩展依赖
- 由 linker 直接构造 `PreserveDescriptorArtifact`
  - type entry：`preserve = "all"`
  - method entry：`preserve = "signature"`
  - field/property entry：`preserve = "members"`
  - `reason = "preserve-attribute"`

#### CodeGen / Driver / Worker

- `CodeGenStage` 把 `linkedWorld.PreserveDescriptor` 写入 `ManagedClosureResult`
- closure manifest 增加 artifact ref：
  - `kind = "preserveDescriptor"`
  - `path = "preserve-descriptor.json"`
- `DriverEntry` 落盘新 artifact
- `subject_workers.py` 的 analysis manifest 挂出 `preserveDescriptorPath`

### 验证策略

先写 RED：
- `tests/unit/run/test_phase2_linker_stripping_proof.py`
- 更新 `test_managed_closure_contract_bundle.py`
- 更新 `test_subject_workers.py`

再做 GREEN：
- `python -m pytest tests/unit/run/test_phase2_linker_stripping_proof.py -v`
- `python -m pytest tests/unit/run/test_managed_closure_contract_bundle.py -v`
- `python -m pytest tests/unit/run/test_subject_workers.py -v`
- `python -m pytest tests/unit/run -v`
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
- `dotnet build subjects/LinkerStrippingProof/source/LinkerStrippingProof.csproj -c Debug`

### 风险

- 以 simple name 识别 `PreserveAttribute` 会比正式产品语义更宽松，但适合当前 proof-first 范围
- 本轮输出的是 preserve descriptor，不是完整 stripped report，不能误报为 `5.3` 已完整闭环

