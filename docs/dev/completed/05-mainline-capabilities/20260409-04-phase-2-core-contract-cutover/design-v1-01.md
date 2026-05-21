# Phase 2 Core Contract Cutover 设计 v1.01

Date: 2026-04-09
Status: plan-ready

## 1. 设计目标

把已经冻结的 performance-first contract 从 discuss/roadmap 层切进代码骨架：

- `SemanticWorld` 负责 canonical subject / shape / capability 建模
- `Linker` 负责 retained world 与 dependency closure
- `CodeGen` 显式消费 shape/capability/optimization contract
- `Emitter` 开始基于显式 contract 做最小消费，而不是继续完全靠 sample shape

## 2. contract 设计

### 2.1 `SemanticWorldModel`

新增正式字段：

- `CanonicalSubjects`
- `SemanticShapes`
- `CapabilityBundles`

当前 `Types / Fields / Properties / Methods` 继续保留，作为当前 frozen slice 的 canonical payload：

- `Types / Fields / Properties` 保留 definition-level carrier 角色
- `Methods` 在 `Phase 2` 中提升为 canonical semantic body carrier，不再允许把 semantic-ish normalization 留到 `CodeGen`

### 2.2 `LinkedWorldModel`

把 `SemanticWorld` 的以下 contract 明确 threaded 到下游：

- `CanonicalSubjects`
- `SemanticShapes`
- `CapabilityBundles`

这样 `CodeGen` 的 contract input 就不再只是 `Methods + OptimizationFacts` 的薄壳。

### 2.3 `TypedIlMethodArtifact`

新增显式字段：

- `methodRole`
- `bodyAvailability`
- `capabilities`

目的：

- 让 `typed-il-ir` 不再只是裸 instruction blocks
- 为后续 `Emitter`/`Phase 3` 提供显式的 shape/capability surface

## 3. stage 责任

### 3.1 `SemanticWorldStage`

本阶段正式承接三类职责：

1. 生成 canonical subjects
2. 生成 semantic shapes
3. 生成 method/world capability bundles

同时把当前 frozen slice 已存在的 body canonicalization 前移进来：

- `String.Concat(string,string,string)` -> pairwise concat chain

### 3.2 `LinkerStage`

本阶段保留 retained/dependency 角色，但修正两个边界：

- 不再做 `String.Concat(3)` 的 subjectId 修补
- dependency reason 改为以 declarative policy + method capability 为主，而不是先做字符串归一化再硬编码 reason

### 3.3 `CodeGenStage`

本阶段不再承担 semantic-ish normalization。

新的职责是：

- 消费 `LinkedWorld` 里的 shape/capability contract
- 消费 `OptimizationFacts`
- 生成带显式 shape/capability 信息的 `typed-il-ir`

### 3.4 `NativeReferenceProofEmitter`

本阶段不重写 family selection 主逻辑，但开始使用显式 contract 做基础校验：

- constructor / instance / static / imported method role
- body availability

这样可以把“全靠 instruction shape 猜 method role”的逻辑先往下压一层。

## 4. 最小 vocabulary

### 4.1 canonical subjects

首轮冻结：

- `TypeSubject`
- `FieldSubject`
- `MethodSubject`
- `MemberSubject`

### 4.2 semantic shapes

首轮冻结：

- `TypeShape.kind = reference-type`
- `FieldShape.kind = instance-field`
- `MethodShape.role = constructor | instance-method | static-method | imported-method`
- `MethodShape.bodyAvailability = has-canonical-body | no-canonical-body`

### 4.3 capability bundles

首轮冻结：

- method-level bundle
- world aggregate bundle

当前最小 capability vocabulary：

- `uses-instance-field-state`
- `requires-string-concat`
- `requires-console-string-output`
- `requires-closed-type-member-query`
- `requires-generic-type-definition-query`
- `requires-imported-call`

## 5. 非目标

本阶段不做：

- 完整 `LoweringFamily` legality filter + performance arbitration
- `Emitter` 的全量 plan selection rewrite
- `MainlineFeaturePack` 真正 source/onboarding
- 新 capability family 实现
- `exceptions` 的语义实现展开

## 6. 测试与 contract 更新

需要同时更新：

- `contracts/artifacts/v0/schemas/typed-il-ir.schema.json`
- `contracts/artifacts/v0/samples/typed-il-ir.min.json`
- `contracts/artifacts/v0/snapshots/typed-il-ir.snapshot.json`

测试分两类：

1. artifact contract 回归
   - `tests/unit/run/test_managed_closure_contract_bundle.py`
2. 代码级 cutover 守卫
   - 新增 `tests/unit/run/test_phase2_core_contract_cutover.py`

## 7. 结论

`Phase 2` 采用如下设计：

1. `SemanticWorld` 正式变成 canonical subject/shape/capability layer
2. 当前 frozen slice 的 concat canonicalization 从 `CodeGen` 前移到 `SemanticWorld`
3. `LinkedWorld` 把 semantic contract 明确 threaded 给 `CodeGen`
4. `typed-il-ir` 开始携带显式 method role / body availability / capabilities
5. `Emitter` 只做最小 contract consumption，把完整 lowering arbitration 留给 `Phase 3`
