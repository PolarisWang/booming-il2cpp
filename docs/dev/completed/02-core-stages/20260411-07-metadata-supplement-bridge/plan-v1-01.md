# Metadata Supplement And Bridge Generation 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。

**目标：** 建立 AOT 与热更之间的 supplemental metadata 写入/加载和跨模式桥接代码生成。

**架构：** 扩展 `Chaos.IL2CPP.HotUpdate` 项目，新增 SupplementalMetadataWriter/Loader 和 BridgeGenerator。MetadataWriter 扩展输出 supplemental-metadata-template。

**技术栈：** C# (.NET 8), C/C++

**设计文档：** `phase-architecture-v1-01.md` Phase 6 节

---

## 步骤

### Batch 1: Supplemental Metadata Template

- [ ] **1.1** 扩展 MetadataWriter：输出 `supplemental-metadata-template.json`（AOT 类型/方法的 metadata 槽位模板）
- [ ] **1.2** 定义 template 格式：已注册的 type/method tokens + 预留的 supplement slots
- [ ] **1.3** 在 convert 输出中包含 template（放入 `hot-update/supplemental-metadata-template.json`）

### Batch 2: Supplemental Metadata Writer + Loader

- [ ] **2.1** 实现 `SupplementalMetadataWriter`：从热更 assembly + AOT template → supplement.bin
  - 分析热更 assembly 引用了哪些 AOT 类型/方法
  - 为热更新增的类型生成 metadata registration entries
  - 为 AOT 没有的泛型实例化生成补充 entries
- [ ] **2.2** 实现 native 侧 `SupplementalMetadataLoader`：加载 supplement.bin → 合并到运行时 metadata 表
- [ ] **2.3** 验证：加载 supplement 后，热更类型可通过 token/SubjectId 查询

### Batch 3: Bridge Generation

- [ ] **3.1** 实现 AOT→HotUpdate bridge stub 生成器：
  - AOT virtual/interface call 目标在热更侧 → 生成 bridge 函数路由到 interpreter
- [ ] **3.2** 实现 HotUpdate→AOT 调用路径：
  - interpreter 遇到 AOT method → 直接查找函数指针 → native call
- [ ] **3.3** 实现 delegate cross-mode wrapper：
  - AOT delegate target 指向热更方法 → wrapper 封装 interpreter 调用
- [ ] **3.4** 实现 HotUpdate→Engine 调用路径（通过 Phase 3 engine contract bridge）

### Batch 4: Subject + Verification

- [ ] **4.1** 创建 `subjects/BridgeRoundtripProof/`：
  - AOT 有 `IService.Process(int)` 接口
  - 热更实现 `HotService : IService { Process(x) => x * 2 }`
  - AOT 调 `IService.Process(21)` → 42（bridge → interpreter）
  - 热更调 AOT `Math.Max(1, 2)` → 2（interpreter → AOT）
- [ ] **4.2** 创建 `subjects/GenericSupplementProof/`：
  - AOT 有 `List<int>` 实例化
  - 热更使用 `List<string>`（AOT 没有）
  - supplement 补充 `List<string>` metadata
  - 热更操作 `List<string>` 成功
- [ ] **4.3** delegate cross-mode test：AOT delegate → 热更 target → 正确调用
