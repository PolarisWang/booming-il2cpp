# testing registry

> 本页由 `.ai/skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

任务涉及测试框架、subject 定义、测试生成或测试治理时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-il2cpp-atg-expert`](../../library/skills/dev-il2cpp-atg-expert/SKILL.md) | chunk 的 fact subjects 为 0（enum-parsing, global-ns）；Probe 判定 IsDeterministic 逻辑变更；SubjectId 在 ATG/CodeGen/TPG 间不匹配；generatedMethodId 命名规范修改；subjects.metadata.json schema 变更 | AutoTestGenerator 测试合约专家 — Probe 机制、SubjectId 规范、Metadata 管线完整性、测试代码质量门 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-pipeline-expert`](../../library/skills/dev-il2cpp-pipeline-expert/SKILL.md) | hephaestus 缓存命中/未命中策略；cmake 增量编译不重新编译运行时 stub；SDK lib 损坏 (LNK1107)；artifacts/presets/ 目录缺失；build.py post-TPG 步骤错误 | 构建管线专家 — Hephaestus 缓存策略、cmake 增量编译、SDK 路径解析、Python 编排净化 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-simd-expert`](../../library/skills/dev-il2cpp-simd-expert/SKILL.md) | Vector2/3/4/Matrix/Plane/Quaternion 方法实现；Vector<T> lane-by-lane 软件模拟；x86 SSE/AVX intrinsics 条件编译；ARM64 NEON intrinsics 条件编译；NaN/Inf/denormal 跨平台一致性 | SIMD 实现专家 — System.Numerics 软件回退、跨平台 SSE/NEON/Generic 实现、IEEE 754 精度、边缘情况处理 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-il2cpp-verification-pipeline`](../../library/skills/dev-il2cpp-verification-pipeline/SKILL.md) | foundation-dll pipeline stage 执行失败；stage 执行顺序错误或超时；结果收集/聚合失败（JSON 解析、JSONL 写入）；Hephaestus 缓存 miss/过期判定异常；cross-stage 数据传递断裂（build→fact→benchmark 链） | 验证管线总编排专家 — 覆盖 foundation-dll 全 13 stage，只做编排和结果收集，零后处理 | 1.0.0 | `00-skills-maintainers` | medium | active |
| [`dev-pressure-test-management`](../../library/skills/dev-pressure-test-management/SKILL.md) | 修改了 GC、内存分配、线程调度相关代码后需要验证稳定性时；需要查看压力测试历史记录或回归检测结果时；AI 开发过程中需要快速验证改动没有引入性能退化时 | 压力测试管理：通过 run stress CLI 运行 GC/容量/线程压力测试，记录结果，检测回归。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-project-test-governance`](../../library/skills/dev-project-test-governance/SKILL.md) | 修改 subject/test 工作流时；修改 Chaos.TestFramework 时；修改 managed/native/hotupdate runners 时；修改 benchmark 或 unit verification 流程时；修改项目 codegen 结构时 | 测试治理：subject/test 工作流、Chaos.TestFramework、runner、benchmark、codegen 结构变更。 | 1.0.0 | `00-skills-maintainers` | large | active |
