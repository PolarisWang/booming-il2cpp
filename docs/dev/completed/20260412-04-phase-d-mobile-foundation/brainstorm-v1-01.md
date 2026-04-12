# Phase D 移动端基础设施收口 Brainstorm

## 背景

父 roadmap 已明确 Phase D 目标是 Android/iOS 真机验证与跨平台 benchmark，但当前环境是 Windows，无法给出 `macOS + Xcode` 的 iOS 真机或模拟器证据。旧任务 `20260411-05-mobile-runtime-host` 也已被明确挂起，只保留外部环境 follow-up。

因此本子任务只处理当前环境可完成、且能真实降低后续 Phase D 成本的部分。

## 用户意图

- 继续推进主线，不停在分析。
- 不伪造 iOS 运行证据。
- 先把当前机器能做完的 Phase D 基础设施做扎实。

## 可选方案

### 方案 A：恢复旧 mobile host 任务继续做

不推荐。

原因：
- 旧任务的剩余项主要是 `macOS + Xcode` 证据，当前环境仍不可执行。
- 继续在同一任务里推进会把“可做基础设施”和“外部环境 blocker”混在一起，状态会失真。

### 方案 B：仅写文档，不改代码

不推荐。

原因：
- 父 roadmap 已经给出明确的 Phase D 代码面缺口。
- 只补文档无法降低后续接入成本，也不能形成新的自动化验证面。

### 方案 C：新建一个 Windows 可执行的 Phase D 子任务，落地基础设施

推荐。

原因：
- 可以把可落地项独立完成并验证。
- 可以明确把 Phase D 剩余问题压缩成真实设备与 macOS/Xcode 证据，而不是代码欠账。

## 本轮收口范围

1. iOS 热更新分发策略文档
2. `IosComplianceValidator` 与 `PackageValidator` 集成
3. `mobile_perf_collector.py` 与 `run_mobile_native_perf()` worker
4. Android/iOS 热更新入口胶水与相关 CMake 连线
5. 至少 4 个 benchmark subject 的移动端 perf matrix 扩展

## 不在本轮范围

1. iOS 真机或模拟器运行证据
2. Android 真机 benchmark 实测数据
3. Phase D 全量归档

## 结论

采用方案 C，直接进入 `writing-plans`，并以 TDD 方式执行。
