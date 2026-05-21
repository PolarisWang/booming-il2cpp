# Hot Update Runtime Skeleton 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。

**目标：** 建立热更运行模式的最小骨架：package loader、runtime mode switch、最小 interpret stub。先在 desktop 验证。

**架构：** 新建 `Chaos.IL2CPP.HotUpdate` managed 项目 + `src/native/hot-update/` native 模块。HotUpdate 只依赖 Contracts，禁止依赖 CodeGen。runtime mode 通过 RuntimeManager 切换 AOT/Mixed 模式。

**技术栈：** C# (.NET 8), C/C++, JSON

**设计文档：** `phase-architecture-v1-01.md` Phase 5 节

---

## 步骤

### Batch 1: Package Format + Loader

- [ ] **1.1** 创建 `Chaos.IL2CPP.HotUpdate` 项目（只依赖 Contracts）
- [ ] **1.2** 定义 `HotUpdatePackage` 数据模型（C#）：packageId, targetAotVersion, assemblies[], supplementalMetadata, signature
- [ ] **1.3** 实现 `PackageReader`：读取 package manifest JSON + 校验 signature（可选）
- [ ] **1.4** 实现 `PackageValidator`：校验 targetAotVersion 兼容性（调用 Phase 0 的 version-policy）
- [ ] **1.5** 创建 `src/native/hot-update/` 项目（CMakeLists.txt + hot_update.h/.cpp）
- [ ] **1.6** 实现 native 侧 package loader：加载 assembly IL 到内存（不走文件 PEReader）

### Batch 2: Runtime Mode Switch

- [ ] **2.1** 定义 `RuntimeMode` 枚举：AOT, Mixed
- [ ] **2.2** 实现 `RuntimeManager`：mode switch 状态机（AOT → Mixed → AOT）
- [ ] **2.3** 在 runtime-core 中添加 mode query：`is_mixed_mode()`
- [ ] **2.4** 验证 mode switch 后 AOT 路径不受影响

### Batch 3: Minimal Interpret Stub

- [ ] **3.1** 实现最小 interpreter stub：只执行 `ldc.i4 <N>; ret` → 返回常量值
- [ ] **3.2** 实现 hot-update method registration：method SubjectId → interpreter entry
- [ ] **3.3** 实现 method dispatch hook：如果 method 注册为 hot-update → 路由到 interpreter stub

### Batch 4: Subject + Verification

- [ ] **4.1** 创建 `subjects/HotUpdateSkeletonProof/`：
  - AOT 主包调 `Helper.GetValue()` → AOT 返回 1
  - 加载 hot update package → 替换 `Helper.GetValue()` 为 interpreter stub 返回 42
  - 调用 → 返回 42
  - 卸载 → 回到 AOT 返回 1
- [ ] **4.2** 验证 AOT-only mode 无回归：不加载 package 时所有现有测试通过
- [ ] **4.3** package corruption test：损坏的 package 被正确拒绝
- [ ] **4.4** enforcement：`test_repo_layout.py` 验证 HotUpdate 不依赖 CodeGen
