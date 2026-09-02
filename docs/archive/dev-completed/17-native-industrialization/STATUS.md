# Native C++ 工业化品质提升 — STATUS

## Phase
`roadmap` — 阶段 P1-P3 完成, P4 收敛可行性受限(GC 架构级)已记录

## task_type
`roadmap`

## roadmap_or_plan
`docs/dev/in-progress/native-industrialization/roadmap-v1-01.md`

## dispatch_model
`hybrid`（T-A/T-B 并行准备 / P1-P4 串行推进）

## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: []
completed_batches: [batch-1 (T-A 工具链 + T-B 架构设计), batch-2 (P1), batch-3 (P2), batch-4 (P3), batch-5 (P4 收敛受限)]
pending_batches: []
terminals_active: []

## 子任务执行策略
- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`

## 分类声明
- 路径：Architectural（架构级 / 全流程）
- 三优先级：P1 性能 > P2 架构 > P3 热更（保持默认，工具链/文档不引入运行时开销）
- 对标基准：production il2cpp 工程门禁体系

## 状态
`question_clearance: cleared`
`clearance_confirmed_by_user: true`
`blocking_questions: []`
`phase: roadmap`

## 边界拍板（用户已确认）
1. **范围**：全选 → 工具链全覆盖 + 模块化重构 + 编码规范收紧 + API 文档体系
2. **深度**：极致扫尾型 → 按 GC-工业化同规格做完整 roadmap（T-A/T-B + P1-P5 串行）
3. **验收口径**：对标成熟引擎 → 核心为 production il2cpp 工程质量门禁
4. **对标基线**：production il2cpp（Unity/微软）装备工程门禁
5. **三优先级**：性能优先不变（P1 性能 > P2 架构 > P3 热更，工具链/文档不引入运行时开销）
6. **落地形态**：独立 roadmap（新建 `native-industrialization/` 目录，复用 GC-工业化四件套结构）

## 架构映射

| 子任务 | 域 | 所属 Expert | 关联文件 |
|--------|----|------------|---------|
| T-A 工具链全覆盖 | 构建(7) | build-fixer / platform-expert | `.clang-tidy`、`cmake/sanitizers/`、CI workflow |
| T-B 架构设计 | 运行时(1) | runtime-expert / translation-expert | `src/native/runtime-core/`、`src/native/common/` |
| P1 编码规范审计 | 运行时(1) | runtime-expert / abi-expert | `src/native/runtime-core/core/`、`common/chaos/` |
| P2 模块化重构 | 运行时(1) | runtime-expert / translation-expert | 全 `src/native/` |
| P3 API 文档 | 构建(7) | platform-expert / build-fixer | `contracts/native/v0/`、`src/native/common/` |
| P4 单测覆盖 | 测试(5) | project-test-governance / fact-verification-expert | `tests/`、`tests/runner/baselines/` |

## 当前结论
设计文档与 roadmap 已产出。进入执行前需创建 DISPATCH.md 并推进 T-A（工具链全覆盖）——该阶段无依赖，可立即并行启动。

## 风险评估摘要
- 最高风险：T-A 阶段 clang-tidy 存量警告过多阻塞门禁启动（分类策略缓解）
- 覆盖率阈值需渐进式（50%→70%→80%），避免形同虚设或频繁误报
- 模块化重构需 T-A 护网先行，确保不引入回归（性能基准 diff 度量）

## 三优先级权衡结论
保持默认（P1 性能 > P2 架构 > P3 热更），无领域化裁决。工具链（T-A）与文档（P3）不引入运行时开销，符合性能优先；模块化重构（P2）在性能基线保护下执行，明确要求重构不降性能。

## 关键文档
- `docs/dev/in-progress/native-industrialization/design-v1-01.md`
- `docs/dev/in-progress/native-industrialization/roadmap-v1-01.md`

## 下一步入口
1. 创建 `DISPATCH.md`，规划 T-A / T-B 并行批次
2. 启动 T-A（工具链全覆盖）——clang-tidy 集成 + CI 强制
3. 启动 T-B（架构设计）——大文件拆分 + 循环依赖消除方案

## 最近摘要
### brainstorming 完成
- 用户确认全部边界：范围（全选）、深度（极致扫尾）、验收（对标 production il2cpp）、三优先级（性能优先不变）、落地形态（独立 roadmap）
- 产出 design-v1-01.md + roadmap-v1-01.md
- blocking_questions 全面清零，question_clearance=cleared

### T-A 工具链全覆盖 完成（6/6）
- ✅ T-A1 clang-tidy：`.clang-tidy` + CMakeLists option + linux-x64-clang-tidy preset + clang-tidy.yml CI workflow（+ 用户确认项目现状命名可接受，不额外强加 identifier-naming）
- ✅ T-A2 clang-format：hygiene-gate.yml 增加 diff-based clang-format check
- ✅ T-A3 覆盖率：codegen-regression.yml 加 CoverletThreshold=50 line
- ✅ T-A4 UBSan：ubsan.cmake + linux-x64-ubsan preset + ubsan-ci.yml workflow
- ✅ T-A5 模糊测试：新增 test_fuzz_vtable（3 个随机测试）+ fuzz/CMakeLists.txt 注册
- ✅ T-A6 性能回归：jit-baseline.yml + codegen-regression.yml 移除 continue-on-error，硬 red
- ✅ 提交：3224dfec6，unit 测试验证无回归（codegen 2143/2143 ✅）

### T-B 架构设计 完成（5/5）
- ✅ T-B1 大文件拆分方案 → `module-split-design.md`：22 个超大文件按 P0/P1/P2 分阶段拆
- ✅ T-B2 循环依赖消除 → `dep-cleanup-design.md`：推荐方案C（回调注册）
- ✅ T-B3 错误类型统一 → `error-model-design.md`：推荐方案A+D（HRESULT 整型 Status 码 + 渐进迁移）
- ✅ T-B4 裸 new 审计 → `alloc-audit.md`：H1 4 处确定性泄漏已修复，H3/H4 经审计后确认为非 bug
- ✅ T-B5 Doxygen → `doxygen-setup.md`：P0 合约头 → P1 基础库 → P2 运行时，CI 自动发布

### P1 编码规范审计 完成
- ✅ H1: collection_stubs.cpp 4 处确定性泄漏 → 添加 CollectionList/HashSet/Queue/StackDispose 函数
- ✅ H3: parse_convert.cpp 类型双关 → 审计确认为合法指针还原（carrier_ptr 指向真实 DecimalCarrier 对象），不修
- ✅ H4: interop_stubs 堆族混用 → 代码本身配对正确，风险在 API 使用者，不修（文档化）
