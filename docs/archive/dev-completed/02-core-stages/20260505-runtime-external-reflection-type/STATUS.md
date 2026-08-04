---
task_id: runtime-external-reflection-type
title: Runtime External 依赖实现 — Reflection/Type + Exception
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-03 20:00:00 +08:00
updated_at: 2026-05-03 18:30:00 +08:00

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: brainstorming-approval

## Scope
实现 reflection-type 和 exception 两个域的外部运行时依赖函数（~35 个 chaos_reflection_* 原生函数），使 reflection-type family 能从 stub（返回 0）变为真实 il2cpp 翻译路径。

## 边界拍板
- Q1 = 方案 A：inline header（native-aot.metadata.h 与 generated.cpp 同目录）
- Q2 = 方案 A：全局静态 constexpr 元数据表
- Q3 = 方案 A：Type 全套 + Exception 全套 ~35 个函数

## 架构映射
- il_feature: reflection/type + exception
- translation_path: external_runtime_helper (SimpleForward)
- native_side: runtime-core/ 中实现 extern "C" 函数
- metadata: reflection_query_model.h 的 ReflectionQueryTypeDescriptor + constexpr 数据表
- constraints_followed: 不修改 bridge vtable，不修改生成的 cpp 结构，通过 TLS RuntimeState 访问运行时上下文
- wiki_entry: needs-update

## 完成状态
- [x] Step 1a: MetadataHeader artifact name constant (ManagedClosureArtifactModels.cs)
- [x] Step 1b: Function declarations in runtime_core.h + TLS (runtime_core.h, runtime_core.cpp)
- [x] Step 2: Emitter — NativeAotLoweringPlanner.BuildAotMetadataHeader() + NativeAotEmitter wiring
- [x] Step 3: reflection_api.cpp (22 functions)
- [x] Step 4: exception_api.cpp (4 functions)
- [x] Step 5: CMakeLists.txt updated, C# build 0 errors, native build 0 errors
- [x] Step 6: Codegen review PASS, native-aot pipeline PASS, semantic patch PASS

## 完成文件清单
### Modified
- src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs — 添加 MetadataHeader = "generated/native-aot.metadata.h"
- src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs — 添加 BuildAotMetadataHeader()、MetadataHeaderContent 属性、#include "native-aot.metadata.h"
- src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs — 添加 MetadataHeader 作为第三 generated source
- src/native/runtime-core/runtime_core.h — 添加 SetCurrentAotImage/GetCurrentAotImage 声明 + 28 个 extern "C" 声明
- src/native/runtime-core/runtime_core.cpp — 添加 g_tls_aot_image + setter/getter
- src/native/runtime-core/CMakeLists.txt — 添加 reflection_api.cpp, reflection_metadata_impl.h, exception_api.cpp
- **src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs** — 新增 `chaos_managed_pointer_local_slot_tag` constexpr 定义（fix pre-existing 编译错误）
- **src/native/common/chaos/native_types.h** — 新增 `chaos_resolve_native_int_slot` 内联函数（fix pre-existing 编译错误）
- **build/toolchains/run/testing/foundation_dll/native_compile_runner.py** — 修复 MSVC 路径 Professional、vcvarsall env 捕获、缺失 include dirs、/utf-8 标志、missing argparse import（5 项 pre-existing 修复）

### Created
- src/native/runtime-core/reflection_metadata_impl.h — constexpr 元数据表（7 个核心类型）
- src/native/runtime-core/reflection_api.cpp — 22 个 chaos_reflection_* 函数
- src/native/runtime-core/exception_api.cpp — 4 个异常元数据函数

## Verification
- C# build: 0 errors, warnings pre-existing
- Native build (chaos_runtime_core): 0 errors
- batch_native_aot_runner (reflection-type): PASS
- batch_codegen_review (reflection-type): PASS
- batch_semantic_patch_runner (reflection-type): PASS
- Generated native-aot.metadata.h present in output directory
