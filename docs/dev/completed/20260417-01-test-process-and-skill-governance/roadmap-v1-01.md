# 测试流程与 TestFramework Sdk 化 Roadmap v1.01

## 1. 目标

把当前测试体系重构为以下主线：

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`

并完成：

- `Chaos.TestFramework.Annotation -> Chaos.TestFramework.Sdk`
- `Assert` 与 subject-side authoring API 收敛进 `Sdk`
- `Runtime` 收敛为 execution host surface
- native / hotupdate 执行侧的 manifest 化绑定
- 旧命名、旧注册逻辑、旧双轨文档和旧 fixture 的彻底清理

## 2. 范围边界

本 roadmap 负责：

- `Sdk / Runtime / collector / manifest` 的职责冻结
- test host 生成与消费链路重构
- roadmap 中明确“清理旧写法”与“删除旧资产”的阶段
- 文档、wiki、skill 与 `docs/dev` 主入口更新

本 roadmap 不直接负责：

- AOT Core IR 新能力扩展
- 非测试主线业务功能开发
- 与本次 Sdk 化无关的 benchmark 指标扩展

## 3. 非目标

- 不保留长期 `Annotation` compatibility alias
- 不允许保留“新旧 runner 双轨长期并存”
- 不把 native 符号绑定细节塞进 collection file
- 不把 patch 装载绑定逻辑塞进 `Sdk`

## 4. 阶段列表

### Phase 0：命名与边界冻结

- 冻结 `Sdk / Runtime / collector / manifest` 四层边界
- 把 `Annotation -> Sdk` 改名写入设计、roadmap、architecture、wiki、skill
- 冻结“不保留长期 alias、迁移完成即删旧写法”

### Phase 1：`Sdk` 工程与 subject-side API 切换

- `Chaos.TestFramework.Annotation` 改为 `Chaos.TestFramework.Sdk`
- 把 `Assert`、`ChaosAssertionException`、最小 assertion state 移入 `Sdk`
- 把 subject / managed project 统一切到只引用 `Sdk`

### Phase 2：`.NET 8` collector 与 collection schema 固化

- 固化 `.NET 8` collector 工程、输入输出契约和 schema
- collector 从 managed project 中分析 `Sdk` 声明并生成 collection file
- collection 保持 execution-agnostic，不混入 native/hotupdate 绑定细节

### Phase 3：managed test project 切换

- generated managed test project 改为 `Sdk + Runtime + collection file`
- 删除 managed 侧手工注册与旧发现逻辑

### Phase 4：native test project 切换

- 引入 native dispatch manifest
- native test project 改为“薄宿主 + collection + dispatch manifest”
- 删除 native 侧旧入口、旧手工注册和旧耦合发现逻辑

### Phase 5：hotupdate patch / host 切换

- hotupdate patch project 改为只引用 `Sdk`
- hotupdate test host project 改为 `Sdk + Runtime + collection + binding manifest`
- 删除 hotupdate 侧旧扫描、旧绑定、旧双轨路径

### Phase 6：模板化测试与生成治理

- Python tests 全面模板化
- runner / manifest / host 生成逻辑转 Scriban
- `NativeAotEmitter` 按文件级 Scriban 规则统一

### Phase 7：旧写法清理与收口

- 删除旧项目名、旧 namespace、旧 alias
- 删除旧 fixture、旧模板、旧文档、旧说明
- 删除已经被新主线替代的旧 pipeline 代码

## 5. 每阶段完成定义

### Phase 0

- goal: 冻结新的命名、引用关系和宿主边界
- exit_criteria:
  - 设计、roadmap、architecture、wiki、skill 统一使用 `Sdk`
  - 文档里明确 `Assert` 属于 `Sdk`
  - 文档里明确 native/hotupdate 的处理方式
- deliverables:
  - `design-v1-01.md`
  - `roadmap-v1-01.md`
  - `docs/architecture/managed-native-hotupdate-test-pipeline.md`
  - `wiki/*`
- dependencies: 无
- open_questions:
  - native dispatch manifest 的最终字段粒度
  - hotupdate binding manifest 的最终字段粒度

### Phase 1

- goal: 完成 `Sdk` 工程切换和 subject-side API 下沉
- exit_criteria:
  - managed project 只引用 `Sdk`
  - `Assert` 不再位于 `Runtime`
  - 不保留 `Annotation` 长期 alias
- deliverables:
  - `Chaos.TestFramework.Sdk`
  - 更新后的 subject 引用关系
  - 迁移测试
- dependencies: Phase 0
- open_questions:
  - `Sdk` 中 benchmark authoring API 是否还需要极简上下文类型

### Phase 2

- goal: 让 collection 生成链路独立、稳定、可测试
- exit_criteria:
  - `.NET 8` collector 可从 managed project 生成 collection
  - collection schema versioned 且 enum-first
  - collection 中不包含 native/hotupdate 专有绑定信息
- deliverables:
  - `Chaos.TestFramework.CollectionGen`
  - collection contract tests
- dependencies: Phase 1
- open_questions:
  - build output 分析与 source/project 分析的最终选择

### Phase 3

- goal: managed 执行侧切到 collection-only
- exit_criteria:
  - managed test project 不再扫描 `Sdk`
  - managed test project 不再维护旧注册表
- deliverables:
  - managed test host generator
  - managed runtime contract tests
- dependencies: Phase 2
- open_questions: 无

### Phase 4

- goal: native 执行侧切到薄宿主模式
- exit_criteria:
  - native test project 只依赖 collection + native dispatch manifest
  - native 侧无旧手工注册逻辑
- deliverables:
  - native dispatch manifest generator
  - native host templates
  - native contract tests
- dependencies: Phase 2
- open_questions:
  - native dispatch manifest 的生成时机放在 codegen 阶段还是 host generation 阶段

### Phase 5

- goal: hotupdate 执行侧切到 patch/host 分离
- exit_criteria:
  - patch project 只引用 `Sdk`
  - host project 引用 `Sdk + Runtime`
  - hotupdate 侧无旧扫描/旧绑定逻辑
- deliverables:
  - hotupdate binding manifest
  - hotupdate host templates
  - hotupdate contract tests
- dependencies: Phase 2
- open_questions:
  - patch 入口绑定的最小稳定标识选择

### Phase 6

- goal: 完成模板化测试和生成治理
- exit_criteria:
  - `tests/templates/` 覆盖 managed/native/hotupdate 主线
  - 生成整文件默认走 Scriban
  - `NativeAotEmitter` 纳入统一规则
- deliverables:
  - Python templates
  - Scriban templates
  - 生成链路测试
- dependencies: Phase 3, Phase 4, Phase 5
- open_questions: 无

### Phase 7

- goal: 清理旧写法，保持仓库干净
- exit_criteria:
  - 删除旧 `Annotation` 命名
  - 删除 compatibility alias
  - 删除旧 runner/registry/fixture/doc
  - 代码库只保留新主线
- deliverables:
  - cleanup checklist 完成记录
  - 删除清单
  - 回归测试证据
- dependencies: Phase 1, Phase 3, Phase 4, Phase 5, Phase 6
- open_questions: 无

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260417-01-phase-0-sdk-boundary-freeze` | Phase 0 | completed | codex | 冻结命名、边界、native/hotupdate 处理方式 | `-` |
| `20260417-02-phase-1-sdk-cutover` | Phase 1 | completed | codex | 完成 `Annotation -> Sdk` 与 subject-side API 下沉 | `20260417-01-phase-0-sdk-boundary-freeze` |
| `20260417-03-phase-2-collector-and-schema` | Phase 2 | completed | codex | 完成 `.NET 8` collector 与 collection schema 固化 | `20260417-02-phase-1-sdk-cutover` |
| `20260417-04-phase-3-managed-test-host-cutover` | Phase 3 | completed | codex | 完成 managed test project collection-only 切换 | `20260417-03-phase-2-collector-and-schema` |
| `20260417-05-phase-4-native-test-host-cutover` | Phase 4 | completed | codex | 完成 native 薄宿主与 dispatch manifest 切换 | `20260417-03-phase-2-collector-and-schema` |
| `20260417-06-phase-5-hotupdate-host-cutover` | Phase 5 | completed | codex | 完成 hotupdate patch/host 分离与 binding manifest | `20260417-03-phase-2-collector-and-schema` |
| `20260417-07-phase-6-template-and-scriban-governance` | Phase 6 | completed | codex | 完成模板化测试和 Scriban 统一生成 | `20260417-04-phase-3-managed-test-host-cutover, 20260417-05-phase-4-native-test-host-cutover, 20260417-06-phase-5-hotupdate-host-cutover` |
| `20260417-08-phase-7-legacy-cleanup-and-cutover-removal` | Phase 7 | completed | codex | 删除旧写法、旧命名、旧入口与旧 fixture | `20260417-02-phase-1-sdk-cutover, 20260417-04-phase-3-managed-test-host-cutover, 20260417-05-phase-4-native-test-host-cutover, 20260417-06-phase-5-hotupdate-host-cutover, 20260417-07-phase-6-template-and-scriban-governance` |

## 7. 依赖

- `.NET 8` SDK 可稳定运行 collector
- 当前 subject / test / generator 代码可被分阶段切换
- `Runtime` 的 managed host 协议可以被 native / hotupdate 宿主复用为等价结果协议

## 8. 风险

- 如果保留 `Annotation` alias，后续会长期维持双命名污染
- 如果把 native / hotupdate 绑定信息塞进 collection，schema 会被执行细节污染
- 如果不把 cleanup 单列阶段，旧入口和旧 fixture 会残留并继续被引用
- 如果 `Assert` 仍留在 `Runtime`，subject 侧会继续被迫多引用或维持错误边界

## 9. 备选路径

### 方案 A：`Sdk` 承担 subject-side authoring API，`Runtime` 承担执行宿主

- 优点：最符合“managed project 只引用一个 DLL”的目标
- 缺点：需要一次性迁移命名和断言位置

### 方案 B：保留 `Annotation + Runtime`，再给 subject 额外引入 assertion 程序集

- 优点：语义更细
- 缺点：违反“managed project 只包含一个测试 DLL”的目标

当前采用：方案 A。

## 10. 当前建议推进顺序

1. 先完成 Phase 0，冻结 `Sdk`、native/hotupdate 宿主处理和 cleanup 原则
2. 立即推进 Phase 1，把错误的 `Assert` 边界纠正过来
3. 再推进 Phase 2，把 collector 和 collection 契约固定
4. 然后并行准备 Phase 3 / 4 / 5
5. 最后统一做 Phase 6 和 Phase 7，确保模板与旧写法一起收口

## 11. 清理旧写法 Checklist

- [x] 删除 `Chaos.TestFramework.Annotation` 项目名、目录名、namespace
- [x] 删除旧文档中的 `Annotation` 口径
- [x] 删除 `Runtime.Assert` 或等价旧断言入口
- [x] 删除 managed/native/hotupdate 的手工测试注册逻辑
- [x] 删除旧 runner discovery path
- [x] 删除旧 compatibility alias
- [x] 删除已被模板替代的旧 fixture / old generated sample
- [x] 删除已被 manifest 机制替代的旧绑定逻辑
- [x] 删除旧 wiki / 旧 roadmap / 旧说明中的双轨说法
