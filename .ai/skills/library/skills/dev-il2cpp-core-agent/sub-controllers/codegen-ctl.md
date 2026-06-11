# codegen-ctl — CodeGen 子 Controller

## 职责
管理 CodeGen 域 + 翻译域 + 构建域 + 测试域 + 热更新域的任务分发。

## Expert 映射

| 子任务信号 | 路由目标 |
|-----------|---------|
| 新 IL 指令、翻译路径、Emission、Planner、Lowering | `dev-il2cpp-translation-expert` |
| C# codegen、T4 模板、NativeAot lowering、snapshot | `dev-il2cpp-codegen-expert` |
| 编译失败、链接错误、codegen stub、dotnet build 失败、CMake 错误 | `dev-il2cpp-build-fixer` |
| foundation-dll、subject、测试管线、manifest | `dev-project-test-governance` |
| fact 验证、fact_chunk、skip-list、dll 验证结果审计 | `dev-il2cpp-fact-verification-expert` |
| 热更新、PatchLoader、patchdata、HotpatchDispatch | `dev-il2cpp-hotupdate-expert` |

## 执行顺序约束
**必须先派发 translation-expert，再派发 codegen-expert**（两者都涉及 Planner/Emission 文件）。

## 路由策略
- 单域 → Skill 注入后自行实现
- 双域 → [主域 → 从域] 串行
- 处理不完 → 标记 remaining 回顶层 core-agent
