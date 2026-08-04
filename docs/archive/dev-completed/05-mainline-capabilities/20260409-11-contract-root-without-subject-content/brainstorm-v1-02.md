# Contract 根目录去 Subject 内容化 Brainstorm v1.02

## 新增诉求

在原有 contract 目录去 subject 内容化之外，用户追加了一层 repo 整理诉求:

- 调整 `src/`、`tests/`、`build/` 的目录结构
- 对明显过长、不合理的脚本或源码文件做拆分
- 让脚本组织更清楚，后续继续扩展时不再堆到单文件里

这意味着本轮方案不再只是路径迁移，还需要决定“整理力度”。

## 当前观察到的热点

### build / tooling

当前最明显的脚本热点:

- `build/toolchains/run/tui.py` 约 1650 行
- `build/toolchains/run/commands/test.py` 约 1389 行
- `build/toolchains/run/testing/subject_workers.py` 约 978 行
- `build/toolchains/run/project_workspace.py` 约 647 行
- `build/toolchains/run/run_manifest.json` 约 925 行

这些文件都不是单一职责:

- `tui.py` 同时承担菜单建模、参数交互、渲染、终端 IO、progress 视图
- `commands/test.py` 同时承担 public test dispatch、registry object dispatch、summary/watch、subject 执行、contract check、python unittest
- `subject_workers.py` 同时承担 analysis、build、runtime、perf、trace worker

### src / managed

当前最明显的源码热点:

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs` 约 2036 行
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs` 约 1899 行
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs` 约 1165 行
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs` 约 642 行

这些文件的职责也已经明显混合:

- `NativeReferenceLoweringPlanner.cs` 同时包含 shape match、plan selection、template load、template model、validation、token helper
- `LoaderStage.cs` 同时包含 assembly load、type load、IL decode、metadata resolve、generic materialize、type provider 与 record model

### tests

当前热点主要集中在:

- `tests/tooling/run/`
- `tests/unit/run/`

两个目录都在不断变胖，但仍然全部平铺在单层目录里，导致:

- 同类测试不聚合
- 找 test 很慢
- 改某个子系统时，测试范围边界不直观

## 可选方案

### 方案 A: 轻量整理

只做最小必要的结构优化:

- 保持大部分目录不动
- 只拆 3 个最明显的大脚本:
  - `tui.py`
  - `commands/test.py`
  - `subject_workers.py`
- `src/managed` 只拆 2 个最明显的大文件:
  - `NativeReferenceLoweringPlanner.cs`
  - `LoaderStage.cs`
- `tests/tooling/run` 与 `tests/unit/run` 暂时不分子目录，只增加命名约束

优点:

- 成本最低
- 对现有 import / namespace / test discovery 影响最小

缺点:

- 目录层次问题基本没解决
- 一两轮开发后还会重新变乱

### 方案 B: 中度重组

按“职责域”把 `build/`、`src/managed/`、`tests/` 都整理到更稳定的结构，但不做激进 rename。

核心动作:

- `build/toolchains/run/` 按 `ui / commands / manifests / testing / runtime-support` 收口
- `build/toolchains/run/testing/` 再按 `subject / contracts / registry / reporting / adapters` 切子目录
- `src/managed/Chaos.IL2CPP.CodeGen/` 把 lowering/planning/template/rendering helper 分目录
- `src/managed/Chaos.IL2CPP.Loader/` 把 loader facade、instruction decode、metadata resolve、generic materialize、type provider 拆文件
- `tests/tooling/run/`、`tests/unit/run/` 改成按子系统分目录
- `run_manifest.json` 拆成目录化 manifest 片段

优点:

- 结构清楚很多
- 后面继续加功能时有稳定落点
- 风险仍然可控，不需要全仓 rename 一遍

缺点:

- 会碰较多 import / project include / test path
- 需要一轮完整回归

### 方案 C: 强规范化重组

把 `build/`、`src/`、`tests/` 全部统一成镜像式 bounded context 布局。

示意:

- source 按 context 分
- tests 完全镜像 source
- manifest/config 也全部目录化

优点:

- 最整齐
- 后续长期维护收益最大

缺点:

- 本轮成本和风险都明显偏高
- 会把 contract 迁移任务和 repo 重构任务耦合得太紧

## 推荐

推荐方案 B。

原因:

- 方案 A 只是在灭火，不足以解决当前已经成型的职责混杂
- 方案 C 对本轮任务太重，容易把“contract 去 subject 内容化”主线拖慢
- 方案 B 能把热点文件拆开，并给未来扩展预留稳定目录，不至于一次重构过头

## 建议加入当前计划的范围边界

如果把这部分并入当前计划，建议只做以下整理，不无限扩大:

- `build/toolchains/run/**`
- `src/managed/Chaos.IL2CPP.CodeGen/**`
- `src/managed/Chaos.IL2CPP.Loader/**`
- `tests/tooling/run/**`
- `tests/unit/run/**`

不建议本轮动:

- `src/native/**` 的大结构
- 所有历史 `docs/dev/completed/**`
- 与当前 contract cutover 无关的大规模 namespace rename
