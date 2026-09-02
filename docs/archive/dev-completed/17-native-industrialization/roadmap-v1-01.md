# Native C++ 工业化品质提升 — Roadmap

## 1. 目标

使 native C++ 基础达到 production il2cpp 级工业化品质：**具备可背书生产改造的质量门禁体系（clang-tidy / 格式化 / 覆盖率 / UBSan / 模糊测试 / 性能回归），模块化架构清晰无循环依赖，编码规范一致，API 文档自动生成，核心路径单测覆盖充足。** 仿照 GC-工业化「验证先行 + 阶段推进」方法论，以「生产引擎工程门禁」为对标基准。

## 2. 范围边界

- **包含**：工具链全覆盖（clang-tidy / clang-format / 覆盖率 / UBSan / 模糊测试 / 性能回归硬门禁）、模块化重构（大文件拆分 / runtime-core 反向依赖消除）、编码规范审计（裸 new 宏体系 / reinterpret_cast / 错误类型）、API 文档体系、单测覆盖补齐
- **对标**：production il2cpp（Unity/微软）工程质量门禁
- **全局优先级**：P1 性能 > P2 架构 > P3 热更（保持默认，工具链/文档不引入运行时开销；性能优先级要求重构不降性能并有基准护网）

### 对标深度档

| 档 | 说明 |
|----|------|
| **D1 门禁级** | clang-tidy 0 P0 error + clang-format CI 强制 + 覆盖率阈值 + UBSan CI。核心质量门禁可用，能 red 住缺陷 |
| **D2 审计级** | 全量分类审查（reinterpret_cast/裸 new/错误类型）+ 命中 production 命名与 RAII 规范。存量代码规范收敛 |
| **D3 架构级** | 模块化重构完成（无循环依赖 / 无 >1000 行文件 / runtime-core 拆 3-5 子系统）+ API 文档自动发布 |
| **D4 完备级** | 核心路径覆盖率 ≥80% + known-fail ≤30 + 性能回归 Δ%<5%。可与 production 引擎对标完备度 |

## 3. 非目标

- 不重写 GC 核心（GC-工业化已覆盖）
- 不做全量代码重写（仅聚焦品质缺口）
- 不改 codegen 输出格式（codegen-expert 域单独处理）
- 不引入新第三方依赖（工具链必需除外）
- 不改 C++ 标准（保留 C++17/20）

## 4. 阶段列表

| Phase | 名称 | 类型 | 依赖 | 预估 |
|:------|:-----|:----:|:-----|:----:|
| T-A | 工具链全覆盖（质量门禁） | 并行准备 | 无 | 2-4周 |
| T-B | 架构设计与编码规范基线 | 并行准备 | 无 | 1-2周 |
| P1 | 编码规范审计（分配/转型/错误） | 串行 | T-A 绿 + T-B 设计完成 | 1-2周 |
| P2 | 模块化重构（大文件/依赖/拆分） | 串行 | P1 | 2-4周 |
| P3 | API 文档体系 | 串行 | P2（用重构后结构） | 1-2周 |
| P4 | 单测覆盖补齐 + known-fail 收敛 | 串行 | P3 | 2-3周 |

**全局优先级约束**：P1 性能 > P2 架构 > P3 热更。裁决方式：工具链（T-A）与文档（P3）不引入运行时开销，符合「性能优先不变」；模块化重构（P2）必须在性能基线与 T-A 护网保护下执行，明确要求**重构不降性能**；已知变更的性能影响通过基准 diff 度量。

## 5. 每阶段完成定义

### T-A 工具链全覆盖（质量门禁）

- `goal`: 建立 production 级质量门禁体系，使 CI 能 red 住缺陷
- `exit_criteria`:
  - clang-tidy 集成 CMake（`CMAKE_CXX_CLANG_TIDY`）+ CI 强制，0 P0 error
  - clang-format CI 强制（`clang-format --dry-run --Werror`）
  - 覆盖率硬门禁（先摸底基线 → 设渐进阈值 50%→70%）
  - UBSan CMakePreset + CI 集成（Linux Ubuntu）
  - 模糊测试扩展至 ≥5 个 + CI（短时运行）
  - 性能回归硬门禁（去掉 continue-on-error，设 Δ% 阈值）
- `deliverables`: `.clang-tidy` 配置、CMake 集成、CI workflow 更新、覆盖率报告、UBSan preset、模糊测试文件、性能门禁脚本
- `dependencies`: 无
- `resolved_decisions`: clang-tidy 分类策略（P0 错误必须修 / P1 警告 stepwise / 合理项 suppress）；覆盖率阈值渐进式（先摸底 50%，稳定后 70%，最终 80%）
- `watch_items`: clang-tidy 存量警告数量（可能超出预期，需分类处理）；覆盖率摸底值（决定阈值起点）

### T-B 架构设计与编码规范基线

- `goal`: 为 P1-P4 提供架构依据与编码规范基线
- `exit_criteria`:
  - 大文件拆分方案（优先 gc_old_gen 3918L / ir_reg_alloc 2969L 等）
  - runtime-core 反向依赖消除方案（jit/interpreter → 接口注入）
  - 统一错误类型方案（Result<T,Error> 可行性评估）
  - 裸 new / reinterpret_cast 审计清单
  - Doxygen 配置方案 + 合约头注释规范
  - known-fail 收敛策略
- `deliverables`: `module-split-design.md`、`dep-cleanup-design.md`、`error-model-design.md`、`alloc-audit.md`、`doxygen-setup.md`
- `dependencies`: 无（可离线设计）
- `resolved_decisions`: 拆分为「只拆不改逻辑」的 git 友好策略；错误类型先评估不强制引入

### P1 编码规范审计

- `goal`: 存量代码规范收敛到 production 水平
- `exit_criteria`:
  - 176 处裸 new 改走 `CHAOS_IL2CPP_NEW` 宏体系（或明确豁免）
  - reinterpret_cast 全量分类审查（确认每处必要，标注理由，高风险项修复）
  - 命名规范一致（宏/命名空间/成员）
  - 错误返回风格统一（当前 Result 类型评估结论落地）
- `deliverables`: 审计报告、修复 commit

### P2 模块化重构

- `goal`: 消除单体风险，达到 D3 架构级（在 T-A 护网下安全进行）
- `exit_criteria`:
  - 22 个 >1000 行文件拆至 ≤5 个（单文件 ≤1000 行为主）
  - runtime-core → jit/interpreter PUBLIC 反向依赖消除（接口注入）
  - runtime-core 81K 行拆分为 3-5 个独立可测试子系统
  - 无循环依赖（构建期验证）
  - 性能基准无回归（Δ% < 5%）
- `deliverables`: 拆分 commit（每单次只做拆分不改逻辑）、性能基准对比

### P3 API 文档体系

- `goal`: 核心 API 文档自动生成并发布，提升可维护性
- `exit_criteria`:
  - Doxyfile 配置部署
  - `contracts/native/v0/` + `src/native/common/chaos/` 核心头文件 API 注释覆盖
  - CI 自动生成 Doxygen + GitHub Pages 发布
- `deliverables`: Doxyfile、头文件注释、CI 发布 workflow

### P4 单测覆盖补齐 + known-fail 收敛

- `goal`: 核心路径覆盖率达标，known-fail 收敛
- `exit_criteria`:
  - runtime-core 核心路径（GC/VTable/线程/COM）覆盖率 ≥70%
  - known-fail 从 82 收敛至 ≤30
  - 每个 known-fail 带 issue 跟踪 + expiry
- `deliverables`: 测试文件、覆盖率报告、known-fail 清理

## 6. 验收标准汇总

| 阶段 | D 档 | 硬指标 |
|------|------|--------|
| T-A | D1 | clang-tidy 0 P0 error；clang-format CI 强制；覆盖率阈值；UBSan CI；性能门禁 |
| T-B | — | 架构设计文档完成 + 用户审批 |
| P1 | D2 | 176 裸 new 清零；reinterpret_cast 审查完成；错误类型落地 |
| P2 | D3 | 超大文件 ≤5；无循环依赖；runtime-core 拆 3-5 子系统；性能无回归 |
| P3 | D3 | Doxygen 自动生成 + GitHub Pages 发布 |
| P4 | D4 | 核心覆盖率 ≥70%；known-fail ≤30 |

## 7. 风险与缓解

| 风险 | 缓解 |
|------|------|
| T-A clang-tidy 存量警告过多阻塞 | 分类策略（P0 修/P1 渐进/suppress 合理项），先建立 baseline 不阻塞门禁启动 |
| 覆盖率低估导致门禁形同虚设 | 先摸底 baseline，渐进 50%→70%→80% |
| 模块化重构引入回归 | T-A 护网先行 + 小步拆分 + 性能基准 diff |
| 大文件拆分破坏 git blame | 只拆不改逻辑，单次提交纯拆分 |
| 工具链平台差异（MSVC vs GCC） | 配置同时兼容，T-A 用 Linux 覆盖 clang 系，Windows 单独验证 |

## 8. 完成判定

当 T-A 至 P4 全部通过（各阶段 exit_criteria 满足），且最终指标达成：
- clang-tidy 全量通过（suppress 仅含合理项）
- 覆盖率（核心）≥70%，净代码覆盖 ≥50%
- 性能回归 Δ% < 5%
- known-fail ≤30
- API 文档自动发布生效
- 无循环依赖、无 >1000 行文件、runtime-core 拆 3-5 子系统

则 native C++ 工业化品质达成 production il2cpp 级（达到 D3-D4 完备度）。
