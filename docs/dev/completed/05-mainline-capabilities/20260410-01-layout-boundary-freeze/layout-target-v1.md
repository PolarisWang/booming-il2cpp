# Layout Target v1 — 终态目录结构与 Ownership 冻结

Date: 2026-04-10
Status: frozen-v1

---

## 1. Ownership 三分类

本文档将仓库中所有目录按以下三类 ownership 归属：

### 1.1 Formal Definition（形式化定义）

承载 contract 的 schema、header、文档。这些文件定义 contract 的接口边界，**不包含任何具体 subject 的数据内容**。

归属路径：
- `contracts/artifacts/<version>/schemas/` — artifact JSON schema
- `contracts/native/<version>/` — native C/C++ header（`codegen_bridge.h`、`runtime_abi.h`）
- `contracts/docs/<version>/` — contract 级别文档

### 1.2 Shared Validation Fixture（共享验证夹具）

承载用于验证 contract 正确性的具体数据文件（sample、snapshot）和编译测试。这些文件不属于某个 subject，而是 contract 本身的验证基线。

归属路径：
- `tests/contracts/analysis/<version>/samples/` — artifact sample JSON
- `tests/contracts/analysis/<version>/snapshots/` — artifact snapshot JSON
- `tests/contracts/native/abi/` — native ABI compile smoke
- `tests/contracts/native/bridge/` — native bridge compile smoke
- `tests/contracts/trace/` — warmup trace 验证脚本与 snapshots

### 1.3 Subject-Owned Content（Subject 自有内容）

每个 subject 自行持有的源码、验证项目、预期输出和基线数据。

归属路径：
- `subjects/<subject-id>/source/` — subject 源码
- `subjects/<subject-id>/validation/` — subject 验证项目
- `subjects/<subject-id>/expected/` — subject 预期输出
- `subjects/<subject-id>/baselines/` — subject 基线数据
- `subjects/<subject-id>/tests/` — subject 专属测试

---

## 2. 终态目录树

### 2.1 `contracts/` — Formal Definition Only

```
contracts/
  artifacts/
    v0/
      schemas/
        aot-manifest.schema.json
        code-registration.schema.json
        metadata-graph.schema.json
        metadata-registration.schema.json
        optimization-facts.schema.json
        preserve-descriptor.schema.json
        typed-il-ir.schema.json
  native/
    v0/
      codegen_bridge.h
      runtime_abi.h
  docs/
    v0/
      overview.md
      codegen-bridge-proof-boundary.md
      runtime-abi-proof-boundary.md
```

**删除项：**
- `contracts/artifacts/v0/samples/` → 迁到 `tests/contracts/analysis/v0/samples/`
- `contracts/artifacts/v0/snapshots/` → 迁到 `tests/contracts/analysis/v0/snapshots/`
- `contracts/native/v0/samples/` → 迁到 `tests/contracts/native/v0/samples/`
- `contracts/examples/` → 删除（已被 samples 完全取代）
- `contracts/native/examples/` → 删除（已被 native samples 完全取代）
- `contracts/native/docs/` → 合并到 `contracts/docs/v0/`

### 2.2 `tests/contracts/` — Shared Validation Fixture

```
tests/contracts/
  analysis/
    v0/
      samples/
        aot-manifest.min.json
        code-registration.min.json
        metadata-graph.min.json
        metadata-registration.min.json
        native-reference.lowering-plan.min.json
        optimization-facts.min.json
        preserve-descriptor.min.json
        typed-il-ir.min.json
      snapshots/
        aot-manifest.snapshot.json
        code-registration.snapshot.json
        metadata-graph.snapshot.json
        metadata-registration.snapshot.json
        optimization-facts.snapshot.json
        preserve-descriptor.snapshot.json
        typed-il-ir.snapshot.json
  native/
    abi/
      CMakeLists.txt
      compile_only_runtime_abi_smoke.cpp
    bridge/
      CMakeLists.txt
      compile_only_codegen_bridge_smoke.cpp
    v0/
      samples/
        abi/
          reflection-handles.cpp
          runtime-lifecycle.cpp
        bridge/
          direct-call-landing.cpp
          invoke-and-delegate.cpp
          registration.cpp
  trace/
    compare-warmup-trace.py
    compare-warmup-trace.ps1
    compare-warmup-trace.sh
    schema/
      warmup-trace.schema.json
    snapshots/
      macos-warmup-trace.snapshot.json
      windows-warmup-trace.snapshot.json
```

**删除项：**
- `tests/contract/` → 删除（legacy 别名，所有引用切到 `tests/contracts/`）

### 2.3 `build/toolchains/run/` — Domain-Based Layout

```
build/toolchains/run/
  __init__.py
  run.py                          # 入口 dispatcher
  runtime.py                      # runtime support
  runtime_manifest.json           # runtime manifest

  core/
    __init__.py
    common.py                     # 公共工具函数
    manifest.py                   # manifest loader
    result.py                     # result model
    operation_reporting.py        # 操作报告
    tooling.py                    # tooling utilities

  ui/
    __init__.py
    menu.py                       # 菜单入口
    menu_entries.py               # 菜单项定义
    menu_flow.py                  # 菜单交互流程
    render_menu.py                # 菜单渲染
    render_progress.py            # 进度渲染
    terminal.py                   # 终端 IO

  commands/
    __init__.py
    bootstrap.py
    build.py
    clean.py
    deploy.py
    doctor.py
    inspect.py
    prepare.py
    project.py
    verify.py
    test/
      __init__.py
      dispatch.py                 # test 命令总入口
      public.py                   # public test dispatch
      registry.py                 # registry object dispatch
      subject.py                  # subject execution dispatch
      reports.py                  # summary / watch
      legacy.py                   # legacy checks

  subject/
    __init__.py
    project_workspace.py          # subject workspace 管理
    runtime.py                    # subject runtime support
    deploy_layout.py              # subject deploy layout

  testing/
    __init__.py
    contracts/
      __init__.py
      contracts.py                # contract validation core
      path_resolver.py            # contract path resolution
      selectors.py                # contract selectors
    registry/
      __init__.py
      catalog.py                  # test catalog
      registry.py                 # test registry
      public_specs.py             # public test specs
      suite_manifest.py           # suite manifest loader
    subject/
      __init__.py
      subjects.py                 # subject test core
      subject_planner.py          # subject test planning
      subject_executor.py         # subject test execution
      subject_reporting.py        # subject test reporting
      subject_validations.py      # subject validations
      workers/
        __init__.py
        common.py                 # worker 公共
        analysis.py               # analysis worker
        build.py                  # build worker
        runtime.py                # runtime worker
        perf.py                   # perf worker
    session/
      __init__.py
      events.py                   # session events
      session.py                  # session management
      reporting.py                # session reporting
      traffic_light.py            # traffic light
    fingerprints.py               # fingerprint utilities
    perf.py                       # perf utilities
    adapters/
      __init__.py
      base.py
      dotnet_app.py
      native_cmake.py
      python_pytest.py
      shell_script.py
      workflow.py

  manifests/
    run_manifest.json             # 组合加载入口（或保留为兼容入口）
    run/
      groups.json                 # command group 定义
      commands.quick-start.json   # quick-start group commands
      commands.build.json         # build group commands
      commands.test.json          # test-and-verify group commands
      commands.environment.json   # environment group commands
      commands.inspect.json       # inspect group commands
      commands.project.json       # project-and-ide group commands
```

### 2.4 `src/managed/Chaos.IL2CPP.CodeGen/` — Lowering Domain Split

```
Chaos.IL2CPP.CodeGen/
  Chaos.IL2CPP.CodeGen.csproj
  CodeGenStage.cs                            # facade / orchestration

  Lowering/
    NativeReferenceLoweringPlanner.cs         # facade: plan selection 入口
    LoweringFamilies.cs                       # lowering family registry
    ShapeMatchers.cs                          # shape match 规则
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
    Validation/
      LoweringValidation.cs

  Emission/
    NativeReferenceProofEmitter.cs            # proof emission facade
    GeneratedTranslationUnitBuilder.cs

  Templates/                                  # Scriban template 文件（保持原位）
    ...existing .sbn files...
```

### 2.5 `src/managed/Chaos.IL2CPP.Loader/` — Decode / Metadata Domain Split

```
Chaos.IL2CPP.Loader/
  Chaos.IL2CPP.Loader.csproj
  LoaderStage.cs                             # facade / orchestration

  Metadata/
    MetadataTypeResolver.cs                  # type resolution
    TypeIdentityProvider.cs                  # type identity
    TypeNameProvider.cs                      # type naming
    SignatureModels.cs                       # signature models

  Decode/
    InstructionDecoder.cs                    # IL instruction decode
    MethodBodyDecoder.cs                     # method body decode
    ReferenceResolver.cs                     # reference resolution

  Materialization/
    GenericMaterializer.cs                   # generic instantiation
    SubstitutionEngine.cs                    # type substitution

  Models/
    MethodReferenceSummary.cs                # method reference model
    FieldReferenceSummary.cs                 # field reference model
    MethodSummary.cs                         # method summary model
    OwnerIndex.cs                            # owner index model
```

### 2.6 `tests/tooling/run/` — 按子系统分目录

```
tests/tooling/run/
  __init__.py

  commands/
    __init__.py
    test_build.py
    test_build_batch.py
    test_prepare_scopes.py
    test_project_command.py
    test_registry_command.py
    test_unified_test_command.py
    test_verify_roadmap0.py

  tui/
    __init__.py
    test_tui.py
    test_tui_routing.py
    test_interactive_session.py

  subject/
    __init__.py
    test_subject_command.py
    test_trace_compare.py
    test_test_watch_summary.py

  contracts/
    __init__.py
    test_contract_check.py
    test_contract_paths.py
    test_python_unittest_command.py

  bootstrap/
    __init__.py
    test_cmake_bootstrap.py
    test_dotnet_bootstrap.py
    test_wrapper_bootstrap_unix.py

  platform/
    __init__.py
    test_platform_gate_validate_only.py
    test_windows_terminal_handoff.ps1
    test_run_host_platform_compat.ps1

  infra/
    __init__.py
    test_command_manifest.py
    test_common.py
    test_doctor.py
    test_json_output.py
    test_operation_reporting.py
    test_runtime_manifest.py
    test_run_cli.ps1
    test_run_cli.sh
```

### 2.7 `tests/unit/run/` — 按子系统分目录

```
tests/unit/run/
  __init__.py

  contracts/
    __init__.py
    test_path_resolver.py
    test_subject_contracts_source_cutover.py
    test_repo_layout.py
    test_managed_closure_contract_bundle.py

  subject/
    __init__.py
    test_subject_planner.py
    test_subject_executor.py
    test_subject_reporting.py
    test_subject_workers.py
    test_subject_workers_perf.py
    test_subject_manifest_schema.py

  registry/
    __init__.py
    test_adapter_registry.py
    test_suite_manifest.py
    test_session.py
    test_events_schema.py
    test_fingerprints.py
    test_traffic_light.py
    test_perf_policy.py

  phases/
    __init__.py
    test_phase2_core_contract_cutover.py
    test_phase3_capability_driven_lowering.py
    test_phase4_mainline_feature_pack_onboarding.py
    test_phase5_capability_batch_a.py
    test_phase6_capability_batch_b.py
    test_phase7_engine_binding_capability.py

  workspace/
    __init__.py
    test_project_workspace.py
    test_deploy_core.py

  native/
    __init__.py
    test_native_prefix_naming.py
    test_native_reference_bootstrap_support.py
    test_native_runtime_core.py
    test_scriban_vendor_build.py

  stages/
    __init__.py
    test_stage3_managed_minimal_closure.py       # legacy shim
    test_stage4_bootstrap_support_skeleton.py     # legacy shim
    test_stage4_native_reference_proof.py         # legacy shim
    test_stage5a_generic_echo_windows_reference.py  # legacy shim
    test_stage5b_reflection_lite_windows_reference.py  # legacy shim
    test_stage5c_pinvoke_lite_windows_reference.py    # legacy shim
```

---

## 3. 命名规则

### 3.1 目录命名

| 层级 | 规则 | 示例 |
| --- | --- | --- |
| 仓库根一级目录 | `kebab-case` 或固定名 | `build/`, `contracts/`, `tests/`, `src/`, `subjects/` |
| Python 包目录 | `snake_case` | `testing/`, `subject/`, `commands/` |
| C# 项目目录 | `PascalCase` | `Chaos.IL2CPP.CodeGen/`, `Lowering/`, `PlanFactories/` |
| C# 子目录 | `PascalCase` | `Metadata/`, `Decode/`, `Emission/` |
| 测试子目录 | `snake_case` | `contracts/`, `subject/`, `registry/` |
| manifest 分片目录 | `kebab-case` | `manifests/run/` |
| contract 版本目录 | 版本号 | `v0/` |

### 3.2 文件命名

| 类型 | 规则 | 示例 |
| --- | --- | --- |
| Python 源码 | `snake_case.py` | `path_resolver.py`, `subject_workers.py` |
| Python 测试 | `test_` 前缀 + `snake_case.py` | `test_path_resolver.py` |
| C# 源码 | `PascalCase.cs` | `LoaderStage.cs`, `MetadataTypeResolver.cs` |
| JSON schema | `kebab-case.schema.json` | `aot-manifest.schema.json` |
| JSON sample | `kebab-case.min.json` | `code-registration.min.json` |
| JSON snapshot | `kebab-case.snapshot.json` | `metadata-graph.snapshot.json` |
| JSON manifest | `kebab-case.json` | `run_manifest.json`, `groups.json` |
| PowerShell | `kebab-case.ps1` | `compare-warmup-trace.ps1` |
| Shell script | `kebab-case.sh` | `compare-warmup-trace.sh` |
| C/C++ header | `snake_case.h` | `codegen_bridge.h`, `runtime_abi.h` |
| C/C++ source | `snake_case.cpp` / `kebab-case.cpp` | 保持现有约定 |

### 3.3 `__init__.py` 规则

- 每个 Python 包目录必须包含 `__init__.py`
- `__init__.py` 默认为空或仅包含必要的 re-export
- 不在 `__init__.py` 中放置业务逻辑

---

## 4. 不动项

以下路径在本轮强规范化重组中**不做变更**：

- `src/native/**` — 不做全面重组
- `docs/dev/completed/**` — 不重排历史文档
- `subjects/<subject-id>/source/**` — subject 源码结构不变
- `artifacts/**` — 构建输出目录不变
- `solutions/**` — 解决方案目录不变
- `third_party/**` — 第三方依赖不变
- `deploy/` — 部署目录不变
- `wiki/**` — wiki 结构不变（但内容可能更新）
- `tests/integration/**` — integration 测试暂不重排
- `tests/registry/**` — 测试注册表暂不重排
- `tests/platform/**` — 平台测试暂不重排
- `tests/gate/**` — 门控测试暂不重排
- `tests/proof/**` — proof 目录暂不重排
- `run.sh`, `run.ps1`, `run.cmd` — 入口脚本不变
- `CMakeLists.txt`, `CMakePresets.json` — CMake 配置仅做路径适配
