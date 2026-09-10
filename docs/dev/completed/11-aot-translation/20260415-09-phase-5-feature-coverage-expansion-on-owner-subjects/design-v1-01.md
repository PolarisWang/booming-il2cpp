# Phase 5 Design v1.01

## 设计目标

- 把 Phase 0 冻结的 capability taxonomy、owner subject、target body availability 落成可执行的测试与 registry contract。
- 每个当前目标 capability item 都能在代码里回答：
  - owner subject 是谁
  - proof / benchmark 是谁
  - support-state 是什么
- capability coverage 审计必须基于新鲜 assembly，不能依赖过期 build 产物。

## 设计边界

- `Chaos.TestFramework` 继续位于 `src/reference/Chaos.TestFramework`；本轮优先在 Python tooling 层聚合 owner/support-state，而不是把测试框架逻辑下沉到 AOT core。
- owner/support-state 的稳定身份继续使用紧凑数值：
  - `ChaosCapabilityFamily : byte`
  - `ChaosCapabilityItem : ushort`
  - `BodyAvailabilityCode : byte`
- coverage 聚合层允许输出标签字符串，但内部主键仍然是数值枚举。

## 落地方向

### 1. capability coverage 模块

- 新增一个 testing 模块，把 Phase 0 冻结结果编码为：
  - family -> owner subject
  - item -> target body availability 集合
  - item -> 是否要求 proof / benchmark
- 输出 capability coverage summary，供 registry / tests 复用。

### 2. registry declared item 扩充

- `declared-unit-test` / `declared-benchmark` registry item 显式带出：
  - `capabilityFamily`
  - `capabilityFamilyLabel`
  - `capabilityItem`
  - `capabilityItemLabel`
  - `ownerSubjectId`
  - `supportStates`
  - `supportStateLabels`
- 这样 owner-subject coverage 不再只停留在 subject 目录或文档里。

### 3. declaration discovery freshness

- `build_subject_declared_test_catalog()` 需要在 solution/project source 更新后触发 rebuild，避免继续消费过期 assembly。
- freshness 规则与 workspace declared catalog 一致：相关 `.cs` / `.csproj` / `.sln` / props/targets 新于产物时，视为 stale。

## 验证方向

- registry 测试验证 declared item 元数据齐全。
- capability coverage 测试验证三类 owner subjects 的当前目标 item 都能映射出 owner/proof/support-state。
- compiled catalog 测试验证 source 更新后会强制重新发现新 capability item。
