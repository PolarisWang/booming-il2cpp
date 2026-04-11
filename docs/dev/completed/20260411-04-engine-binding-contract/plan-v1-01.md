# Engine Binding Contract 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。

**目标：** 把引擎接入从 proof-level helper 升级为正式 engine-facing contract，包含 lifecycle、ownership、callback 和 binding generation baseline。

**架构：** 新建 `Chaos.IL2CPP.EngineBinding` managed 项目（只依赖 Contracts）+ `src/native/engine-bridge/` native 模块。定义 `contracts/engine/v0/` ABI headers。禁止依赖 pipeline 内部模块。

**技术栈：** C# (.NET 8), C/C++, Python (tests)

**设计文档：** `phase-architecture-v1-01.md` Phase 3 节

---

## 步骤

### Batch 1: Engine ABI Contract

- [ ] **1.1** 创建 `contracts/engine/v0/` 目录
- [ ] **1.2** 编写 `contracts/engine/v0/lifecycle.h`：`engine_init`, `engine_shutdown`, `engine_reload`, `engine_scene_attach`
- [ ] **1.3** 编写 `contracts/engine/v0/ownership.h`：`engine_retain_object`, `engine_release_object`, GCHandle ↔ engine ref 协议
- [ ] **1.4** 编写 `contracts/engine/v0/callback.h`：`engine_register_callback`, `engine_dispatch_callback`, callback signature typedef
- [ ] **1.5** 编写 `contracts/engine/v0/thread-model.md`：主线程约束、engine_post_to_main_thread、IsMainThreadLane

### Batch 2: Native Engine Bridge

- [ ] **2.1** 创建 `src/native/engine-bridge/` 项目（CMakeLists.txt + engine_bridge.h/.cpp）
- [ ] **2.2** 实现 lifecycle hook 注册/派发（从 runtime-core 的现有 EngineLifecycle* 迁移并扩展）
- [ ] **2.3** 实现 ownership bridge（GCHandle ↔ engine native ref 双向映射）
- [ ] **2.4** 实现 callback dispatch（engine C++ → managed C# + managed → engine C++）
- [ ] **2.5** 实现 engine mock host（最小 C++ 宿主完成 init → register_callback → dispatch → shutdown）

### Batch 3: Managed EngineBinding + Binding Generation

- [ ] **3.1** 创建 `Chaos.IL2CPP.EngineBinding` 项目（只依赖 Contracts）
- [ ] **3.2** 定义 engine binding attribute：`[EngineExport]`, `[EngineCallback]`
- [ ] **3.3** 实现 binding code generator baseline：从 `[EngineExport]` 接口 → C++ wrapper stub
- [ ] **3.4** 验证：生成的 wrapper 可编译

### Batch 4: Subject + Verification

- [ ] **4.1** 创建 `subjects/EngineHostProof/`：完整 lifecycle + ownership + callback 验证
- [ ] **4.2** 现有 Engine* subjects 继续通过
- [ ] **4.3** 扩展 `HostEmbeddingLite` 使用新 ownership 协议
- [ ] **4.4** lifecycle stress test：1000 次 init/shutdown 无泄漏
- [ ] **4.5** enforcement：`test_repo_layout.py` 验证 EngineBinding 不依赖 pipeline 内部模块
