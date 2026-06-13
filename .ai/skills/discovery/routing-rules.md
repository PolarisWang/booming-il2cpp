# 路由规则（Routing Rules）— 单一权威源

> 本文是项目路由规则**唯一权威来源**。所有引用路由规则的文件必须引用本文，不得重复定义。

---

## 1. 域编号表

| 编号 | 域 | 说明 | 对应 Expert | 受保护的文件路径 |
|------|----|------|------------|-----------------|
| 1 | 运行时 | runtime-core/interpreter/VTable/bootstrap、JIT、SEH、tier promotion | `dev-il2cpp-runtime-expert`、`dev-il2cpp-jit-expert` | `src/native/runtime-core/`, `src/native/interpreter/`, `src/native/bootstrap/`, `src/native/support/`, `src/native/jit/` |
| 2 | GC | 内存分配、分代、写屏障、stress test | `dev-il2cpp-gc-expert` | `src/native/runtime-core/gc/` |
| 3 | 调试 | crash、segfault、测试失败 | `dev-il2cpp-debug-expert` | 所有 native 源 |
| 4 | CodeGen | C# codegen、T4 模板、snapshot | `dev-il2cpp-codegen-expert` | `src/managed/Chaos.IL2CPP.Generator/` |
| 5 | 测试 | foundation-dll、subject、manifest | `dev-il2cpp-fact-verification-expert` | `testing/foundation-dll/` |
| 6 | 翻译 | 新 IL 指令、Planner、Emission | `dev-il2cpp-codegen-expert` | `src/managed/Chaos.IL2CPP.Generator/Planning/`, `src/managed/Chaos.IL2CPP.Generator/Emission/` |
| 7 | 构建 | 编译、链接、SDK、cmake | `dev-il2cpp-build-fixer` | `src/tools/`, `testing/foundation-dll/verification/stages/build.py` |
| 8 | 热更新 | PatchLoader、patchdata | `dev-il2cpp-hotupdate-expert` | `src/native/hot-update/` |

## 2. Expert 加载强制规则

**分类声明必须包含 Expert 加载声明**：

```
格式: 本轮任务涉及 CodeGen(4) ，fix 操作，第 1 轮 → 加载 dev-il2cpp-codegen-expert
```

**执行顺序**:
1. 输出分类声明（含 Expert 声明）→ `echo "..." > .claude/.classified`
2. 通过 registry 发现流程读取 Expert 的 SKILL.md 加载知识（`Skill` 工具不支持子 Expert 加载）
3. 编辑域文件 → hook 验证分类声明格式，**不强制 loaded_expert**

**建议**：在编辑受保护域文件前，先通过 discovery 流程阅读对应 Expert 的 SKILL.md。
| 6 | 翻译 | 新 IL 指令、Planner、Emission |
| 7 | 构建 | 编译、链接、SDK、cmake |
| 8 | 热更新 | PatchLoader、patchdata |

action: `read` / `fix` / `build` / `verify` / `plan`

---

## 2. 分类声明格式

```
本轮任务涉及 {域1(编号)} + {域2(编号)} ... ，{action} 操作，第 N 轮
```

示例: `本轮任务涉及 运行时(1) + 构建(7) ，fix 操作，第 1 轮`

---

## 3. 分层路由架构

顶层 core-agent 按域分组路由到子 Controller，子 Controller 再分派到具体 Expert。

```
顶层 core-agent                          子 Controller              Expert
├── 运行时(1) / 调试(3)  → runtime-ctl → ├── dev-il2cpp-runtime-expert
│                                          └── dev-il2cpp-debug-expert
├── GC(2) / 优化(5)      → gc-ctl      → ├── dev-il2cpp-gc-expert
│                                          └── dev-il2cpp-foundation-dll-optimizer
└── CodeGen(4) / 翻译(6) / 构建(7)        → codegen-ctl → 6 个 Expert
    测试(8) / 热更新(9)
```

### Expert 分类矩阵（完整清单）

| 子任务信号 | 目标 Expert |
|-----------|-----------|
| 新 IL 指令、翻译路径、Emission、Planner、Lowering | `dev-il2cpp-translation-expert` |
| runtime-core、interpreter、VTable、bootstrap、method_table、线程状态 | `dev-il2cpp-runtime-expert` |
| crash、segfault、test failure、异常行为 | `dev-il2cpp-debug-expert` |
| GC 相关、分配模式、内存回收、写屏障、stress test | `dev-il2cpp-gc-expert` |
| C# codegen、T4 模板、NativeAot lowering、snapshot | `dev-il2cpp-codegen-expert` |
| foundation-dll、subject、测试管线、manifest | `dev-project-test-governance` |
| 性能优化、profile 分析、benchmark | `dev-il2cpp-foundation-dll-optimizer` |
| 热更新、PatchLoader、patchdata、HotpatchDispatch | `dev-il2cpp-hotupdate-expert` |
| 编译失败、链接错误、codegen stub、dotnet build 失败、CMake 错误 | `dev-il2cpp-build-fixer` |
| fact 验证、fact_chunk、skip-list 维护、value_suspicious、dll 验证结果审计 | `dev-il2cpp-fact-verification-expert` |

### 执行顺序约束
Translation Expert 和 CodeGen Expert 都涉及 Planner/Emission 文件：
- **必须先派发 Translation Expert，再派发 CodeGen Expert**

| 子任务信号 | 目标 Expert |
|-----------|-----------|
| 新 IL 指令、翻译路径、Emission、Planner、Lowering | `dev-il2cpp-translation-expert` |
| runtime-core、interpreter、VTable、bootstrap、method_table、线程状态 | `dev-il2cpp-runtime-expert` |
| crash、segfault、test failure、异常行为 | `dev-il2cpp-debug-expert` |
| GC 相关、分配模式、内存回收、写屏障、stress test | `dev-il2cpp-gc-expert` |
| C# codegen、T4 模板、NativeAot lowering、snapshot | `dev-il2cpp-codegen-expert` |
| foundation-dll、subject、测试管线、manifest | `dev-project-test-governance` |
| 性能优化、profile 分析、benchmark | `dev-il2cpp-foundation-dll-optimizer` |
| 热更新、PatchLoader、patchdata、HotpatchDispatch | `dev-il2cpp-hotupdate-expert` |
| 编译失败、链接错误、codegen stub、dotnet build 失败、CMake 错误 | `dev-il2cpp-build-fixer` |
| fact 验证、fact_chunk、skip-list 维护、value_suspicious、dll 验证结果审计 | `dev-il2cpp-fact-verification-expert` |

---

## 4. 多域处理规则

| 域数 | 策略 |
|------|------|
| 1 | Skill 加载 Expert 知识后当前 Agent 自行实现 |
| 2 | Controller 串行调度 [主域 → 从域] |
| ≥3 | 默认走 Workflow 委托 |
| 不明确的输入 | 反问用户澄清后重路由 |

### 多域执行顺序约束

Translation Expert 和 CodeGen Expert 都涉及 Planner/Emission 文件：
- **必须先派发 Translation Expert，再派发 CodeGen Expert**
- Translation Expert 完成翻译路径设计后，CodeGen Expert 在此基础上做代码生成修改

---

## 5. 终止守卫

| 条件 | 行为 |
|------|------|
| 超过 5 轮 | 终止，报残留任务 |
| 连续 2 轮无进展 | 终止，报"无进展" |
| 所有 Expert 已尝试但仍有残留 | 终止，报"无法处理的子任务" |

## 5b. 深度调试 Workflow 触发条件

当以下任一条件满足时，**必须切换为 Workflow 深度调试模式**，不得继续手动单域迭代：

| 触发条件 | 说明 |
|---------|------|
| 同一 chunk 的 fact 失败超过 5 轮未解决 | 当前 numerics chunk 已达 9 轮 |
| 失败根因横跨 2 个以上域 | CodeGen + 运行时 + 构建同时涉及 |
| 需要 trace C++ 编译/链接/运行全链路 | extern 声明 → 编译 → dispatch → SEH |
| 当前 Agent 连续 3 次尝试未取得进展 | 109/121 已停滞多轮 |

深度调试 Workflow 执行模式：

```
Step 1 — 并行域内调查（3 个独立 agent）
├── Expert A (CodeGen):   分析 extern 声明 emit 逻辑、slot map 生成、DirectNativeSymbol 路由
├── Expert B (运行时):    编译 entry.exe → trace dispatch 链 → 确认 12 个方法卡在哪一步
└── Expert C (构建):     分析 stub 插入点、cmake 增量编译导致 interop_stubs.cpp 不重新编译的问题

Step 2 — 汇聚根因
└── synthesize agent: 合并三域调查结果，输出 root_cause + fix_strategy + regression_check

Step 3 — 分域修复
├── CodeGen fix: 修改 extern 声明 emit
├── 运行时 fix:  修改 ChaoternalRuntimeFallback 拦截逻辑
└── 构建 fix:    修复 Python stub 插入点 + 缓存 key 含 runtime stub hash
```

---

## 6. 质量门

```
□ 如果有代码修改 → dev-trace-enforcement
□ dev-verification-before-completion
□ 更新 STATUS.md
□ 如果涉及翻译路径变更 → wiki 维护
□ 如果涉及多域修改（≥2 Expert）→ 运行 foundation-dll pipeline 集成验证
□ ✅ 全部通过 → 提交并推送
```

## 7. Expert 知识加载

Expert 的知识通过 registry 发现流程加载：

1. 读取 `skills/discovery/skill-index.md`（已预加载）
2. 匹配领域 → 读取对应 `registries/<domain>.md`
3. 找到目标 Expert 的 SKILL.md 路径
4. 读取 `skills/library/skills/<name>/SKILL.md` 获取 domain knowledge

注意：`Skill("dev-xxx-expert")` 在当前环境中**不可用**（Claude Code Skill 工具不支持子技能）。因此 loaded_expert 强制验证已被移除。


## 7b. 跨域技能（独立于域编号表）

以下技能不绑定到单一域，需根据实际涉及域调用：

| 技能 | 覆盖域 | 适用场景 |
|------|--------|---------|
| `dev-il2cpp-external-runtime-expert` | CodeGen(4) + 运行时(1) | extern stub 生成、ShapeRegistry 注册、跨程序集路由 |
| `dev-il2cpp-abi-expert` | CodeGen(4) + 运行时(1) + 构建(7) | ABI 类型映射、返回类型一致性、跨平台调用约定 |

激活方式：通过 discovery 流程读取 SKILL.md。

## 8. Hot Expert 缓存

常用 Expert 可直接引用 `.claude/.hot_skills`，跳过发现链（3 步 → 1 步）。

```json
.hot_skills 中列出的 Expert 可直接读取其 SKILL.md 加载知识，
无需走 skill-index.md → registry → SKILL.md 的发现流程。
```

当前 Hot Expert：
- `dev-il2cpp-runtime-expert`
- `dev-il2cpp-codegen-expert`
- `dev-il2cpp-gc-expert`
- `dev-il2cpp-translation-expert`
- `dev-il2cpp-build-fixer`

---

## 9. 变更记录

| 日期 | 变更 | 原因 |
|------|------|------|
| 2026-06-11 | 初始创建 | 从 CLAUDE.md / core-agent SKILL.md 提取为单一源 |
