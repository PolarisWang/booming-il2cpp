# Contract 去 Subject 内容化 + Repo 结构优化设计选项

## 1. 目标

在原有 contract 目录边界重画之外，补充一层代码与脚本组织优化。

新增目标不是“顺手美化”，而是解决两个已经明显存在的问题:

1. `build/toolchains/run/**` 的职责已经开始堆叠到超长脚本
2. `src/managed` 与 `tests` 有几个目录已经缺乏稳定落点，继续扩展会越来越乱

## 2. 当前问题归类

### 2.1 build 侧问题

`build/toolchains/run/` 当前大致有三层:

- 根目录公共模块
- `commands/`
- `testing/`

但真实职责已经更多:

- manifest/model
- menu / tui
- command dispatch
- subject execution
- contract validation
- registry / reporting / session
- terminal runtime support

结果是几个文件明显过长:

- `build/toolchains/run/tui.py`
- `build/toolchains/run/commands/test.py`
- `build/toolchains/run/testing/subject_workers.py`

### 2.2 managed 侧问题

两个最需要拆的文件:

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`

它们当前都像“超大厨房水槽文件”，包含 facade、规则、helper、model、decode、template、validation 多种职责。

### 2.3 tests 侧问题

`tests/tooling/run` 和 `tests/unit/run` 当前还是单层平铺。

问题不是测试不能跑，而是:

- 同子系统用例不聚合
- 文件一多就难以导航
- 和 source 的对应关系不够清楚

## 3. 方案选项

### 方案 A: 轻量整理

#### 做什么

- 保持现有目录主结构
- 只拆超长文件，不新建太多目录

建议拆法:

`build/toolchains/run/tui.py`

- `tui_menu_entries.py`
- `tui_menu_flow.py`
- `tui_render_menu.py`
- `tui_render_progress.py`
- `tui_terminal_io.py`

`build/toolchains/run/commands/test.py`

- `test_public_dispatch.py`
- `test_registry_dispatch.py`
- `test_subject_dispatch.py`
- `test_summary_watch.py`
- `test_legacy_checks.py`

`build/toolchains/run/testing/subject_workers.py`

- `subject_workers_analysis.py`
- `subject_workers_build.py`
- `subject_workers_runtime.py`
- `subject_workers_perf.py`

`NativeReferenceLoweringPlanner.cs`

- `NativeReferenceLoweringPlanner.cs` 保留 facade
- `NativeReferenceLoweringPlanFactories.cs`
- `NativeReferenceLoweringShapeMatchers.cs`
- `NativeReferenceLoweringTemplateSupport.cs`
- `NativeReferenceLoweringValidation.cs`

`LoaderStage.cs`

- `LoaderStage.cs` 保留 facade
- `LoaderInstructionDecoder.cs`
- `LoaderMetadataResolver.cs`
- `LoaderGenericMaterializer.cs`
- `LoaderTypeProviders.cs`

#### 优点

- 改动集中
- 风险最小
- 能立刻缓解单文件过长

#### 缺点

- 目录层次改善有限
- 很多新文件仍会堆在同一层

#### 适用场景

- 如果你希望这轮以 contract cutover 为主，结构优化只做最低配，这个方案最稳

### 方案 B: 中度重组

这是推荐方案。

#### 3.2.1 build 目录建议

建议把 `build/toolchains/run/` 调整为:

```text
build/toolchains/run/
  core/
    common.py
    manifest.py
    result.py
    operation_reporting.py
    tooling.py
  manifests/
    runtime_manifest.json
    run/
      groups.json
      commands.build.json
      commands.project.json
      commands.test.json
      commands.inspect.json
  ui/
    menu.py
    menu_entries.py
    menu_flow.py
    render_menu.py
    render_progress.py
    terminal.py
  commands/
    build.py
    prepare.py
    project.py
    deploy.py
    test/
      dispatch.py
      public.py
      registry.py
      subject.py
      reports.py
      legacy.py
  subject/
    project_workspace.py
    runtime.py
    deploy_layout.py
  testing/
    contracts/
      contracts.py
      path_resolver.py
      selectors.py
    registry/
      catalog.py
      registry.py
      public_specs.py
      suite_manifest.py
    subject/
      subjects.py
      subject_planner.py
      subject_executor.py
      subject_reporting.py
      subject_validations.py
      workers/
        common.py
        analysis.py
        build.py
        runtime.py
        perf.py
    session/
      events.py
      session.py
      reporting.py
      traffic_light.py
    adapters/
      ...
```

这里的重点不是追求目录多，而是把:

- UI
- command dispatch
- subject execution
- testing helper

从现在的混放状态里拉开。

#### 3.2.2 src/managed 目录建议

`src/managed/Chaos.IL2CPP.CodeGen/`

```text
Chaos.IL2CPP.CodeGen/
  CodeGenStage.cs
  Lowering/
    NativeReferenceLoweringPlanner.cs
    LoweringFamilies.cs
    ShapeMatchers.cs
    PlanFactories/
      ConstructorThenInstanceCallFactory.cs
      StaticCallCtorGetterFactory.cs
      ArrayBoxingReferenceArrayFactory.cs
      DelegateClosedTargetRelayFactory.cs
      ExceptionThrowCatchFinallyFactory.cs
      ReflectionInteropClosureFactory.cs
      PInvokeDllImportMinimalFactory.cs
    Templates/
      TemplateLoader.cs
      TemplateModelBuilder.cs
  Emission/
    NativeReferenceProofEmitter.cs
    GeneratedTranslationUnitBuilder.cs
```

`src/managed/Chaos.IL2CPP.Loader/`

```text
Chaos.IL2CPP.Loader/
  LoaderStage.cs
  Metadata/
    MetadataTypeResolver.cs
    TypeIdentityProvider.cs
    TypeNameProvider.cs
    SignatureModels.cs
  Decode/
    InstructionDecoder.cs
    MethodBodyDecoder.cs
    ReferenceResolver.cs
  Materialization/
    GenericMaterializer.cs
    SubstitutionEngine.cs
  Models/
    MethodReferenceSummary.cs
    FieldReferenceSummary.cs
    MethodSummary.cs
    OwnerIndex.cs
```

关键原则:

- `Stage.cs` 保持 facade
- 规则和 helper 下沉到子目录
- model / provider / decoder 不再塞进一个文件

#### 3.2.3 tests 目录建议

`tests/tooling/run/`

```text
tests/tooling/run/
  commands/
    test_build.py
    test_prepare_scopes.py
    test_project_command.py
    test_registry_command.py
  tui/
    test_tui.py
    test_tui_routing.py
    test_interactive_session.py
  subject/
    test_subject_command.py
    test_trace_compare.py
    test_test_watch_summary.py
  contracts/
    test_contract_check.py
    test_contract_paths.py
    test_python_unittest_command.py
  bootstrap/
    ...
```

`tests/unit/run/`

```text
tests/unit/run/
  contracts/
    test_path_resolver.py
    test_subject_contracts_source_cutover.py
    test_repo_layout.py
  subject/
    test_subject_planner.py
    test_subject_executor.py
    test_subject_reporting.py
    test_subject_workers.py
    test_subject_workers_perf.py
  registry/
    test_adapter_registry.py
    test_suite_manifest.py
    test_session.py
  phases/
    test_phase2_core_contract_cutover.py
    test_phase3_capability_driven_lowering.py
    test_phase4_mainline_feature_pack_onboarding.py
    test_phase5_capability_batch_a.py
    test_phase6_capability_batch_b.py
  workspace/
    test_project_workspace.py
    test_deploy_core.py
```

#### 优点

- 结构显著更清楚
- 后续继续加功能时有明确落点
- 单文件过长和单目录过胖两个问题一起缓解

#### 缺点

- 这轮改动面较大
- 需要同步更新 import、测试发现路径、可能的 `.csproj` include

#### 风险控制

- 先拆 `build/`
- 再拆 `CodeGen/Loader`
- 最后整理 `tests`

不要三边同时大规模 rename。

### 方案 C: 强规范化重组

#### 做什么

- 全仓按 bounded context 重排
- `build/`、`src/`、`tests/` 完全镜像
- manifest 全部目录化
- test 名称也严格镜像 source path

#### 优点

- 最规整
- 长期收益最大

#### 缺点

- 本轮过重
- contract cutover 会被 repo 重构吞掉节奏
- 容易引入大量纯机械改动

## 4. 推荐结论

推荐方案 B。

原因:

- 它能解决当前最明显的热点文件和目录混杂问题
- 不会像方案 C 那样把事情做成大迁坟
- 相比方案 A，它不是只靠“把大文件切一刀”来延缓问题，而是给后续阶段留下稳定结构

## 5. 建议并入当前计划的方式

建议把这部分作为当前计划里的一个“附加工作流”，但设置明确边界:

### 必做

- contract fixture 与 formal definition 分离
- `build/toolchains/run/**` 做中度重组
- `NativeReferenceLoweringPlanner.cs`、`LoaderStage.cs` 拆分
- `tests/tooling/run`、`tests/unit/run` 按子系统分目录

### 可选

- `NativeReferenceProofEmitter.cs` 同轮拆分
- `run_manifest.json` 改为分片 manifest 目录

### 暂缓

- `src/native/**` 大规模重组
- 全仓 namespace 风格统一
- completed 文档目录重排

## 6. 如果你要我继续推进

你可以直接从下面三档里选:

1. 选方案 A: 只拆超长文件，不大改目录
2. 选方案 B: 中度重组，作为当前计划推荐解
3. 选方案 C: 强规范化重组，但我会单独拆成更大的 roadmap

如果你不特别指定，我后续会按方案 B 继续写实施计划。
