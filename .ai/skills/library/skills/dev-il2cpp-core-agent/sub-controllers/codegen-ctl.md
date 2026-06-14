# codegen-ctl — CodeGen 子 Controller

## 职责
管理 CodeGen 域（4）+ 翻译域（6）+ 构建域（7）+ 测试域（5）+ 热更新域（8）+ 平台域（9）的任务分发。

## 管理的 Expert

- `dev-il2cpp-codegen-expert` — C# codegen 管线/T4 模板/snapshot
- `dev-il2cpp-translation-expert` — 新 IL 指令翻译路径/Planner/Emission
- `dev-il2cpp-build-fixer` — 编译错误/LNK/codegen stub/CMake
- `dev-il2cpp-fact-verification-expert` — foundation-dll fact/subject/manifest
- `dev-il2cpp-hotupdate-expert` — PatchLoader/HotpatchDispatch/.patchdata
- `dev-il2cpp-platform-expert` — 跨平台 PAL/CMakePresets/CI-CD
- `dev-il2cpp-abi-expert` — ABI 类型映射/调用约定（跨域）
- `dev-il2cpp-external-runtime-expert` — extern stub/ShapeRegistry（跨域）
- `dev-il2cpp-pipeline-expert` — 缓存策略/SDK 路径（跨域）
- `dev-il2cpp-simd-expert` — SIMD 翻译路径（跨域）
- `dev-il2cpp-codegen-capabilities` — codegen 能力矩阵
- `dev-il2cpp-verification-pipeline` — 测试管线治理/gate

## 分组路由策略

路由策略（任务类型→Expert）、执行顺序约束、混合域处理统一在 `expert-registry.json` 的 `expert_sub_controller.codegen-ctl.routing_strategy` 中定义。
本文不重复维护。

## 输出约束
- 处理不完 → 标记 `⏳ remaining` 回顶层 core-agent
- 转交时必须附诊断上下文（见 build-fixer SKILL.md 的"诊断交接模板"）
