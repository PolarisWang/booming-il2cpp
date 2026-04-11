# Brainstorm v1-05

## 背景

Batch 5 同时包含两条线：
- linker stripping
- debug baseline

当前主线已经具备隐式 stripping 能力：`LinkerStage` 先做 reachable closure，再由后续阶段只消费 `LinkedWorldModel`。这意味着“不可达主体不会进入 registration”其实已经成立，但仍缺三件事：
- 没有显式 `Preserve` 入口
- 没有把 preserve 结果产出为正式 artifact
- 没有一个独立 proof subject 锁定这条能力边界

相比之下，debug baseline 需要新项目、新 metadata surface，切入成本更高。

## 候选方案

### 方案 A：先做 debug metadata 空壳

- 优点：
  - 能尽早打开 `Chaos.IL2CPP.Debug` 入口
- 缺点：
  - 需要新项目与 artifact surface
  - proof 价值弱，容易只得到一层空骨架

### 方案 B：先把 stripping 升级为 Preserve-aware proof slice

- 内容：
  - 新建 `LinkerStrippingProof`
  - loader 识别 `[Preserve]`
  - linker 把被 preserve 的 type/member 纳入 closure
  - 主线输出 `preserve-descriptor.json`
- 优点：
  - 建立一条从 source attribute 到 artifact 的可验证闭环
  - 直接把“隐式 stripping”提升为“可声明、可观察的 stripping”
  - 复用仓库里已经存在但尚未接线的 `preserve-descriptor` contract
- 缺点：
  - 还不是完整的 stripping report

### 方案 C：直接输出 stripped 列表 report

- 优点：
  - 更贴近 Batch 5 原始条目 `5.3`
- 缺点：
  - 需要先定义“剥离原因”和“剥离粒度”
  - 在没有 `Preserve` 入口时，report 的业务价值不如 preserve 闭环明确

## 选择

选择方案 B。

## 最小 proof slice

- subject：`LinkerStrippingProof`
- source 形态：
  - 本地 `PreserveAttribute`
  - 一个不可达但带 `[Preserve]` 的 type
  - 一个不可达且未 preserve 的 type，作为对照
- pipeline 目标：
  - loader 识别 `PreserveAttribute`
  - linker 把 preserve type 及其 members 纳入 closure
  - 输出 `preserve-descriptor.json`
  - closure manifest / worker manifest 接上该 artifact

## 非目标

- 本轮不新建 `Chaos.IL2CPP.Debug`
- 本轮不做 IL offset -> C++ line mapping
- 本轮不输出完整 stripped subjects report
- 本轮不处理 XML descriptor / 外部 preserve 配置

