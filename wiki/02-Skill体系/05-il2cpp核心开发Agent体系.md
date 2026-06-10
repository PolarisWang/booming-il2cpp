# il2cpp 核心开发 Agent 体系

> 本文档记录 il2cpp 核心开发 Agent 体系的架构设计、路由规则和使用方式。
>
> 源文件位置：`skills/library/skills/dev-il2cpp-*-*/SKILL.md`

## 架构概览

il2cpp 核心开发 Agent 体系采用 **Master Controller + Specialized Agents** 混合架构：

```
dev-using-booming → 检测到 il2cpp 域
  ↓
dev-il2cpp-core-agent (Master Controller)
  ├── 阶段1: 语境加载（优先级约束 + 00-快速导航）
  ├── 阶段2: 8 路分类路由 + 跨域处理
  └── 阶段3: 自动质量门（trace → verification → STATUS.md → wiki）
       │
       ├── 单域 → Expert Agent
       ├── 双域 → Controller 串行调度
       └── 3+ 域 → Workflow Orchestrator 或 roadmap
```

### 设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| Controller 不写代码 | 纯路由 + 质量门 | 避免职责混淆，Controller 只"知道用什么工具" |
| Expert Agent 独立 SKILL.md | 独立可注册 | 可单独调用、可独立演化、不依赖 Controller |
| 路由替换 dev-using-booming | il2cpp 域自动跳转 | 用户无感，所有 il2cpp 任务自动走统一入口 |
| 三层复杂度 | 直路由 → 串行 → Orchestrator | 小任务不浪费 token，大任务有协调方案 |

## Expert Agent 职责矩阵

| Agent | 域 | 知识域 | 下游集成 | 阶段 |
|-------|-----|---------|---------|------|
| `dev-il2cpp-core-agent` | 总入口 | 00-快速导航.md + 全局优先级 | 所有 Expert Agent | P1 |
| `dev-il2cpp-translation-expert` | 翻译 | 31 管线文档 + 12 路径参考 + 5 类约束 | arch-first-dev, wiki-maintenance | P1 |
| `dev-il2cpp-runtime-expert` | 运行时 | runtime-core/interpreter/bootstrap/support | codegen_bridge, runtime_abi | P1 |
| `dev-il2cpp-debug-expert` | 调试 | 16 种故障模式 + trace 系统 | systematic-debugging, TDD | P1 |
| `dev-il2cpp-gc-expert` | GC | CRAG GC + 分代 + 写屏障 + stress test | pressure-test-management | P2 |
| `dev-il2cpp-codegen-expert` | CodeGen | 25 Planner 文件 + 40+ Scriban 模板 + 88 snapshot 基线 | test-governance | P2 |
| `dev-il2cpp-hotupdate-expert` | 热更新 | PatchLoader + .patchdata + InterpreterEntryDirect | runtime-expert, codegen-expert | P2 |

## 路由矩阵

| 任务信号 | 路由目标 |
|---------|---------|
| 新 IL 指令、Emission、Planner、Lowering | `dev-il2cpp-translation-expert` |
| runtime-core、interpreter、VTable、bootstrap | `dev-il2cpp-runtime-expert` |
| crash、segfault、test failure、编译错 | `dev-il2cpp-debug-expert` |
| GC、分配、写屏障、stress test | `dev-il2cpp-gc-expert` |
| C# codegen、T4 模板、snapshot | `dev-il2cpp-codegen-expert` |
| foundation-dll、subject、测试管线 | `dev-project-test-governance` |
| 性能优化、profile、benchmark | `dev-optimization-campaign` |
| 热更新、PatchLoader、patchdata | `dev-il2cpp-hotupdate-expert` |
| **跨域（2+ 域）** | Controller 串行调度 |
| **跨域（3+ 域）** | roadmap / Workflow Orchestrator |

## 演化路径

| 阶段 | 交付物 | 状态 |
|------|--------|------|
| Phase 1 | 知识基座 + Controller + 3 Expert（翻译/运行时/调试） | ✅ |
| Phase 2 | 3 Expert（GC/CodeGen/热更新） | ✅ |
| Phase 3 | Workflow Orchestrator + 6 冲突检测规则 | ✅ |

### 未来的可能演化

- **GC Expert 深度化**：增加自动 stress test 场景调度
- **HotUpdate Expert 深度化**：增加 7 步验证脚本化
- **Orchestrator 深度化**：从模板升级为可运行的 Workflow 脚本
- **跨 Expert 知识共享**：建立公共的"合约变更检测"机制

## 使用示例

### 示例 1：翻译新 IL 指令（自动路由）

```
用户输入: "添加 newobj 指令的翻译路径"
→ dev-using-booming 检测到 il2cpp 域
→ 自动跳转 dev-il2cpp-core-agent
→ Controller 分类为"翻译"
→ 调用 dev-il2cpp-translation-expert
  → 加载 arch-first-development 三阶段
  → 查翻译路径表 → 实现 → snapshot 验证
→ 质量门
```

### 示例 2：GC crash 修复（双域串行）

```
用户输入: "stress test 中 Page Decommission 导致访问冲突"
→ Controller 分类为"调试 + GC"双域
→ 先调 dev-il2cpp-debug-expert 定位（检查故障模式表 → 匹配已知模式）
→ 再调 dev-il2cpp-gc-expert 修复（选择 stress 场景 E）
→ 质量门
```

### 示例 3：复杂新功能（跨域，走 roadmap）

```
用户输入: "翻译 calli + 改 runtime helper + 加 foundation-dll 测试"
→ Controller 分类为"翻译 + 运行时 + 测试"三域
→ 提示用户走 dev-roadmap 生成多步计划
→ 每步依次调用对应 Expert Agent
→ 质量门
```

## 与现有 skill 体系的关系

- `dev-using-booming`：上游入口，检测到 il2cpp 域时自动路由到 Core Agent
- `dev-architecture-first-development`：被 Translation Expert 封装为内置流程
- `dev-systematic-debugging`：被 Debug Expert 引用为底层四阶段流程
- `dev-project-test-governance`：被 Controller 直接路由（复用，不新建 il2cpp 版本）
- `dev-optimization-campaign`：被 Controller 直接路由（复用）
