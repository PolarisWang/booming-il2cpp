# 路由规则（Routing Rules）— 单一权威源

> 本文是项目路由规则**唯一权威来源**。所有引用路由规则的文件必须引用本文，不得重复定义。

---

## 1. 域编号表

| 编号 | 域 | 说明 | 对应 Expert | 受保护的文件路径 |
|------|----|------|------------|-----------------|
| 1 | 运行时 | runtime-core/interpreter/VTable/bootstrap | `dev-il2cpp-runtime-expert` | `src/native/runtime-core/`, `src/native/interpreter/`, `src/native/bootstrap/`, `src/native/support/` |
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
2. `Skill("dev-xxx-expert")` → 加载 Expert 知识（hook 自动写 `.claude/.loaded_expert`）
3. 编辑域文件 → hook 检查 `.claude/.loaded_expert` 是否匹配

**禁止**：未加载对应 Expert 直接编辑受保护文件路径下的文件。
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

## 7. Sub-Agent 防护

通过 Skill 工具加载子 Expert 时，必须设置 sub-agent 标记以防止递归循环：

```bash
# 进入子 Agent 前
echo "dev-il2cpp-xxx-expert" > .claude/.subagent

# 子 Agent 返回后
rm -f .claude/.subagent
```

hook 会检查：如果在 sub-agent 内再次调用 Skill 工具，将拒绝执行并提示"标记 remaining 后由 Dispatcher 分配"。

## 8. Hot Expert 缓存

常用 Expert 可直接引用 `.claude/.hot_skills`，跳过发现链（3 步 → 1 步）。

```json
.hot_skills 中列出的 Expert 可直接 Skill("dev-xxx-expert") 加载，
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
