# Family Verification Kernel Hard Cut — Design v1

Date: 2026-04-27 00:20:00 +08:00
Status: approved-for-roadmap

## 1. 目标

把 foundation-dll 当前“报表内联计算 + 局部工具拼接”的验证链路，硬切到统一的 family verification kernel。

这次 hard cut 的最终目标不是只修 `native proof` 一列，而是建立一套能长期承载以下 gate 的正式内核：

- `testCode`
- `nativeProof`
- `managedProof`
- `hotupdateProof`
- `benchmark`

并满足：

- capability / family 的验证事实、验证声明、验证计算、投影视图四层彻底分离
- `verify verification-v1` 成为唯一正式刷新入口
- foundation-dll dashboard / DLL detail / projection JSON 全部只读新的 kernel snapshot
- 旧 generator 内联计算逻辑全部删除，不保留长期双轨

## 2. 当前问题

当前实现的结构问题有四类：

### 2.1 事实、声明、计算、展示混层

- artifact 选择
- family 分母映射
- native proof 进度计算
- dashboard/detail 渲染

这些逻辑散在：

- `build/toolchains/run/testing/foundation_dll_audit_generator.py`
- `build/toolchains/run/testing/foundation_dll/gap_analyzer.py`
- `build/toolchains/run/testing/foundation_dll/test_discovery.py`
- 若干 ledger / manifest / coverage artifact

结果是：

- 一改进度口径就要改 generator
- 一改 tooltip/detail 就会碰 formal projection
- 一改 family 分母 authority 就要重写多处本地逻辑

### 2.2 family 级 native proof 没有统一正式对象

当前 `capabilityFamilies[*]` 里没有稳定的“验证内核对象”。
现状只是：

- `verificationGates.native-proof = pending/in-progress/passed`
- `testCode = {...}`
- 新补的 `nativeProof = {...}` 仍属于 generator 内的局部派生字段

这意味着 family 级 `native proof` 还不是 formal kernel 的一等公民。

### 2.3 正式入口与局部 helper 之间边界不稳

现在：

- `test_discovery.py` 还是静态 helper，不是正式 kernel 输入
- `gap_analyzer.py` 只负责 `testCode`
- generator 仍直接碰 `projects.artifacts`

正式刷新虽然已经能产出结果，但结构上仍不干净。

### 2.4 旧逻辑未退场

即使新字段已经出现，只要 generator 里仍存在：

- 直接读取 coverage artifact
- 直接聚合 DLL/project evidence
- 启发式 fallback

那就仍属于双轨逻辑，后续维护成本不会真正下降。

## 3. 目标架构

这次 hard cut 固定引入四个正式对象：

### 3.1 VerificationFact

回答：实际发生了什么。

示例字段：

- `factId`
- `familyId`
- `gateCode`
- `runId`
- `artifactPath`
- `artifactKind`
- `producedAtUtc`
- `sourceKind`
- `payload`

特点：

- 只记录事实
- 不做聚合判断
- 不直接决定 `passed/blocked`

### 3.2 VerificationClaim

回答：这个 family 需要证明什么。

示例字段：

- `claimId`
- `familyId`
- `gateCode`
- `required`
- `denominator`
- `methodSubjectIds`
- `ownerSubjectId`
- `scope`

特点：

- 正式承接 family 分母 authority
- 明确 route/platform/gate 要求

### 3.3 VerificationEvaluator

回答：如何从 fact + claim 算出正式结论。

统一输出：

- `status`
- `numerator`
- `denominator`
- `progressPercent`
- `reason`
- `evidence[]`
- `runs[]`

特点：

- 一个 gate 一个 evaluator
- generator 不再直接做业务判断

### 3.4 VerificationSnapshot

回答：当前正式结论是什么。

建议正式产物：

- `verification/projections/foundation-dll-audit/family-verification.json`

每个 family 至少包含：

- `testCode`
- `nativeProof`
- `managedProof`
- `hotupdateProof`
- `benchmark`

特点：

- dashboard / DLL detail / program.json / dll-matrix.json 全部从 snapshot 派生

## 4. 正式边界

### 4.1 输入边界

新 kernel 允许消费的正式输入：

- `capability-family ledger`
- `native-reference/native-aot` 正式 coverage / audit / manifest artifacts
- `declared` test metadata
- family test project source / build output

不允许：

- 直接把 HTML 视图当 authority
- 直接把 task `STATUS.md` 当 family proof authority

### 4.2 计算边界

以下逻辑必须进入 evaluator，而不是留在 generator：

- latest valid coverage source selection
- family -> methodSubjectIds 映射
- numerator / denominator 计算
- run/evidence 折叠
- blocked/pending/missing 的正式口径

### 4.3 展示边界

generator 只负责：

- 读取 snapshot
- 渲染 JSON projection
- 渲染 HTML

generator 不再负责：

- 选择 artifact
- 推断 family 进度
- 解释 run 是否有效

## 5. Hard Cut 策略

采用“一次硬切、分阶段落地”的方式：

### 5.1 先冻结 authority，再重建内核

必须先冻结：

- family `methodSubjectIds` authority
- latest valid native proof source selection rule
- run selection rule
- family snapshot schema

### 5.2 先接 `testCode + nativeProof`

第一批 hard cut 只覆盖：

- `testCode`
- `nativeProof`

原因：

- 这是当前最痛的链路
- 也是 generator 里最重的混层区域

### 5.3 然后再接 `managed/hotupdate/benchmark`

等 kernel 跑稳后，再把其余 gate 并入同一模型。

### 5.4 最后清理旧逻辑

只有新 snapshot 跑通后，才删除：

- generator 中旧的 artifact 内联解析
- 旧启发式 fallback
- 旧局部临时字段

## 6. 验证要求

这次 hard cut 必须完成四类验证：

### 6.1 单测

- facts builder
- claims builder
- native proof evaluator
- snapshot builder
- projection generator

### 6.2 集成验证

- family test project build/test
- discovery -> snapshot
- snapshot -> projection
- `verify verification-v1 --json`

### 6.3 清理验证

要显式确认旧逻辑是否已完全退场：

- grep 旧 helper 调用
- grep 旧启发式路径
- grep generator 是否仍直接读取旧 coverage 逻辑

### 6.4 dashboard 验证

必须确认：

- dashboard 有真实 family 数据
- DLL detail 有真实 native proof detail
- 不是只有空块或默认值

## 7. 退出条件

只有全部满足才允许宣称 hard cut 完成：

- family verification kernel 正式对象已落地
- `verify verification-v1 --json` 使用新链路
- dashboard / DLL detail 只读 snapshot
- 旧逻辑已清理干净
- 全量 checklist 逐项勾选完成
- review 通过后没有剩余结构 blocker
