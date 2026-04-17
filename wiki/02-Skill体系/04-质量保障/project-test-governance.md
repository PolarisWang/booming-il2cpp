# project-test-governance

> 面向本仓库 subject 测试主线、`Chaos.TestFramework.Sdk/Runtime` 分层、collection file、manifest、Python 模板化测试与 Scriban codegen 的项目级治理 skill。

## 基本信息

- 分层：质量保障
- 项目职责：把测试主线、`.NET 8` collector、manifest、codegen 规则和长期知识入口绑定成一套稳定工程规范
- 实现路径：`.codex/skills/dev-project-test-governance/SKILL.md`
- 状态：`project`

## 核心规则

- managed solution 是唯一测试源
- subject / managed project 只直接引用 `Chaos.TestFramework.Sdk`
- collection file 是 managed/native/hotupdate 的唯一跨执行形态契约
- collection file 统一由 `.NET 8` collector 分析 managed project 中的 `Sdk` 声明后产出
- managed test host、native test host、hotupdate test host 统一消费 collection file
- `subject.features.json` 是 owner subject 与 proof / benchmark obligation 的真源
- native / hotupdate 的执行绑定细节通过 manifest 分层
- bugfix 必须先补自动测试，再动实现
- 测试阶段的 `dotnet` 编译崩溃必须先查根因并修复，不能通过重试、跳过或环境借口绕过
- Python 测试优先模板化，不再长期堆一次性 C# fixture
- file-level codegen 默认 Scriban
- 不保留长期 `Annotation` alias 与旧双轨逻辑
- canonical proof / benchmark / host correctness 不允许使用 `Console.WriteLine` / `ChaosEvidenceKind.Stdout`

## 适用任务

- 调整 `subjects/` 的测试入口、runner、registry 或统一入口
- 修改 `Chaos.TestFramework`
- 修改 managed/native/hotupdate 测试主线
- 修改 benchmark / unit / hotupdate collection 的生成或消费逻辑
- 修改 native dispatch manifest / hotupdate binding manifest
- 修改 benchmark 数据生成与展示逻辑
- 修改生成整文件的 runner 或 codegen emitter

## 关联文档

- [`../../../04-工具与集成/统一测试框架.md`](../../../04-%E5%B7%A5%E5%85%B7%E4%B8%8E%E9%9B%86%E6%88%90/%E7%BB%9F%E4%B8%80%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6.md)
- [`../../../06-测试验证/新增测试接入规范.md`](../../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/%E6%96%B0%E5%A2%9E%E6%B5%8B%E8%AF%95%E6%8E%A5%E5%85%A5%E8%A7%84%E8%8C%83.md)
- [`../../../../docs/architecture/managed-native-hotupdate-test-pipeline.md`](../../../../docs/architecture/managed-native-hotupdate-test-pipeline.md)

## 最近变更

- `2026-04-17`：新增测试阶段 `dotnet` 编译崩溃闸门，要求收集崩溃证据、追根因并修复后再继续验证。
- `2026-04-17`：确认采用 `Annotation -> Sdk` 方案，`Assert` 下沉到 `Sdk`，并要求 roadmap 显式包含旧写法清理与 compatibility alias 删除。
- `2026-04-18`：补充 `subject.features.json` owner/obligation authority、canonical verification 禁止 stdout 判定，以及旧入口协议与旧目录语义 purge contract。
