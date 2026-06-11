# il2cpp registry

> 本页由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要开发 il2cpp 翻译逻辑或查询架构路径时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-architecture-first-development`](../../library/skills/dev-architecture-first-development/SKILL.md) | 开发新的 IL 特性翻译路径时；修改现有翻译行为时；新增 Emission 或 Planning 策略时；修改 C++ 运行时中与代码生成对应的行为时 | 开发 il2cpp 翻译逻辑前必须执行的架构查询与路径匹配。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-cpp-conventions`](../../library/skills/dev-cpp-conventions/SKILL.md) | 编写新的 native C++ 文件时；需要确定命名空间、include 风格时；需要添加 extern C 声明时；运行 clang-format 时；添加新的头文件时 | C++ 编码规范：命名空间、Include、extern C、头文件原则、clang-format。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-build-fixer`](../../library/skills/dev-il2cpp-build-fixer/SKILL.md) | 链接错误 LNK / C++ 编译错 Cxxxx / CMake 错误；CHAOS_IL2CPP_FAIL / ChaosExternalRuntimeFallback stub；dotnet build 失败（ATG/CodeGen/CombinedSubjects）；Python 管线编排错误 | 编译失败与 codegen stub 修复专家。严格分层诊断（ATG/TPG/CodeGen/Python），在哪层出问题就在哪层修 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-codegen-expert`](../../library/skills/dev-il2cpp-codegen-expert/SKILL.md) | C# codegen 管线修改（NativeAotEmitter / LoweringPlanner）；Scriban 模板修改或新增；Snapshot 测试基线变更；PatchDataExtractor/Dumper 修改；AOT/JIT 模式切换相关 | il2cpp CodeGen 专家 — C# codegen 管线、NativeAotLoweringPlanner、Scriban 模板、snapshot 测试 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-core-agent`](../../library/skills/dev-il2cpp-core-agent/SKILL.md) | il2cpp/AOT/GC/Runtime/翻译/CodeGen 相关工作；用户从 dev-using-booming 路由到 il2cpp 域；用户直接输入 /dev-il2cpp-core-agent | il2cpp 核心开发统一入口 — 分类任务并路由到对应的 Expert Agent，完成后自动执行质量门 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-debug-expert`](../../library/skills/dev-il2cpp-debug-expert/SKILL.md) | il2cpp 运行时 crash / segfault；多层管线（Python→C#→C++）问题追踪；GC/interpreter/runtime-core 异常行为 | il2cpp 调试专家 — il2cpp 领域专用的系统性调试，集成 trace 系统和 il2cpp 故障模式索引 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-fact-verification-expert`](../../library/skills/dev-il2cpp-fact-verification-expert/SKILL.md) | fact 流程执行/结果判定；fact 失败诊断与路由到 codegen/build-fixer/debug；skip-list 维护（_KNOWN_FACT_FAILURES / fact_skip_indices）；ATG/TPG 质量审查与回归验证；codegen 输出审查（native-aot.generated.cpp / aot-core-ir.json）；value_suspicious / shutdown-AV 分析 | foundation-dll fact 验证专家 — fact 流程执行、错误诊断路由、ATG/TPG 质量门、codegen 输出审查、skip-list 维护 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-foundation-dll-optimizer`](../../library/skills/dev-il2cpp-foundation-dll-optimizer/SKILL.md) | benchmark 运行/分析/回归检测；profile 数据分析/GC 健康检查；pipeline 故障排查（build/fact/benchmark/profile）；性能优化（AOT vs JIT 对比、代码段大小优化）；snapshot 基线管理 / chunk 配置调优 | foundation-dll 验证 DLL 的 benchmark 运行、性能分析和优化专家 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-gc-expert`](../../library/skills/dev-il2cpp-gc-expert/SKILL.md) | GC crash / 分配异常 / OOM / stress test 回归；CRAG GC 分代策略 / 写屏障 / Safepoint 修改；Page Decommission / Free List 问题；NurseryAllocateSlow / GcAllocate 修改 | il2cpp GC 专家 — CRAG GC 架构、分配策略、分代分析、写屏障、stress test、内存域 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-hotupdate-expert`](../../library/skills/dev-il2cpp-hotupdate-expert/SKILL.md) | 热更新架构修改（Hotpatch / PatchLoader）；.patchdata 格式或提取逻辑变更；InterpreterEntryDirect / HotpatchDispatch 修改；emit-patch-data CLI 相关；旧版 method_replacement 兼容 | il2cpp 热更新专家 — Hotpatch 双层分派、PatchLoader、.patchdata、PatchMetadataCache、Interpreter 解释器路由 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-runtime-expert`](../../library/skills/dev-il2cpp-runtime-expert/SKILL.md) | 修改 runtime-core / interpreter / bootstrap / support 模块；修改 VTable/method_table/线程状态/GC Safepoint；运行时合约（codegen_bridge.h / runtime_abi.h）变更 | il2cpp 运行时专家 — runtime-core/interpreter/bootstrap/support 领域的修改与调试 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-translation-expert`](../../library/skills/dev-il2cpp-translation-expert/SKILL.md) | 新 IL 指令翻译路径设计；修改 Planning / Emission / RuntimeSupport 文件；新增 Emission 或 Planning 策略 | il2cpp 翻译路径专家 — 处理 IL→C++ 翻译路径选择、Planner/Emission 修改、新增 IL 指令翻译 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-native-debug-profiling`](../../library/skills/dev-native-debug-profiling/SKILL.md) | 需要调试 native 崩溃或逻辑错误时；需要性能分析或热点统计时；需要插入 LOG_DEBUG 临时调试时；对 fast_dispatch.cpp 或 interpreter_entry.cpp 等热点路径做修改时；cmake 构建需要切换 CHECK/PROFILE/SHIP 配置时 | Native 调试与性能分析规范：构建配置、LOG 级别、PROFILE_SCOPE、profile.h、热点路径约束。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-unordered-map-selection`](../../library/skills/dev-unordered-map-selection/SKILL.md) | 编写 native C++ 代码时需要选择 unordered_map 实现时；需要在热点路径中使用哈希表时；需要确定使用 CHAOS_IL2CPP_UNORDERED_DENSE_MAP 还是 std::unordered_map 时 | unordered_map 选型规则：何时用 CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY / MAP / std 退回避让。 | 1.0.0 | `00-skills-maintainers` | small | active |
