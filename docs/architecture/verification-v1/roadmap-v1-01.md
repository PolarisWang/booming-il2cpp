# Verification V1 落地 Roadmap

## 1. 目标

把当前已经确认的 `Verification V1` 架构落成可执行主线，最终替代旧测试路径，形成新的正式验证流程、正式归档路径、统一投影视图和长期 wiki 规范。

最终目标固定为四项：

- 以 `Closure -> ObligationClaim -> EvidenceClaim -> ExecutionFacts -> latest/master/reports` 为唯一 formal 验证主线。
- 用新的 formal source 替代旧的 run-oriented 测试路径与零散报告路径。
- 覆盖 `managed -> aot -> hotupdate` 主线，以及多平台、多设备 profile 的 capability / correctness / lifecycle / perf / packaging 验证。
- 把新的测试流程固化成正式规范写入 `wiki/`，并清理所有旧测试流程入口。

## 2. 范围边界

本 roadmap 覆盖以下内容：

- `build/toolchains/run/testing/` 下的 formal verification DTO、merge、report、projection 生成逻辑
- `docs/testing-inventory/verification/` 的 formal 结果布局
- `subjects/<OwnerPack>/verification/codegen-stubs/` 的正式存根布局
- `docs/testing-inventory/*` 与 `docs/benchmark/*` 的投影切换
- `wiki/06-测试验证/` 的新规范与旧入口退役

## 3. 非目标

本 roadmap 不覆盖：

- 一次性重写全部执行器内部实现
- 历史 run 结果的永久迁移归档
- 为每一个旧页面保留长期兼容层
- 继续维护旧 `pipeline/*` 作为长期 authority

## 4. 阶段列表

### Phase 1 `Formal Foundation`

- `goal`
  - 建立 formal verification 的对象层、归并层和阶段报告层，使新系统先有稳定 source，再谈 UI 和下游入口切换。
- `exit_criteria`
  - formal DTO、merge key、path key、stage report generator 可运行
  - `docs/testing-inventory/verification/latest|master|reports` 可生成最小有效输出
  - `subjects/<OwnerPack>/verification/codegen-stubs/` 可写入正式存根索引
- `deliverables`
  - `verification` contracts / DTO
  - `latest/master` merge engine
  - stage report generator
  - codegen stub materializer
- `dependencies`
  - [`spec.md`](./spec.md)
  - [`schema.md`](./schema.md)
  - [`ownership-map.md`](./ownership-map.md)
- `resolved_decisions`
  - formal source 只认 `verification/` 与 `codegen-stubs/`
  - 不使用 `run-id` 作为 formal 路径
  - `supportState` 与 `verificationState` 分离
- `watch_items`
  - 避免把 `actualDeviceId`、`sessionId`、`attemptId` 混进 formal 主键
  - 避免 stage report 只记录失败项

### Phase 2 `Coverage Graph`

- `goal`
  - 把“怎么 cover 所有测试”从零散目录关系升级为正式 coverage 图：`capability x route x platform x deviceProfile x evidenceKind`。
- `exit_criteria`
  - capability、scenario、closure、route、platform、device profile 之间有稳定映射
  - 每个 declared unit test / benchmark 都能落到 `EvidenceClaim`
  - `unsupported / blocked / missing / not_required` 在 formal 层可区分
- `deliverables`
  - coverage expansion / claim planner
  - unit test inventory row 映射
  - benchmark inventory row 映射
  - lifecycle / packaging / contract scenario 映射
- `dependencies`
  - Phase 1
- `resolved_decisions`
  - coverage 的最小叶子是 `EvidenceClaim`
  - 聚合单元是 `ObligationClaim`
  - `partial` 只出现在聚合层
- `watch_items`
  - 避免只 cover proof / benchmark，漏掉 lifecycle、packaging、contract
  - 避免把 scenario 和物理 device 提前挤进首页

### Phase 3 `Executor Fact Normalization`

- `goal`
  - 让现有 managed / aot / hotupdate 执行器统一回写 execution facts，再被正式规范化为 `EvidenceRecord`。
- `exit_criteria`
  - managed/aot/hotupdate 至少各有一条正式 fact -> evidence -> report 通路
  - success/failure/blocked/missing 都能写进 formal 报告
  - codegen 关键产物能被挂接到对应 evidence
- `deliverables`
  - execution fact adapters
  - evidence normalizer
  - authority / semantic / contract / lifecycle / packaging / perf 阶段落表
- `dependencies`
  - Phase 1
  - Phase 2
- `resolved_decisions`
  - correctness 优先于 perf
  - 结构缺口在 preflight 阶段判 `blocked`
- `watch_items`
  - 避免把旧 matrix report 直接当新 evidence
  - 避免只补 perf，不补 correctness

### Phase 4 `Projection Cutover`

- `goal`
  - 让 `docs/testing-inventory/*`、`docs/benchmark/*` 和主页视图只消费新的 formal source。
- `exit_criteria`
  - inventory / benchmark / dashboard 不再自行推导 authority
  - 表头 tooltip 全部来自 formal schema 元数据
  - 首页视图顺序固定为 `Closure Strip -> Responsibility -> Route Topology -> Platform-Device Matrix -> Detail`
- `deliverables`
  - projection generator
  - inventory generator cutover
  - benchmark dashboard cutover
  - 首页视图数据切换
- `dependencies`
  - Phase 2
  - Phase 3
- `resolved_decisions`
  - `deviceProfileCode` 是 formal 维度
  - 物理设备只在 detail 显示
- `watch_items`
  - 避免 inventory、benchmark 和 formal schema 各写一套状态口径
  - 避免 tooltip 再出现 HTML 内部手写解释

### Phase 5 `Wiki Canonicalization`

- `goal`
  - 把新的测试流程落成正式规范写入 `wiki/06-测试验证/`，让 wiki 从“旧管线入口集合”切到“Verification V1 正式入口”。
- `exit_criteria`
  - `wiki/06-测试验证/` 有新的 canonical 测试流程规范页
  - `wiki/06-测试验证/INDEX.md` 已切换为新入口优先
  - 旧测试管线索引被明确标记为 `legacy`
- `deliverables`
  - 新测试流程规范 wiki 页
  - wiki 索引导航更新
  - 旧 `测试管线` 入口的 deprecation 说明
- `dependencies`
  - Phase 1
  - Phase 4
- `resolved_decisions`
  - docs 记录架构与 rollout，wiki 记录长期 canonical 流程
- `watch_items`
  - 避免 wiki 仍把旧 pipeline 页当主入口
  - 避免把执行日志写进 wiki

### Phase 6 `Legacy Retirement`

- `goal`
  - 清理所有旧测试流程路径、旧 formal 结果路径和旧 wiki 入口，使新流程成为唯一 authority。
- `exit_criteria`
  - 旧 run-oriented formal 路径不再接收新写入
  - 旧 `pipeline/*` 页面不再作为主入口
  - 页面、脚本、索引中不再存在旧路径主引用
- `deliverables`
  - old path removal checklist 完成
  - legacy reporting / pipeline purge
  - 旧 wiki 页面降级或删除
- `dependencies`
  - Phase 4
  - Phase 5
- `resolved_decisions`
  - 不长期双写
  - 旧路径只允许短期桥接，不允许长期 authority 并存
- `watch_items`
  - 删除前必须确认没有下游仍在消费
  - 避免“页面读新数据、脚本还在写旧数据”的半切换状态

## 5. 每阶段完成定义

### Phase 1 完成定义

- formal DTO 有自动化单测
- merge 规则有自动化单测
- stage report 有自动化单测
- codegen stub 路径与索引有自动化单测
- `verification/latest|master|reports` 目录 contract 固定

### Phase 2 完成定义

- 所有 declared unit tests 能映射到 `method x route/stage`
- 所有 declared benchmarks 能映射到 `benchmark x device-profile`
- capability 覆盖表能表达 `supportState + verificationState + blocker`
- `not_required / missing / blocked / failed / passed` 有稳定判定

### Phase 3 完成定义

- managed、aot、hotupdate 三条主线至少各有一条正式 evidence 通路
- lifecycle / packaging / contract 不再只是零散脚本输出
- 成功项与失败项都进入 formal 报告

### Phase 4 完成定义

- `docs/testing-inventory/unit-test-inventory.*` 只读 formal source
- `docs/testing-inventory/benchmark-inventory.*` 只读 formal source
- `docs/testing-inventory/capability-inventory.*` 只读 formal source
- `docs/benchmark/*` 只读 formal source

### Phase 5 完成定义

- wiki 新测试流程规范生效
- wiki/06 索引切换到新入口
- 旧 `测试管线` 索引被明确标记为 legacy

### Phase 6 完成定义

- 所有旧 formal 路径停止写入
- 所有旧测试流程页面完成降级/删除
- 旧脚本/索引/命名不再指向旧主线

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `verification-v1-formal-contracts` | Phase 1 | ready | controller | 落 formal DTO、merge、report、stub contract | `spec/schema` |
| `verification-v1-coverage-expansion` | Phase 2 | planned | controller | 落 capability/scenario/claim 扩张与覆盖图 | `verification-v1-formal-contracts` |
| `verification-v1-fact-normalization` | Phase 3 | planned | controller | 落 execution fact -> evidence 正规化 | `verification-v1-coverage-expansion` |
| `verification-v1-projection-cutover` | Phase 4 | planned | controller | 切 inventory/benchmark/dashboard 到 new formal source | `verification-v1-fact-normalization` |
| `verification-v1-wiki-cutover` | Phase 5 | planned | controller | 落 wiki 新规范并切入口 | `verification-v1-projection-cutover` |
| `verification-v1-legacy-retirement` | Phase 6 | planned | controller | 清理旧 formal path、旧 pipeline、旧 wiki 入口 | `verification-v1-wiki-cutover` |

## 7. 依赖

- 架构 authority：
  - [`spec.md`](./spec.md)
  - [`schema.md`](./schema.md)
  - [`ownership-map.md`](./ownership-map.md)
  - [`projection-and-reporting.md`](./projection-and-reporting.md)
  - [`cutover-and-rollout.md`](./cutover-and-rollout.md)
- 现有代码入口：
  - `build/toolchains/run/testing/inventory_source.py`
  - `build/toolchains/run/testing/inventory_generator.py`
  - `build/toolchains/run/testing/subject_reporting.py`
  - `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- 长期知识入口：
  - `wiki/06-测试验证/INDEX.md`

## 8. 风险

- 旧 reporting 与新 evidence 并存太久，导致 authority 双写
- 只切 inventory/benchmark 页面，不切 formal source，形成倒挂
- 只覆盖 benchmark，不覆盖 lifecycle / contract / packaging
- codegen 存根仍落在临时目录，无法与 owner subject 对照
- `supportState` 与 `verificationState` 被页面层重新揉成一列

## 9. 备选路径

### 方案 A：先页面后 formal

不采用。因为会导致 UI 再定义一套 schema，后续难以收敛。

### 方案 B：长期双写旧路径与新路径

不采用。因为最终一定会造成 authority 分裂和维护负担。

### 方案 C：先 formal、后 projection、最后 retirement

采用。这是当前 roadmap 的唯一推荐路径。

## 10. 当前建议推进顺序

1. `verification-v1-formal-contracts`
2. `verification-v1-coverage-expansion`
3. `verification-v1-fact-normalization`
4. `verification-v1-projection-cutover`
5. `verification-v1-wiki-cutover`
6. `verification-v1-legacy-retirement`

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child: verification-v1-formal-contracts`

自动推进前的最小 `preflight self-review` 固定检查：

- authority 是否有漂移
- closure / merge / path key 是否被修改
- 是否新增未冻结 open question
- 当前 child 的输入依赖是否已满足
- 是否会破坏 `verification/` 与 `codegen-stubs/` 正式路径边界

若结果为：

- `pass`
  - 直接继续
- `warn`
  - 写入子任务 `STATUS.md` 后继续
- `stop`
  - 停止并回到用户确认

## 12. 旧测试路径替代方案

### 12.1 formal 结果路径替代

旧路径思路：

- run-oriented matrix report
- 临时执行目录中的零散 report
- 页面侧自行聚合的 inventory / benchmark 数据

新路径固定替代为：

- `docs/testing-inventory/verification/latest/*`
- `docs/testing-inventory/verification/master/*`
- `docs/testing-inventory/verification/reports/<closure-kind>/<scope-code>/*`
- `subjects/<OwnerPack>/verification/codegen-stubs/*`

### 12.2 pipeline 入口替代

旧 `pipeline/*` 页面统一按三类处理：

- 能提升为 `ClosureTemplate`
  - 转入新 closure 模型
- 能提升为 `ScenarioBase`
  - 转入新 scenario/evidence claim 模型
- 仅是 run-oriented 组合说明
  - 标记 legacy，并在 Phase 6 删除

### 12.3 inventory / benchmark 替代

旧路径：

- `inventory_source -> inventory_generator -> html/csv/json`
- `benchmark_dashboard_generator -> docs/benchmark/*`

新路径：

- `execution facts -> evidence -> latest/master/reports -> projection generator -> docs/testing-inventory/* / docs/benchmark/*`

## 13. 如何 cover 所有测试

新的覆盖模型固定为五维：

```text
capability x route x platform x deviceProfile x evidenceKind
```

具体覆盖要求如下：

### 13.1 correctness 覆盖

- `unit test / proof`
  - 覆盖 `semantic`
- `contract check / collection / manifest / ABI`
  - 覆盖 `contract`
- `load / bind / activate / rollback`
  - 覆盖 `lifecycle`
- `package topology / version / integrity`
  - 覆盖 `packaging`

### 13.2 perf 覆盖

- `declared-benchmark x device-profile`
  - 覆盖 `perf`
- perf 必须依附 capability / route / platform / device profile，不能孤立存在

### 13.3 路由覆盖

- `managed`
  - 最小 correctness 基线
- `aot`
  - correctness + contract + codegen + platform gate
- `hotupdate`
  - correctness + contract + lifecycle + packaging + 必要 perf

### 13.4 状态覆盖

每个叶子 claim 必须显式落为以下之一：

- `passed`
- `failed`
- `blocked`
- `missing`
- `planned`
- `not_required`

并且 `supportState` 独立表达：

- `supported`
- `unsupported`
- `not_applicable`

### 13.5 owner 覆盖

所有 capability 必须能回到 owner subject：

- `SolutionCorePack`
- `MixedExecutionFeaturePack`
- `HotUpdateHostPack`

任何无法归属 owner 的测试项，直接视为 authority 缺口。

## 14. 详细 Checklist

### Phase 1 Checklist

- [ ] 新增 formal verification contracts 模块
- [ ] 新增 `ClosureRecord / ObligationClaimRecord / EvidenceClaimRecord / EvidenceRecord / StageReportRecord`
- [ ] 新增 `latest/master` merge 逻辑
- [ ] 新增 `stage report` 生成逻辑
- [ ] 新增 `codegen stub` 索引与路径物化逻辑
- [ ] 为上述 contract/merge/report/stub 补齐 unit tests

### Phase 2 Checklist

- [ ] 把 declared unit tests 映射到 `method x route/stage`
- [ ] 把 declared benchmarks 映射到 `benchmark x device-profile`
- [ ] 把 contract/lifecycle/packaging 补进 claim expansion
- [ ] 补齐 `unsupported / blocked / missing / not_required` 判定测试
- [ ] 补齐 capability coverage matrix 的 formal 归并测试

### Phase 3 Checklist

- [ ] 定义 execution facts 输入 contract
- [ ] managed 执行结果接入 evidence normalizer
- [ ] aot 执行结果接入 evidence normalizer
- [ ] hotupdate 执行结果接入 evidence normalizer
- [ ] 让 authority/semantic/contract/lifecycle/packaging/perf 形成阶段报告
- [ ] 把关键 codegen 产物挂接到 `codegen-stubs`

### Phase 4 Checklist

- [ ] inventory generator 改为只读 formal source
- [ ] benchmark dashboard generator 改为只读 formal source
- [ ] `unit-test-inventory.*` 切到新 projection
- [ ] `benchmark-inventory.*` 切到新 projection
- [ ] `capability-inventory.*` 切到新 projection
- [ ] 首页五个固定视图接入新 projection 数据
- [ ] tooltip 改为只读 schema 元数据

### Phase 5 Checklist

- [ ] 新增 wiki 新测试流程规范页
- [ ] 更新 `wiki/06-测试验证/INDEX.md`
- [ ] 更新 `wiki/06-测试验证/测试管线/INDEX.md`，标记 legacy
- [ ] 把旧流程与新流程的 authority 切换写清楚
- [ ] 把旧路径清理策略写入 wiki

### Phase 6 Checklist

- [ ] 停止旧 formal 路径写入
- [ ] 删除/降级旧 `pipeline/*` 主入口
- [ ] 删除旧 run-oriented formal report 生成逻辑
- [ ] 删除旧 inventory/benchmark authority 推导逻辑
- [ ] 清理仍指向旧测试主线的 wiki 导航
- [ ] 补齐 legacy retirement 验收测试

## 15. 收尾约束

每个子任务在终态前都必须经过固定链路：

`结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交`

命中 `AOT / IL2CPP / codegen / contracts / loader / test governance` 任一主线时，必须附带最小权责图审核。
