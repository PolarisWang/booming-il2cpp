# Subject 测试框架重构落地路线 Brainstorm v1.01

Date: 2026-04-06
Status: design-approved-pending-roadmap

## 1. 目标

这次 brainstorm 不是重新讨论测试框架“该不该重构”，而是把已经散落在 `docs/archive/discuss/` 下的设计结论，收成一条可以真正进入实施前评审的路线：

- 明确推荐的实施路径
- 明确哪些阶段可以先落
- 明确哪些阶段必须后置
- 明确进入 `plan` 前必须拍板的关键问题

## 2. 本轮输入

本轮以 `docs/archive/discuss/` 现有结论为输入，尤其以以下文档为锚点：

- `docs/archive/discuss/test-framework/20260406-03-test-framework-reboot-design-v1-02.md`
- `docs/archive/discuss/helloworldobject-windows/20260406-05-helloworldobject-windows-matrix-design-v1-01.md`
- `docs/archive/discuss/helloworldobject-windows/20260406-06-helloworldobject-windows-stage-worker-design-v1-01.md`
- `docs/archive/discuss/subject-framework/20260406-08-first-implementation-cutover-order-v1-01.md`
- `docs/archive/discuss/subject-framework/20260406-15-subject-manifest-schema-draft-v1-01.md`
- `docs/archive/discuss/subject-framework/20260406-17-planner-output-design-v1-01.md`
- `docs/archive/discuss/subject-framework/20260406-18-executor-stage-io-boundary-design-v1-01.md`
- `docs/archive/discuss/reporting/20260406-20-reporting-schema-design-v1-01.md`
- `docs/archive/discuss/reporting/20260406-21-event-schema-alignment-design-v1-01.md`

这些文档已经把对象模型基本定出来了：

- `subject`
- `goal`
- `environment-matrix`
- `artifact-chain`
- `planner`
- `executor`
- `reporting`
- `events`

当前缺的是：

- 从“设计对象”到“实施路径”的收口

## 3. 当前代码现实

虽然设计对象已经比较完整，但当前代码现实仍明显停留在旧体系：

- 还没有 `subjects/` 目录
- `HelloWorldObject` 仍位于 `tests/proof/`
- `build/toolchains/run/testing/registry.py`
  - 仍围绕 `suite / module / system / pipeline`
- `build/toolchains/run/testing/session.py`
  - 仍围绕 `suiteResults`
- `build/toolchains/run/commands/test.py`
  - 仍以 public suite/registry object 为执行入口
- `build/toolchains/run/testing/contracts.py`
  - 仍读取 `analysis/contracts`
- `build/scripts/verify-roadmap-0.py`
  - 仍是 monolithic workflow 脚本

也就是说：

- 新设计不是在“增量补一两个字段”
- 而是在替换测试框架的中轴

## 4. 三条候选落地路线

## 4.1 方案 A：内部执行脊柱优先，public surface 后置

做法：

1. 先把内部对象落地：
   - `subjects/`
   - `subject.manifest.json`
   - artifact bucket
   - planner / executor / worker / reporting / events
2. 让 `HelloWorldObject` 先在新脊柱上跑通
3. 之后再接 public CLI / registry / TUI

优点：

- 与当前设计顺序一致
- 能先解决真正危险的内部隐式耦合
- 不会过早冻结外部接口

问题：

- 前期用户可见收益较少
- 短期内新旧入口会并存

## 4.2 方案 B：长时间双栈并行

做法：

- 旧 `suite/registry/session` 和新 `subject/planner/executor` 长时间并列存在
- 两边都保持较完整 public surface

优点：

- 迁移时心理上更稳
- 旧工具兼容成本短期更低

问题：

- 模型漂移风险最大
- 会长期维护两套 registry / report / event 语义
- 实际实现量明显膨胀

## 4.3 方案 C：public entry 先行

做法：

- 优先做 `run test subject ...`
- 优先做 subject registry / TUI 入口
- 内部 worker/artifact/stage 再慢慢补

优点：

- 外部感知最强

问题：

- 会把未稳定的内部实现提前暴露成 public contract
- 后续拆 Stage 4 / 拆 monolithic script 时回改成本最高

## 5. 推荐路线

推荐采用：

- **方案 A：内部执行脊柱优先，public surface 后置**

原因：

- 现在最危险的不是“命令怎么叫”
- 而是：
  - `subjects/` 还不存在
  - Stage 4 仍有隐式链路
  - `contracts` 仍未切到 canonical source
  - `reporting/events` 还没真正接到 subject 语义上

在这些内部边界没稳定前，越早做 public surface，后面返工越大。

## 6. 推荐的实施切面

如果采用方案 A，实施切面建议固定为：

### 第一层：对象与文件系统

- 创建 `subjects/HelloWorldObject/`
- 明确 `shared/`、`matrices/`、`subject-report/` 目录
- 切 `contracts/` canonical source

### 第二层：执行脊柱

- 抽出 planner 最小输出
- 抽出 executor 最小输入输出
- 抽出三类核心 worker
- 拆掉 Stage 4 CMake / `verify-roadmap-0.py` 的隐式 orchestration

### 第三层：验证闭环

- 让 `HelloWorldObject` 的 Windows 首批 matrix 在新脊柱上跑通
- 先覆盖：
  - `windows-dev-output`
  - `windows-reference-trace`
  - `windows-android-buildable`
  - `windows-linux-buildable`

### 第四层：聚合与兼容

- 接上 matrix report / subject summary / session summary
- 接上 matrix / subject / session events
- 仅对 session 保留 watch/TUI 兼容投影

### 第五层：public cutover

- 再做 public CLI / registry / TUI
- 处理旧 `roadmap-0` / old registry object 的退役或桥接

## 7. 这套路线最核心的风险

### 风险 1：双模型漂移

如果新 `subject` 模型和旧 `suite/registry/session` 长时间同权并行，最终会维护两套真相。

### 风险 2：脚本/构建层继续暗箱执行

如果 Stage 4 CMake 和 `verify-roadmap-0.py` 不先拆，worker/reporting 再漂亮都只是表面层。

### 风险 3：首批范围再次膨胀

用户目标里包含：

- `correctness.dev`
- `correctness.platform`
- `perf.dev`
- `perf.release`

但如果第一份实现计划同时覆盖四类目标，几乎必然失控。

### 风险 4：public surface 过早冻结

如果先做 CLI / registry / TUI，后面内部对象一变，外部接口就得跟着重构。

### 风险 5：parallel 设计任务与 active 主线失联

本任务当前是并行设计任务。如果后面不把它回收回 active reboot 主线，就会形成两个决策源。

## 8. 进入 plan 前必须确认的问题

我认为真正重要、会影响后续计划边界的只有下面几项：

### 问题 1：本并行任务的归属

选项：

- A. 这条并行任务只做设计；进入 `plan` 前必须回收到 `20260405-01-il2cpp-reboot-after-abandoned-roadmap`
- B. 这条并行任务后续保持独立，不再回收到 active reboot 主线

推荐：

- **A**

原因：

- 设计可以并行
- 但真正进入实施时，最好只有一个主路线图

### 问题 2：首份实现计划是否明确排除性能维度

选项：

- A. 首份 plan 只做 correctness / platform buildable 与框架骨架，不碰 `perf.dev` / `perf.release`
- B. 首份 plan 直接同时纳入性能

推荐：

- **A**

原因：

- 性能目标需要稳定 stage/reporting 之后才有意义
- 现在先把性能纳入第一计划，会直接拉大 blast radius

### 问题 3：`subjects/HelloWorldObject/` 是否第一阶段就物理落地

选项：

- A. 第一阶段就创建 `subjects/HelloWorldObject/`，旧 `tests/proof` 只保留兼容壳
- B. 先保持 `tests/proof` 真源，`subjects/` 后移

推荐：

- **A**

原因：

- 你前面已经明确希望 subject 成为唯一正式顶层对象
- 如果第一阶段还不物理落地，后面所有 planner/executor/reporting 路径都会继续漂

### 问题 4：public entry 是否后置

选项：

- A. 先 internal-first，CLI / registry / TUI 后置
- B. 在第一份 plan 里就把 public entry 一起做掉

推荐：

- **A**

原因：

- 这是当前最能控制风险的方式

### 问题 5：Windows `verify-roadmap-0` gate 在重构期间的地位

选项：

- A. 继续保留为 mandatory non-regression gate，但只作为 compatibility gate，不再扩义
- B. 重构期间先降级，不作为强 gate

推荐：

- **A**

原因：

- 当前它已经真实消费 proof 结果
- 这是现阶段最直接的非回归锚点

## 9. 结论

这轮 brainstorm 的结论是：

- 可以进入 `roadmap`
- 但**还不应该直接进入 `plan`**

原因不是对象模型没想清楚，而是：

- 进入 `plan` 前还有几项真正影响边界的关键问题必须先拍板
