# Codegen / TPG / Python 模块边界与修复原则

> 补充 `Chaos.IL2CPP.Generator`、`TestProjectGenerator`、`verification/` Python 管线的正式职责分工。

## 四层架构总览

```
┌────────────────────────────────────────────────────────────┐
│ 1. Codegen (C#) — Chaos.IL2CPP.Generator                   │
│    路径: src/managed/Chaos.IL2CPP.Generator/                │
│    产出: native-aot.generated.cpp / .header.h / .page-*.cpp │
│          enum_metadata.generated.h                          │
│          chaos_generated_module.h / .cpp                    │
│    职责: IL→C++ 翻译、类型声明、方法体生成、对象模型发出     │
│    地位: 这是 PRIMARY 代码生成层，所有 C++ 输出应在此层正确   │
├────────────────────────────────────────────────────────────┤
│ 2. TPG (C#) — TestProjectGenerator                          │
│    路径: src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/ │
│    模板: Templates/*.scriban                                │
│    产出: CMakeLists.txt, entry.cpp/h, dispatch.cpp,         │
│          runtime-entry.cpp, chaos-config/targets cmake,     │
│          metadata/subjects.json                             │
│    职责: 测试项目脚手架生成（非 IL→C++ 翻译）                │
│    规则: 模板逻辑改 .scriban，emitter 逻辑改 C# emitter     │
├────────────────────────────────────────────────────────────┤
│ 3. Python Pipeline                                          │
│    路径: testing/foundation-dll/verification/               │
│    核心文件:                                                 │
│      stages/pipeline_native_aot_runner.py  — 编排管线       │
│      stages/test_code_generator.py         — C# 测试代码生成 │
│      stages/native_code_generator.py       — 原生骨架生成    │
│      orchestration/family_entrypoint.py    — family 入口生成 │
│    职责: 编排（调用 codegen → TPG → cmake build）、          │
│          数据流、测试配置、C# 测试项目代码生成                │
│    禁止: 后处理修复 C++ 输出（remediate_* 模式）             │
├────────────────────────────────────────────────────────────┤
│ 4. Native Runtime (C++)                                      │
│    路径: src/native/                                         │
│    职责: GC、dispatch、interpreter、类型系统等运行时支持       │
│    规则: codegen 输出引用 runtime 符号时必须保持一致          │
└────────────────────────────────────────────────────────────┘
```

## 各层产出物归属

| 产出文件 | 所属层 | 生成方式 |
|---------|--------|---------|
| `native-aot.generated.cpp` | Codegen | NativeAotLoweringPlanner |
| `native-aot.generated.header.h` | Codegen | BuildTypeDeclarationsCode() |
| `native-aot.page-*.cpp` | Codegen | BuildGeneratedPage() |
| `enum_metadata.generated.h` | Codegen | EnumMetadataExtractor |
| `chaos_generated_module.h/.cpp` | Codegen | BuildGeneratedModuleHeader/Source |
| `CMakeLists.txt` | TPG | .scriban template |
| `CMakePresets.json` | TPG | .scriban template |
| `entry.cpp/h` | TPG | .scriban template |
| `dispatch.cpp` / `verification_dispatch.generated.cpp` | TPG | .scriban template |
| `runtime-entry.cpp` | TPG | .scriban template |
| `chaos-config.cmake` / `chaos-targets.cmake` | TPG | .scriban template |
| `metadata/subjects.json` | TPG | CppProjectEmitter |
| `verification-report.json` | Python | pipeline_native_aot_runner |
| `entry.exe` / `libentry.a` | 构建系统 | cmake + native compiler |

## 修复时的判断标准

### 症状 → 应修改的层

| 症状 | 应修改 | 不应修改 |
|------|--------|---------|
| codegen 输出了错误 C++ 语法/语义 | **Codegen (C#)** | Python remediate |
| codegen 输出的 struct 定义不完整 | **Codegen (C#)** | Python 后处理补齐 |
| codegen 输出了未声明的符号引用 | **Codegen (C#)** | Python 后处理插入声明 |
| codegen 漏掉了类型/方法 | **Codegen (C#)** | — |
| CMakeLists.txt 构建配置错误 | **TPG / .scriban** | Python f-string |
| 测试项目结构不对 | **TPG / .scriban** | — |
| 管线编排逻辑（调用顺序、参数传递） | **Python** | — |
| 测试数据/配置变更 | **Python** | — |
| C# 测试代码生成变化 | **Python (test_code_generator)** | — |
| Runtime 符号/ABI 不匹配 | **Native Runtime (C++)** | codegen 或 Python |

### 硬性规则

1. **remediate_* 是红灯** — 任何 Python `remediate_` 函数的出现都意味着 codegen emitter 存在 bug，必须修复 emitter 而非新增 remediate。历史案例见 Phase 2 优化战役。
2. **Python 不做 C++ 后处理** — Python 层只做编排和测试配置，不修改 .cpp/.h 文件内容。
3. **Scriban 模板是 TPG 的一部分** — 修改测试项目模板时改 .scriban 文件，不写 Python f-string。
4. **Codegen emitter 是全权责任人** — Codegen 产出的每个 .cpp/.h 文件必须是自包含、语法正确、语义完整的 C++，不依赖任何后处理。

## 历史教训

Phase 2 优化战役中清理了 5 个 `remediate_*` Python 后处理函数（合计 ~700 行），根因全部是 codegen emitter 的 bug：

| remediate | 根因 | 修复 |
|-----------|------|------|
| `remediate_delegate_struct_definitions` | delegate 前向声明不足 | 在 header 中发出完整平铺 struct 定义 |
| `remediate_supplemental_codegen` Step B | 缺失 `chaos_type_id_*` extern 声明 | emitter 扫描并发出 |
| `remediate_supplemental_codegen` Step C | `constexpr` 初始化非法 extern 符号 | 改为 `const` |
| `remediate_supplemental_codegen` Step E | 无枚举时错误发出注册 IIFE | 检查 header 内容 |
| `remediate_supplemental_codegen` Step D | `_sN` 槽深度低估 | 与 slotContext 取较大值 |

**根因**：历史上团队图快在 Python 层打补丁，导致 codegen emitter 长期带病运行，每次 runtime 升级都引发 remediate 连锁失效。

## 合约文件统一化

所有 family 的合约文件已完成从 `contract.json` 到 `capability-family-contract.json` 的统一迁移：

- **文件名统一**：152 个 legacy `contract.json` 已迁移，所有 family 仅使用 `capability-family-contract.json`
- **集中查找**：`verification/orchestration/context.py` 提供 `resolve_contract_path()` 和 `load_contract()` 作为唯一合约查找入口
- **消费者清理**：`codegen.py`、`pre_verification_audit.py`、`discovery.py`、`family_entrypoint.py` 中所有双重文件名检查已替换为集中式调用
- **测试修复**：`test_preflight.py`、`test_context.py`、`test_engine.py` 中的测试夹具已更新为新文件名

## Stage Registry 完整性

`stages/__init__.py` 的 `STAGE_REGISTRY` 已补充所有缺失阶段（`managed_fact`、`cross_verify`），与 `engine.py` 保持一致。当前注册 16 个阶段。

## Sentinel Dispatch 验证

`codegen.py` 新增 `_is_sentinel_dispatch()` 验证，TPG emit 失败或 sentinel 存根残留时显式报错（而非静默降级通过）。

## Config Tier 默认值修复

CMake 模板中 `config_tier` 默认值从 `"debug"` 改为 `"check"`，与 CLI 保持一致。未知值触发 `message(WARNING)` 警告。

## build_call_expr 统一

新增 `build_call_expr_safe()` 统一入口，替代 `build_call_expr_for_benchmark` 和 `build_call_expr_for_semantic_patch` 中的重复安全逻辑（auto-callable 检查、ref 参数处理、fallback 策略）。旧函数保留为薄包装器。

## 相关文档

- [`仓库目录与产物边界.md`](./仓库目录与产物边界.md) — 仓库顶层目录职责
- [`wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/INDEX.md`](../03-功能模块/06-il2cpp核心架构/01-翻译管线/INDEX.md) — 翻译管线详细架构
